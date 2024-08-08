//
// Created by david on 2024-06-07.
//

#pragma once

#include <string>
#include <filesystem>
#include <spdlog/spdlog.h>
#include "yaml-cpp/yaml.h"

#include "Config_object.h"
#include "common.h"
#include "future-config_export.h"
    
    
namespace fc {    

    


template<class C>
concept Config_class = requires(C c, const Config_object& config) {
	// requires a constructor that takes a Config_object
	C(config);
};



template<Config_class C>
FUTURE_CONFIG_EXPORT C load(std::vector<std::unique_ptr<Config_definition>>& config_definitions, const fs::path& local_config_path);

template<Config_class C>
C load(std::vector<std::unique_ptr<Config_definition>>& config_definitions, const fs::path& local_config_path) {
	try{
		// check if local config file exists
		check_path(local_config_path);
	} catch(const std::runtime_error& e) {
		throw std::runtime_error(std::format("There is a problem with the user configuration: {}", e.what()));
	}

	// check master config definitions
	for(unsigned i = 0; i < config_definitions.size(); i++) {
		const auto& config_definition = config_definitions[i];
		try{
			check_path(config_definition->yaml_file_path);
		} catch(const std::runtime_error& e) {
			throw std::runtime_error(
				std::format("There is a problem with the {}. master configuration: {}", i + 1, e.what()));
		}
	}

	// add local config definition
	config_definitions.emplace_back(std::make_unique<Config_definition>(
		Config_type::LOCAL,
		local_config_path
	));

	// config loading
	auto config_object = load_config(config_definitions);

	return C(config_object);

}

}
    