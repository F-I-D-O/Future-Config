#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

#include "dependency.h"

struct Dependency_test_config {
	Parent_config parent_config;

	explicit Dependency_test_config(const YAML::Node& yaml_config):
		parent_config(yaml_config["parent_config"])
	{};
};