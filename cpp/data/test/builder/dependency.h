#pragma once

#include <Config_object.h>
#include <string>

struct Parent_config {
	std::string parent_var;

	explicit Parent_config(const fc::Config_object& config_object):
		parent_var(config_object.get<std::string>("var"))
	{};
};