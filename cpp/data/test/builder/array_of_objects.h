#pragma once

#include <future-config/Config_object.h>
#include <string>

struct Array_of_objects_item {
	std::string property;

	explicit Array_of_objects_item(const fc::Config_object& config_object):
		property(config_object.get<std::string>("property"))
	{};
};

struct Array_of_objects_config {
	std::vector<Array_of_objects_item> array_of_objects;

	explicit Array_of_objects_config(const fc::Config_object& config_object){
		for (const auto& item: config_object.get_array<fc::Config_object>("array_of_objects")) {
			array_of_objects.emplace_back(item);
		}
	};
};