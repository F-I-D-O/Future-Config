#pragma once

#include <future-config/Config_object.h>
#include <string>

struct Test_project_name_config {
	std::string var;

	explicit Test_project_name_config(const fc::Config_object& config_object):
		var(config_object.get<std::string>("var"))
	{};
};