#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

struct Array_of_objects_item {
	std::string property;

	explicit Array_of_objects_item(const YAML::Node& yaml_config):
		property(yaml_config["property"].as<std::string>())
	{};
};

struct Array_of_objects_config {
	std::vector<Array_of_objects_item> array_of_objects;

	explicit Array_of_objects_config(const YAML::Node& yaml_config){
		for (const auto& yaml_item: yaml_config["array_of_objects"]) {
			array_of_objects.emplace_back(yaml_item);
		}
	};
};