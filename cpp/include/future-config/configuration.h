//
// Created by david on 2024-06-07.
//

#pragma once

#include <filesystem>

#include "future-config/format.h"
#include "future-config/Config_object.h"
#include "future-config/common.h"
    
    
namespace fc {    

template<class C>
concept Config_class = requires(C c, const Config_object& config) {
	// requires a constructor that takes a Config_object
	C(config);
};

/**
 * Load function that uses the provided config definitions. This is the most generic way to load a configuration.
 * Other functions are provided that use conventional load order, all of which call this function.
 *
 * @tparam C root config class
 * @param config_definitions list of config definitions
 * @return root config object filled with the configuration data
 */
template<Config_class C>
C load(std::vector<std::unique_ptr<Config_definition_base>>& config_definitions) {

	// config loading
	auto config_object = load_config(config_definitions);

	return C(config_object);
}

/**
 * Load function that uses the provided config definitions, local config file and command line arguments, if provided.
 * The order will be:
 * 1. Config definitions
 * 2. Local config file
 * 3. Command line arguments
 *
 * @tparam C root config class
 * @param config_definitions list of config definitions
 * @param local_config_path path to the local configuration file
 * @param argc number of command line arguments, default 0
 * @param argv command line arguments, default nullptr
 * @return root config object filled with the configuration data
 */
template<Config_class C>
C load(
	std::vector<std::unique_ptr<Config_definition_base> >& config_definitions,
	const fs::path& local_config_path,
	int argc = 0,
	const char** argv = nullptr
) {
	try{
		// check if local config file exists
		check_path(local_config_path);
	} catch(const std::runtime_error& e) {
		throw std::runtime_error(format::format("There is a problem with the user configuration: {}", e.what()));
	}

	// add local config definition
	config_definitions.emplace_back(std::make_unique<Config_definition>(
		Config_type::LOCAL,
		local_config_path
	));

	// add command line config definition
	if(argc > 0 && argv != nullptr) {
		config_definitions.emplace_back(std::make_unique<Command_line_config_definition>(
			argc,
			argv
		));
	}

	// config loading
	auto config_object = load_config(config_definitions);

	return C(config_object);
}

/**
 * Default load function that uses the default location for the main config file. Command line arguments are also used,
 * if provided. The order will be:
 * 1. Main config file
 * 2. Local config file
 * 3. Command line arguments
 *
 * @tparam C root config class
 * @param local_config_path path to the local configuration file
 * @param argc number of command line arguments, default 0
 * @param argv command line arguments, default nullptr
 * @return root config object filled with the configuration data
 */
template<Config_class C>
C load(const fs::path& local_config_path, int argc = 0, const char** argv = nullptr) {
	std::vector<std::unique_ptr<fc::Config_definition>> config_definitions;
	config_definitions.emplace_back(std::make_unique<fc::Config_definition>());
	return load<C>(config_definitions, local_config_path, argc, argv);
}

}
    