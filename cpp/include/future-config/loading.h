//
// Created by david on 5/21/2025.
//

#pragma once

#include "future-config/future-config_export.h"
#include "future-config/Config_object.h"
#include "future-config/common.h"

namespace fc {
template<typename I, typename E>concept Input_pointer_range =
	std::ranges::input_range<I> &&
	std::convertible_to<decltype(*std::declval<std::ranges::range_value_t<I>>()), E&>;

static_assert(Input_pointer_range<std::vector<std::unique_ptr<Config_definition_base>>, Config_definition_base>);
static_assert(Input_pointer_range<std::vector<std::unique_ptr<Config_definition>>, Config_definition>);
static_assert(Input_pointer_range<std::vector<std::unique_ptr<Config_definition>>, Config_definition_base>);


Config_object FUTURE_CONFIG_EXPORT parse_config(const Config_definition& config_definition);

Config_object FUTURE_CONFIG_EXPORT merge_configs(Config_object& result_config, Config_object& new_config);

void FUTURE_CONFIG_EXPORT process_variables(Config_object& config);


Config_object FUTURE_CONFIG_EXPORT parse_command_line_config(
	const Command_line_config_definition& command_line_config,
	const std::optional<Config_object>::value_type& result_config
);

template<Input_pointer_range<Config_definition_base> I>
Config_object FUTURE_CONFIG_EXPORT load_config(const I& config_definitions) {
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
			config = parse_command_line_config(command_line_config, result_config.value());
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
		}
		result_config = merge_configs(result_config.value(), config);
	}

	process_variables(result_config.value());

	return result_config.value();
}

template<Input_pointer_range<Config_definition> I>
Config_object FUTURE_CONFIG_EXPORT load_config_for_builder(const I& config_definitions) {
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

		result_config = merge_configs(result_config.value(), config);
	}

	return result_config.value();
}
}
