#pragma once

#include <future-config/Config_object.h>
#include <string>

struct Hyphen_in_key_config {
	int test_var;

	explicit Hyphen_in_key_config(const fc::Config_object& config_object):
		test_var(config_object.get<int>("test-var"))
	{};
};
