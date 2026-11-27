//
// Created by Fido on 2024-04-14.
//

#include <fstream>
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

// text representation of a scalar value
std::string get_scalar_string(const fkyaml::node& node) {
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
	throw std::runtime_error(format::format("Unsupported scalar type: {}", get_type_name(node)));
}

Config_object create_config_object(const fkyaml::node& yaml_config) {
	Config_object config_object;
	for(auto it = yaml_config.begin(); it != yaml_config.end(); ++it) {
		const auto key = get_scalar_string(it.key());
		const auto& value = it.value();

		if(value.is_mapping()) {
			config_object.emplace(key, std::make_unique<Config_object>(create_config_object(value)));
		}
		else if(value.is_sequence()) {
			if(value.empty() || !value[0].is_scalar() || value[0].is_null()) {
				throw std::runtime_error(format::format("Unsupported as array member type"));
			}
			std::vector<std::string> items;
			for(const auto& item: value) {
				items.push_back(get_scalar_string(item));
			}
			config_object.emplace(key, std::move(items));
		}
		else if(!value.is_null()) {
			config_object.emplace(key, get_scalar_string(value));
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
	return create_config_object(yaml_content);
}

} // namespace


Config_object Parser::parse(const std::string& yaml_content) {
	return create_config_object_from_document(fkyaml::node::deserialize(yaml_content));
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
