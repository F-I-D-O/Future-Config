#pragma once

#include <future-config/Config_object.h>
#include <filesystem>
#include <string>

struct Path_array_config {
	std::vector<std::filesystem::path> files;

	explicit Path_array_config(const fc::Config_object& config_object){
		for (const auto& item: config_object.get_array<std::filesystem::path>("files")) {
			files.emplace_back(item);
		}
	};
};
