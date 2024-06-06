#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

struct Parent_config {
	std::string parent_var;

	explicit Parent_config(const YAML::Node& yaml_config):
		parent_var(yaml_config["var"].as<std::string>())
	{};
};