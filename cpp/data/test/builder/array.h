#pragma once

#include <Config_object.h>
#include <string>

struct Array_config {
	std::vector<int> array;

	explicit Array_config(const fc::Config_object& config_object){
		for (const auto& item: config_object.get_array<int>("array")) {
			array.emplace_back(item);
		}
	};
};