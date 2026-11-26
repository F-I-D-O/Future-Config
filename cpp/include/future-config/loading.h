//
// Created by david on 5/21/2025.
//

#pragma once

#include <optional>

#include "future-config/future-config_export.h"
#include "future-config/Config_object.h"
#include "future-config/common.h"

namespace fc {
template<typename I, typename E>concept Input_pointer_range =
	std::ranges::input_range<I> &&
	std::convertible_to<decltype(*std::declval<std::ranges::range_value_t<I>>()), E&>;

Config_object FUTURE_CONFIG_EXPORT parse_config(const Config_definition& config_definition);

Config_object FUTURE_CONFIG_EXPORT merge_configs(Config_object& result_config, Config_object& new_config);

void FUTURE_CONFIG_EXPORT process_variables(Config_object& config);

/**
 * Finds the object a config source is bound to.
 * @param config config object in which the bound object is searched
 * @param bind_path path to the bound object, with keys separated by dots. An empty path is the root object.
 * @param source_description description of the bound config source used in the error message
 * @return the bound object
 * @throws std::runtime_error if the bind path does not exist in the config, or if it does not lead to an object
 */
FUTURE_CONFIG_EXPORT const Config_object& get_bound_object(
	const Config_object& config,
	const std::string& bind_path,
	const std::string& source_description
);

/**
 * Places the config into the object determined by the bind path, so that it can be merged from the root object.
 * @param config config object with the content of the bound config source
 * @param bind_path path to the bound object, with keys separated by dots. An empty path is the root object.
 * @return config object with the same content as the input config, wrapped in the objects from the bind path
 */
Config_object FUTURE_CONFIG_EXPORT bind_config(Config_object config, const std::string& bind_path);


Config_object FUTURE_CONFIG_EXPORT parse_command_line_config(
	const Command_line_config_definition& command_line_config,
	const std::optional<Config_object>::value_type& result_config
);

template<Input_pointer_range<Config_definition_base> I>
Config_object load_config(const I& config_definitions) {
	unsigned counter = 1;
	std::optional<Config_object> result_config;
	for(const auto& config_definition: config_definitions) {
		Config_object config;

		// command line config
		if(config_definition->type == Config_type::COMMAND_LINE) {
			// first check if master config has been loaded
			if(!result_config) {
				throw std::runtime_error(
					format::format("Trying to load command line config before master config ({}).", counter)
				);
			}

			const auto& command_line_config = dynamic_cast<const Command_line_config_definition&>(*config_definition);

			// the command line arguments are only searched in the object the command line is bound to
			const auto& bound_object = get_bound_object(
				result_config.value(),
				command_line_config.bind_path,
				"command line arguments"
			);
			config = bind_config(
				parse_command_line_config(command_line_config, bound_object),
				command_line_config.bind_path
			);
		}
		else {
			const auto& file_config = dynamic_cast<const Config_definition&>(*config_definition);
			try {
				config = parse_config(file_config);
			} catch(const std::runtime_error& e) {
				throw std::runtime_error(
					format::format(
						"Failed to parse the {}. configuration ({}): {}",
						counter,
						file_config.yaml_file_path.string(),
						e.what()
					)
				);
			}

			// local configs can only be bound to objects that already exist in the configuration
			if(file_config.type == Config_type::LOCAL && !file_config.bind_path.empty()) {
				const auto source_description = format::format("config file {}", file_config.yaml_file_path.string());
				if(!result_config) {
					throw std::runtime_error(format::format(
						"Trying to bind {} to '{}' before the master config is loaded.",
						source_description,
						file_config.bind_path
					));
				}
				get_bound_object(result_config.value(), file_config.bind_path, source_description);
			}
			config = bind_config(std::move(config), file_config.bind_path);
		}
		if(result_config){
			result_config = merge_configs(result_config.value(), config);
		}
		else{
			result_config = config;
		}
	}

	process_variables(result_config.value());

	return result_config.value();
}

template<Input_pointer_range<Config_definition> I>
Config_object load_config_for_builder(const I& config_definitions) {
	unsigned counter = 1;
	std::optional<Config_object> result_config;
	for(const auto& config_definition: config_definitions) {
		Config_object config;

		const auto& file_config = dynamic_cast<const Config_definition&>(*config_definition);
		try {
			config = parse_config(file_config);
		} catch(const std::runtime_error& e) {
			throw std::runtime_error(
				format::format(
					"Failed to parse the {}. configuration ({}): {}",
					counter,
					file_config.yaml_file_path.string(),
					e.what()
				)
			);
		}

		// dependency configs are placed into the object they are bound to
		config = bind_config(std::move(config), file_config.bind_path);

		if(result_config){
			result_config = merge_configs(result_config.value(), config);
		}
		else{
			result_config = config;
		}
	}

	return result_config.value();
}
}
