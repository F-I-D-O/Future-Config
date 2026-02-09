#pragma once

#include <future-config/Config_object.h>
#include <string>

struct Test_config {
	int test_par;

	explicit Test_config(const fc::Config_object& config_object):
		test_par(config_object.get<int>("test_par"))
	{};
};
