#pragma once

#include <future-config/Config_object.h>
#include <string>

struct One_var_config {
	std::string var;

	explicit One_var_config(const fc::Config_object& config_object):
		var(config_object.get<std::string>("var"))
	{};
};