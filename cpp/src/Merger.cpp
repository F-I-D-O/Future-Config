//
// Created by david on 2024-05-08.
//

#include "Merger.h"
    
    
namespace fc {    

    

Config_object Merger::merge(std::vector<Config_object>& configs) {
	// Copy the default config here
	Config_object final_config_data = configs[0];

	for(int i = 1; i < configs.size(); ++i) {
		override_level(final_config_data, configs[i]);
	}

	return std::move(final_config_data);
}

Config_object Merger::merge(Config_object& base_config, Config_object& override_config) {
    std::vector<Config_object> configs = {base_config, override_config};
    return merge(configs);
}

void Merger::override_level(Config_object& config, Config_object& overriding_config) {
	for(auto&[key, value]: overriding_config) {
		if(std::holds_alternative<config_object_property_value>(value) && config.contains(key)) {
			override_level(*std::get<config_object_property_value>(config[key]), *std::get<config_object_property_value >(value));
		}
		else{
			config.emplace(key, std::move(value));
		}
	}
}

        
}
    