#pragma once

#include <future-config/Config_object.h>
#include <string>

struct Comparison {
	std::vector<std::string> experiment_names;

	explicit Comparison(const fc::Config_object& config_object){
		for (const auto& item: config_object.get_array<std::string>("experiment_names")) {
			experiment_names.emplace_back(item);
		}
	};
};

struct String_array_in_object_config {
	Comparison comparison;

	explicit String_array_in_object_config(const fc::Config_object& config_object):
		comparison(config_object.get<fc::Config_object>("comparison"))
	{};
};