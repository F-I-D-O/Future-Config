#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

struct Array_config {
	std::vector<int> array;

	explicit Array_config(const YAML::Node& yaml_config)
	{
		const YAML::Node& yaml_array = yaml_config["array"];
		for (const auto& yaml_item: yaml_array) {
			array.push_back(yaml_item.as<int>());
		}
	};
};