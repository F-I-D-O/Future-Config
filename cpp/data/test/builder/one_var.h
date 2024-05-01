#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

struct One_var_config {
	std::string var;

	explicit One_var_config(const YAML::Node& yaml_config):
		var(yaml_config["var"].as<std::string>())
	{};
};