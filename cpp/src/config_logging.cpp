#include <algorithm>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "spdlog/spdlog.h"

#include "future-config/format.h"
#include "future-config/loading.h"

namespace fc {

namespace {

const std::unordered_map<std::string, Config_log_level> log_level_names{
	{"none", Config_log_level::NONE},
	{"sources", Config_log_level::SOURCES},
	{"values", Config_log_level::VALUES},
	{"chain", Config_log_level::CHAIN}
};

// lines of the configuration hierarchy
const std::string hierarchy_item = "├── ";
const std::string hierarchy_last_item = "└── ";
const std::string hierarchy_indent = "│   ";
const std::string hierarchy_last_indent = "    ";

// lines of the overwrite chain of a single value
const std::string chain_item = "┆╌╌ ";

struct Source {
	std::string type;
	std::string location;
	std::string bind_path;
};

// value provided by a single source
struct Value_record {
	unsigned source_number;
	std::string value;
};

// line of the value tree. The origin is kept separately, so that the origins can be aligned in a single column.
struct Tree_line {
	std::string text;
	std::string origin;
};

std::string get_type_name(Config_type type) {
	switch(type) {
		case Config_type::MAIN:
			return "main";
		case Config_type::DEPENDENCY:
			return "dependency";
		case Config_type::LOCAL:
			return "local";
		case Config_type::COMMAND_LINE:
			return "command line";
	}
	throw std::runtime_error("Unknown config type");
}

// Number of characters of an UTF-8 string (the tree lines are not ASCII, so the number of bytes cannot be used).
std::size_t get_width(const std::string& text) {
	return std::ranges::count_if(text, [](const char character) {
		return (static_cast<unsigned char>(character) & 0xC0) != 0x80;
	});
}

std::string pad(const std::string& text, std::size_t width) {
	return text + std::string(width - get_width(text), ' ');
}

std::string get_path_string(const std::string& parent_path, const std::string& key) {
	return parent_path.empty() ? key : parent_path + "." + key;
}

bool is_object(const config_property_value& property) {
	return std::holds_alternative<config_object_property_value>(property);
}

// string representation of a scalar or array value
std::string get_value_string(const config_property_value& property) {
	if(std::holds_alternative<std::string>(property)) {
		return std::get<std::string>(property);
	}

	std::string array_string = "[";
	for(const auto& item: std::get<std::vector<std::string>>(property)) {
		if(array_string.size() > 1) {
			array_string += ", ";
		}
		array_string += item;
	}
	return array_string + "]";
}

std::vector<std::string> get_sorted_keys(const Config_object& config) {
	std::vector<std::string> keys;
	for(const auto& [key, value]: config) {
		keys.push_back(key);
	}
	std::ranges::sort(keys);
	return keys;
}

} // namespace


std::optional<Config_log_level> get_command_line_log_level(const Command_line_config_definition& command_line_config) {
	const auto argument = "--" + config_log_argument_name;
	for(int i = 1; i < command_line_config.argc; ++i) {
		if(argument != command_line_config.argv[i]) {
			continue;
		}

		const std::string level_names = "none, sources, values, chain";
		if(i + 1 >= command_line_config.argc) {
			throw std::runtime_error(format::format(
				"Command line argument {} requires a value, one of: {}.",
				argument,
				level_names
			));
		}
		const std::string level_name = command_line_config.argv[i + 1];
		if(!log_level_names.contains(level_name)) {
			throw std::runtime_error(format::format(
				"Unknown value of the command line argument {}: '{}'. Supported values: {}.",
				argument,
				level_name,
				level_names
			));
		}
		return log_level_names.at(level_name);
	}
	return std::nullopt;
}


struct Config_load_log::Data {
	Config_log_level level;

	std::vector<Source> sources;

	// values provided by the sources, in the load order. The key is the path to the value from the root object.
	std::map<std::string, std::vector<Value_record>> value_records;

	void add_values(unsigned source_number, const Config_object& config, const std::string& parent_path) {
		for(const auto& [key, property]: config) {
			const auto path = get_path_string(parent_path, key);
			if(is_object(property)) {
				add_values(source_number, *std::get<config_object_property_value>(property), path);
			}
			else {
				value_records[path].push_back({source_number, get_value_string(property)});
			}
		}
	}

