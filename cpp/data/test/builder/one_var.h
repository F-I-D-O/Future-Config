//
// Created by david on 2024-04-28.
//

#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

struct One_var_config {
	std::string var;

	One_var_config(const YAML::Node& config) {
		var = config["var"].as<std::string>();
	}
};