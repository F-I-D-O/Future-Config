#pragma once

#include <future-config/Config_object.h>
#include <string>

#include "parent-lib/config/parent-lib_config.h"

struct Libraries {
	Parent_lib_config parent_config;

	explicit Libraries(const fc::Config_object& config_object):
		parent_config(config_object.get<fc::Config_object&>("parent_config"))
	{};
};

struct Dependency_from_definitions_test_config {
	Libraries libraries;

	explicit Dependency_from_definitions_test_config(const fc::Config_object& config_object):
		libraries(config_object.get<fc::Config_object&>("libraries"))
	{};
};
