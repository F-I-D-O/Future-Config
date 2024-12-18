//
// Created by david on 2024-05-08.
//

#include "Merger.h"
    
    
namespace fc {    

    

Config_object Merger::merge(const std::vector<Config_object>& configs) {
	auto final_config_data = configs[0];
	for(int i = 1; i < configs.size(); ++i) {
		override_level(final_config_data, configs[i]);
	}

	return final_config_data;
}

void Merger::override_level(Config_object& config, const Config_object& overriding_config) {
	auto& final_config = config;

	for(const auto&[key, value]: overriding_config) {
		if(std::holds_alternative<Config_object>(*value) && config.contains(key)) {
			override_level(std::get<Config_object>(config[key]), std::get<Config_object>(*value));
		}
		else{
			config[key] = *value;
		}
	}
}

        
}
    