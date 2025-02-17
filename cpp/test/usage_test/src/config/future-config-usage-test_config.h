#pragma once

#include <future-config/Config_object.h>
#include <string>


struct Future_config_usage_test_config {
	int test_par;

	explicit Future_config_usage_test_config(const fc::Config_object& config_object):
		test_par(config_object.get<int>("test_par"))
	{};
};

