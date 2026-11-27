#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "future-config/format.h"
#include "future-config/loading.h"
#include "Parser.h"
#include "Resolver.h"

namespace fc {

namespace {

std::string get_path_string(const std::string& parent_path, const std::string& key) {
	return parent_path.empty() ? key : parent_path + "." + key;
}

bool is_scalar(const config_property_value& property) {
	return std::holds_alternative<Config_scalar>(property);
}

bool is_array(const config_property_value& property) {
	return std::holds_alternative<std::vector<Config_scalar>>(property);
}

bool is_object(const config_property_value& property) {
	return std::holds_alternative<config_object_property_value>(property);
}

// Checks if the value is a string with a variable. The type of such value is not known until the variable is resolved.
bool has_variable(const Config_scalar& scalar) {
	return std::holds_alternative<std::string>(scalar) && Resolver::contains_variable(std::get<std::string>(scalar));
}

std::string get_kind_name(const config_property_value& property) {
	if(is_object(property)) {
		return "object";
	}
	if(is_array(property)) {
		return format::format("array of {}", get_type_name(std::get<std::vector<Config_scalar>>(property).front()));
	}
	return get_type_name(std::get<Config_scalar>(property));
}

[[noreturn]] void throw_type_error(
	const std::string& path,
	const std::string& source_description,
	const std::string& value_description,
	const std::string& expected_type
) {
	throw std::runtime_error(format::format(
		"Property '{}' from {} has a wrong type: the value is {}, but the type of the property is {}.",
		path,
		source_description,
		value_description,
		expected_type
	));
}

/**
 * Converts the scalar value to the type of the reference value.
 * @return the converted value
 */
Config_scalar convert_scalar(
	const Config_scalar& value,
	const Config_scalar& reference,
	const std::string& path,
	const std::string& source_description
) {
	// same type
	if(value.index() == reference.index()) {
		return value;
	}

	// any scalar is accepted as a string, using its text representation
	if(std::holds_alternative<std::string>(reference)) {
		return to_string(value);
	}

	// strings are accepted as paths
	if(std::holds_alternative<std::filesystem::path>(reference) && std::holds_alternative<std::string>(value)) {
		return std::filesystem::path(std::get<std::string>(value));
	}

	// integers are accepted as floating point numbers
	if(std::holds_alternative<double>(reference) && std::holds_alternative<std::int64_t>(value)) {
		return static_cast<double>(std::get<std::int64_t>(value));
	}

	throw_type_error(
		path,
		source_description,
		format::format("'{}' ({})", to_string(value), get_type_name(value)),
		get_type_name(reference)
	);
}

void convert_types(
	Config_object& config,
	const Config_object& type_reference,
	const std::string& source_description,
	const std::string& parent_path
) {
	for(auto& [key, property]: config) {
		// properties that are not in the reference keep their types
		if(!type_reference.contains(key)) {
			continue;
		}
		const auto& reference_property = type_reference[key];
		const auto path = get_path_string(parent_path, key);

		if(is_object(reference_property)) {
			if(!is_object(property)) {
				throw_type_error(path, source_description, get_kind_name(property), "object");
			}
			convert_types(
				*std::get<config_object_property_value>(property),
				*std::get<config_object_property_value>(reference_property),
				source_description,
				path
			);
		}
		else if(is_array(reference_property)) {
			if(!is_array(property)) {
				throw_type_error(path, source_description, get_kind_name(property), get_kind_name(reference_property));
			}
			const auto& reference_item = std::get<std::vector<Config_scalar>>(reference_property).front();
			for(auto& item: std::get<std::vector<Config_scalar>>(property)) {
				item = convert_scalar(item, reference_item, path, source_description);
			}
		}
		else {
			if(!is_scalar(property)) {
				throw_type_error(path, source_description, get_kind_name(property), get_kind_name(reference_property));
			}
			auto& scalar = std::get<Config_scalar>(property);

			// the values with variables are converted after the variables are resolved
			if(has_variable(scalar)) {
				continue;
			}
			scalar = convert_scalar(scalar, std::get<Config_scalar>(reference_property), path, source_description);
		}
	}
}

void check_master_types(const Config_object& config, const std::string& source_description, const std::string& parent_path) {
	for(const auto& [key, property]: config) {
		const auto path = get_path_string(parent_path, key);
		if(is_object(property)) {
			check_master_types(*std::get<config_object_property_value>(property), source_description, path);
		}
		else if(is_array(property)) {
			const auto& items = std::get<std::vector<Config_scalar>>(property);
			for(const auto& item: items) {
				if(item.index() != items.front().index()) {
					throw std::runtime_error(format::format(
						"Property '{}' from {} is an array with items of different types ({} and {}). All items of an "
						"array must have the same type.",
						path,
						source_description,
						get_type_name(items.front()),
						get_type_name(item)
					));
				}
			}
		}
	}
}

// Finds the paths to all values with variables
void find_values_with_variables(
	const Config_object& config,
	std::vector<std::string>& current_path,
	std::vector<std::vector<std::string>>& paths
) {
	for(const auto& [key, property]: config) {
		current_path.push_back(key);
		if(is_object(property)) {
			find_values_with_variables(*std::get<config_object_property_value>(property), current_path, paths);
		}
		else if(is_scalar(property) && has_variable(std::get<Config_scalar>(property))) {
			paths.push_back(current_path);
		}
		current_path.pop_back();
	}
}

// Returns the scalar located at the path in the config, or nullptr if there is no scalar at the path
const Config_scalar* find_scalar(const Config_object& config, const std::vector<std::string>& path) {
	const Config_object* current_object = &config;
	for(std::size_t i = 0; i < path.size(); ++i) {
		if(!current_object->contains(path[i])) {
			return nullptr;
		}
		const auto& property = (*current_object)[path[i]];
		if(i == path.size() - 1) {
			return is_scalar(property) ? &std::get<Config_scalar>(property) : nullptr;
		}
		if(!is_object(property)) {
			return nullptr;
		}
		current_object = std::get<config_object_property_value>(property).get();
	}
	return nullptr;
}

Config_scalar& get_scalar(Config_object& config, const std::vector<std::string>& path) {
	Config_object* current_object = &config;
	for(std::size_t i = 0; i < path.size() - 1; ++i) {
		current_object = std::get<config_object_property_value>((*current_object)[path[i]]).get();
	}
	return std::get<Config_scalar>((*current_object)[path.back()]);
}

} // namespace


std::string get_source_description(const Config_definition_base& config_definition) {
	if(config_definition.type == Config_type::COMMAND_LINE) {
		return "command line arguments";
	}
	return format::format(
		"config file {}",
		dynamic_cast<const Config_definition&>(config_definition).yaml_file_path.string()
	);
}

void check_master_types(const Config_object& config, const std::string& source_description) {
	check_master_types(config, source_description, "");
}

Config_object get_type_reference(const Config_object& master_config) {
	Config_object type_reference = master_config;
	process_variables(type_reference, nullptr);
	return type_reference;
}

void convert_types(Config_object& config, const Config_object& type_reference, const std::string& source_description) {
	convert_types(config, type_reference, source_description, "");
}

void process_variables(Config_object& config, const Config_object* type_reference) {
	std::vector<std::string> current_path;
	std::vector<std::vector<std::string>> paths;
	find_values_with_variables(config, current_path, paths);

	Resolver(config).resolve();

	// the values with resolved variables are strings, their types have to be determined
	for(const auto& path: paths) {
		auto& scalar = get_scalar(config, path);
		const auto& text = std::get<std::string>(scalar);

		std::string path_string;
		for(const auto& key: path) {
			path_string = get_path_string(path_string, key);
		}

		const Config_scalar* reference = type_reference == nullptr ? nullptr : find_scalar(*type_reference, path);
		if(reference == nullptr) {
			// unknown property: the type is determined from the resolved value
			scalar = Parser().parse_scalar(text);
		}
		else if(!std::holds_alternative<std::string>(*reference)) {
			scalar = convert_scalar(
				Parser().parse_scalar(text),
				*reference,
				path_string,
				"the variable substitution"
			);
		}
	}
}

} // namespace fc