	void add_tree_lines(
		const Config_object& config,
		const std::string& parent_path,
		const std::string& indent,
		std::vector<Tree_line>& lines
	) const {
		const auto keys = get_sorted_keys(config);
		for(std::size_t i = 0; i < keys.size(); ++i) {
			const auto& key = keys[i];
			const auto& property = config[key];
			const bool last = i == keys.size() - 1;
			const auto path = get_path_string(parent_path, key);
			const auto item_prefix = indent + (last ? hierarchy_last_item : hierarchy_item);
			const auto child_indent = indent + (last ? hierarchy_last_indent : hierarchy_indent);

			if(is_object(property)) {
				lines.push_back({item_prefix + key, ""});
				add_tree_lines(*std::get<config_object_property_value>(property), path, child_indent, lines);
				continue;
			}

			const auto value = get_value_string(property);
			const auto& records = value_records.at(path);

			// the value comes from the last source that provided it
			const auto& origin_record = records.back();
			auto origin = format::format("[{}]", origin_record.source_number);

			// the value in the source differs from the final value if it contains variables
			const bool resolved = origin_record.value != value;

			if(level == Config_log_level::VALUES && resolved) {
				origin += " " + origin_record.value;
			}
			lines.push_back({format::format("{}{}: {}", item_prefix, key, value), origin});

			if(level == Config_log_level::CHAIN && (records.size() > 1 || resolved)) {
				for(const auto& record: records) {
					lines.push_back({
						format::format("{}{}[{}] {}", child_indent, chain_item, record.source_number, record.value),
						""
					});
				}
			}
		}
	}
};

Config_load_log::Config_load_log(Config_log_level level): data(std::make_unique<Data>()) {
	data->level = level;
}

Config_load_log::~Config_load_log() = default;

void Config_load_log::add_source(const Config_definition_base& config_definition) {
	if(data->level == Config_log_level::NONE) {
		return;
	}

	std::string location;
	if(config_definition.type != Config_type::COMMAND_LINE) {
		location = dynamic_cast<const Config_definition&>(config_definition).yaml_file_path.string();
	}
	data->sources.push_back({get_type_name(config_definition.type), location, config_definition.bind_path});
}

void Config_load_log::log_sources() const {
	if(data->level == Config_log_level::NONE) {
		return;
	}

	std::size_t type_width = 0;
	std::size_t location_width = 0;
	for(const auto& source: data->sources) {
		type_width = std::max(type_width, get_width(source.type));
		location_width = std::max(location_width, get_width(source.location));
	}

	std::string message = "Future Config sources (in load order; a source with a higher number has higher priority and "
		"overrides the lower ones):";
	unsigned source_number = 1;
	for(const auto& source: data->sources) {
		auto line = format::format("[{}] {}", source_number, source.type);
		if(!source.location.empty() || !source.bind_path.empty()) {
			line = pad(line, get_width(line) + type_width - get_width(source.type)) + "  " + source.location;
		}
		if(!source.bind_path.empty()) {
			line += std::string(location_width - get_width(source.location), ' ') + "  -> " + source.bind_path;
		}
		message += "\n" + line;
		++source_number;
	}
	spdlog::info("{}", message);
}

void Config_load_log::add_values(unsigned source_number, const Config_object& config) {
	if(data->level == Config_log_level::NONE || data->level == Config_log_level::SOURCES) {
		return;
	}
	data->add_values(source_number, config, "");
}

void Config_load_log::log_values(const Config_object& final_config) const {
	if(data->level == Config_log_level::NONE || data->level == Config_log_level::SOURCES) {
		return;
	}

	std::vector<Tree_line> lines;
	data->add_tree_lines(final_config, "", "", lines);

	// the origins are aligned in a single column
	std::size_t text_width = 0;
	for(const auto& line: lines) {
		if(!line.origin.empty()) {
			text_width = std::max(text_width, get_width(line.text));
		}
	}

	std::string message = "Future Config values ([n] = source the value comes from";
	if(data->level == Config_log_level::CHAIN) {
		message += "; in the overwrite chains, the highest number wins";
	}
	message += "):\nConfiguration";
	for(const auto& line: lines) {
		message += "\n";
		message += line.origin.empty() ? line.text : pad(line.text, text_width) + "  " + line.origin;
	}
	spdlog::info("{}", message);
}

} // namespace fc
