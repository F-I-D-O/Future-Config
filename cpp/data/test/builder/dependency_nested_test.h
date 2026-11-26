#pragma once

#include <future-config/Config_object.h>
#include <string>

#include "dependency.h"

struct Libraries {
	Parent_config parent_config;

	explicit Libraries(const fc::Config_object& config_object):
		parent_config(config_object.get<fc::Config_object&>("parent_config"))
	{};
};

struct Dependency_nested_test_config {
	Libraries libraries;

	explicit Dependency_nested_test_config(const fc::Config_object& config_object):
		libraries(config_object.get<fc::Config_object&>("libraries"))
	{};
};
