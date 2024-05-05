#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

struct Comparison {
	std::vector<std::string> experiment_names;

	explicit Comparison(const YAML::Node& yaml_config)
	{
		for (const auto& yaml_item: yaml_config["experiment_names"]) {
			experiment_names.push_back(yaml_item.as<std::string>());
		}
	};
};

struct String_array_in_object_config {
	Comparison comparison;

	explicit String_array_in_object_config(const YAML::Node& yaml_config):
		comparison(yaml_config["comparison"])
	{};
};