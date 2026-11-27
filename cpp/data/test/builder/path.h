#pragma once

#include <future-config/Config_object.h>
#include <filesystem>
#include <string>

struct Path_config {
	std::filesystem::path out_dir;

	explicit Path_config(const fc::Config_object& config_object):
		out_dir(config_object.get<std::filesystem::path>("out_dir"))
	{};
};
