#pragma once

#include <future-config/Config_object.h>
#include <string>

struct Parent_config {
	std::string var;

	explicit Parent_config(const fc::Config_object& config_object):
		var(config_object.get<std::string>("var"))
	{};
};

struct Dependency_outside_bind_path_test_config {
	Parent_config parent_config;

	explicit Dependency_outside_bind_path_test_config(const fc::Config_object& config_object):
		parent_config(config_object.get<fc::Config_object&>("parent_config"))
	{};
};
