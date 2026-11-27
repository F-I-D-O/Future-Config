//
// Created by Fido on 2024-04-14.
//

#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include <fkYAML/node.hpp>

#include "future-config/format.h"
#include "Parser.h"
#include "future-config/common.h"


namespace fc {

namespace {

std::string get_type_name(const fkyaml::node& node) {
	switch(node.get_type()) {
		case fkyaml::node_type::SEQUENCE:
			return "Sequence";
		case fkyaml::node_type::MAPPING:
			return "Map";
		case fkyaml::node_type::NULL_OBJECT:
			return "Null";
		default:
			return "Scalar";
	}
}

// text representation of a mapping key
std::string get_key_string(const fkyaml::node& node) {
	if(node.is_string()) {
		return node.get_value<std::string>();
	}
	if(node.is_integer()) {
		return std::to_string(node.get_value<std::int64_t>());
	}
	if(node.is_float_number()) {
		return format::format("{}", node.get_value<double>());
	}
	if(node.is_boolean()) {
		return node.get_value<bool>() ? "true" : "false";
	}
	throw std::runtime_error(format::format("Unsupported type of a key: {}", get_type_name(node)));
}

// Creates a scalar value of the type determined by the YAML library
Config_scalar create_scalar(const fkyaml::node& node, const std::string& key_path) {
	if(node.is_string()) {
		return node.get_value<std::string>();
	}
	if(node.is_integer()) {
		try {
			return node.get_value<std::int64_t>();
		} catch(const fkyaml::exception&) {
			throw std::runtime_error(format::format(
				"The value of the property {} is an integer that is too large (the maximum is {}).",
				key_path,
				std::numeric_limits<std::int64_t>::max()
			));
		}
	}
	if(node.is_float_number()) {
		return node.get_value<double>();
	}
	if(node.is_boolean()) {
		return node.get_value<bool>();
	}
	if(node.is_null()) {
		throw std::runtime_error(format::format(
			"The property {} has no value (null). Null values are not supported.",
			key_path
		));
	}
	throw std::runtime_error(format::format(
		"The property {} contains an unsupported value: {}. Arrays can only contain scalar values.",
		key_path,
		get_type_name(node)
	));
}

Config_object create_config_object(const fkyaml::node& yaml_config, const std::string& parent_path) {
	Config_object config_object;
	for(auto it = yaml_config.begin(); it != yaml_config.end(); ++it) {
		const auto key = get_key_string(it.key());
		const auto key_path = parent_path.empty() ? key : parent_path + "." + key;
		const auto& value = it.value();

		if(value.is_mapping()) {
			config_object.emplace(key, std::make_unique<Config_object>(create_config_object(value, key_path)));
		}
		else if(value.is_sequence()) {
			if(value.empty()) {
				throw std::runtime_error(format::format(
					"The property {} is an empty array. Empty arrays are not supported, because the type of the "
					"array items cannot be determined.",
					key_path
				));
			}
			std::vector<Config_scalar> items;
			for(const auto& item: value) {
				items.push_back(create_scalar(item, key_path));
			}
			config_object.emplace(key, std::move(items));
		}
		else {
			config_object.emplace(key, create_scalar(value, key_path));
		}
	}
	return config_object;
}

Config_object create_config_object_from_document(const fkyaml::node& yaml_content) {
	// check that the root object is of the correct type
	if(!yaml_content.is_mapping()) {
		throw std::runtime_error(format::format(
			"The root node of configuration file must be a map. Root node: {}",
			get_type_name(yaml_content)
		));
	}
	return create_config_object(yaml_content, "");
}

} // namespace


Config_object Parser::parse(const std::string& yaml_content) {
	return create_config_object_from_document(fkyaml::node::deserialize(yaml_content));
}

Config_scalar Parser::parse_scalar(const std::string& text) {
	// Integers, floating point numbers, and booleans never contain a white space. Other texts are not passed to the
	// YAML library, as they could be interpreted as something else than a scalar (e.g., a comment).
	if(text.empty() || text.find_first_of(" \t\r\n") != std::string::npos) {
		return text;
	}

	fkyaml::node node;
	try {
		node = fkyaml::node::deserialize(text);
	} catch(const fkyaml::exception&) {
		// not a valid YAML document, so it is not a number or a boolean
		return text;
	}

	if(node.is_integer() || node.is_float_number() || node.is_boolean()) {
		return create_scalar(node, text);
	}
	return text;
}

Config_object Parser::parse(const fs::path& yaml_file_path) {
	auto canonical_path = check_path(yaml_file_path);
	std::ifstream file(canonical_path);
	if(!file) {
		throw std::runtime_error(format::format("Cannot open the configuration file: {}", canonical_path.string()));
	}
	return create_config_object_from_document(fkyaml::node::deserialize(file));
}


}
