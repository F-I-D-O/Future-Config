//
// Created by david on 2024-06-07.
//

#pragma once

#include <filesystem>

#include "future-config/format.h"
#include "future-config/Config_object.h"
#include "future-config/common.h"
#include "future-config/loading.h"
#include "future-config/resources.h"

// generated config paths header
#include "config/config_paths.h"
    
    
namespace fc {    

template<class C>
concept Config_class = requires(const Config_object& config) {
	// requires a constructor that takes a Config_object
	C(config);
};

template <std::size_t N>
struct string_literal {
	char value[N];

	constexpr string_literal(const char (&str)[N]) {
		for (std::size_t i = 0; i < N; ++i)
			value[i] = str[i];
	}

	constexpr operator const char*() const { return value; }
};

template <class T>
concept Config_mapping_contract =
	// requires
	// {
		// { T::key() } -> std::is_same_as<const char*>; // adjust if your `key` type differs
		std::same_as<decltype(T::key()), const char*>
	// }
	&& Config_class<typename T::config_class>;

template<string_literal K, Config_class C>
struct Config_mapping {
	static constexpr string_literal key = K;

	using config_class = C;
};

inline auto get_default_config_definitions() {
	Config_definitions config_definitions;
	for(const auto& config_path: config_paths) {
		config_definitions.add(std::make_unique<Config_definition>(get_resource_path(config_path)));
	}
	return config_definitions;
};

/**
 * Options for the load function. Properties
 * - config_definitions: vector of config definitions to be used for loading the configuration. If not provided, the default
 *		config definitions will be used.
 * - local_config_path: path to the local configuration file. If provided, a local config definition will be added to the
 *		config definitions vector right after the provided config definitions.
 * - command_line_arguments: number of command line arguments and their values. If provided, a command line config
 *		definition will be added to the config definitions vector right after the local config definition
 **/
struct Load_options {
	std::optional<Config_definitions> config_definitions{get_default_config_definitions()};
	std::optional<fs::path> local_config_path;
	std::optional<std::pair<int, const char**>> command_line_arguments;
};


/**
* Main load function. If all load options are provided, the order will be:
 * 1. Config definitions
 * 2. Local config file
 * 3. Command line arguments
 *
 * For a custom order, skip the respective load options and provide your own config definitions created manually.
 *
 * @tparam C root config class
 * @param options struct with the loading options
 * @return root config object filled with the configuration data
 */
template<Config_class C>
C load(Load_options options = {}) {
	auto& config_definitions = *options.config_definitions;

	if(options.local_config_path) {
		const auto& local_config_path = *options.local_config_path;
		try{
			// check if local config file exists
			check_path(local_config_path);
		} catch(const std::runtime_error& e) {
			throw std::runtime_error(format::format("There is a problem with the user configuration: {}", e.what()));
		}

		// add local config definition
		config_definitions.add(std::make_unique<Config_definition>(Config_type::LOCAL, local_config_path));
	}

	if(options.command_line_arguments) {
		const auto& [argc, argv] = *options.command_line_arguments;
		// add command line config definition
		if(argc > 0 && argv != nullptr) {
			config_definitions.add(std::make_unique<Command_line_config_definition>(argc, argv));
		}
	}

	// config loading
	auto config_object = load_config(config_definitions.config_definitions);

	return C(config_object);
}

template<class... M>
requires(Config_mapping_contract<M> && ...)
auto load(std::vector<std::unique_ptr<Config_definition_base>>& config_definitions) {

	// config loading
	auto config_object = load_config(config_definitions);

	return std::tuple<typename M::config_class...>{
		(M::config_class(config_object[M::key]),...)
	};
}

}
    
