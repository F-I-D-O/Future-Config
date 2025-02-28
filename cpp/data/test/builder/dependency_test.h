#pragma once

#include <future-config/Config_object.h>
#include <string>

#include "dependency.h"

struct Dependency_test_config {
	Parent_config parent_config;

	explicit Dependency_test_config(const fc::Config_object& config_object):
		parent_config(config_object.get<fc::Config_object&>("parent_config"))
	{};
};