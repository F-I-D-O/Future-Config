//
// Created by david on 5/21/2025.
//

#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include "future-config/future-config_export.h"
#include "future-config/Config_object.h"
#include "future-config/common.h"

namespace fc {
template<typename I, typename E>concept Input_pointer_range =
	std::ranges::input_range<I> &&
	std::convertible_to<decltype(*std::declval<std::ranges::range_value_t<I>>()), E&>;

Config_object FUTURE_CONFIG_EXPORT parse_config(const Config_definition& config_definition);

/**
 * Checks that a master config (main or dependency) contains at least one property.
 * @throws std::runtime_error if the config is empty
 */
void FUTURE_CONFIG_EXPORT check_master_config_not_empty(const Config_object& config);

Config_object FUTURE_CONFIG_EXPORT merge_configs(Config_object& result_config, Config_object& new_config);

void FUTURE_CONFIG_EXPORT process_variables(Config_object& config);

/**
 * Resolves the variables in the config and determines the types of the values with the resolved variables.
 * @param config config with variables
 * @param type_reference config that determines the types of the properties (see `get_type_reference`). The properties
 * that are not present in the reference, and all properties if the reference is null, get the type determined from
 * the resolved value.
 * @throws std::runtime_error if a resolved value cannot be converted to the type of the property
 */
void FUTURE_CONFIG_EXPORT process_variables(Config_object& config, const Config_object* type_reference);

/**
 * Makes all relative paths in the config absolute. The paths that are already absolute, the paths relative to the home
 * directory (`~/...`), and the paths starting with a variable are not changed.
 * @param config content of a config source
 * @param base_directory directory the relative paths are relative to
 */
void FUTURE_CONFIG_EXPORT make_paths_absolute(Config_object& config, const std::filesystem::path& base_directory);

/**
 * @return the directory the relative paths from the config source are relative to: the directory of the config file,
 * or the current working directory for the command line arguments
 */
FUTURE_CONFIG_EXPORT std::filesystem::path get_base_directory(const Config_definition_base& config_definition);

/**
 * @return description of the config source for error messages
 */
FUTURE_CONFIG_EXPORT std::string get_source_description(const Config_definition_base& config_definition);

/**
 * Checks that the master config can be used for determining the types of the properties.
 * @throws std::runtime_error if some array has items of different types
 */
void FUTURE_CONFIG_EXPORT check_master_types(const Config_object& config, const std::string& source_description);

/**
 * Creates the config that determines the types of the properties: the master config with resolved variables.
 * @param master_config master config (merged main and dependency configs)
 */
Config_object FUTURE_CONFIG_EXPORT get_type_reference(const Config_object& master_config);

/**
 * Converts the values in the config to the types of the same properties in the type reference. The properties that are
 * not present in the reference keep their types. The values with variables are skipped, they are converted when
 * the variables are resolved.
 *
 * Any scalar can be converted to a string, and an integer can be converted to a floating point number.
 * @throws std::runtime_error if a value cannot be converted to the type of the property
 */
void FUTURE_CONFIG_EXPORT convert_types(
	Config_object& config,
	const Config_object& type_reference,
	const std::string& source_description
);

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

/**
 * Name of the command line argument that sets the config log level: `--fconfig_log <none|sources|values|chain>`.
 */
inline const std::string config_log_argument_name = "fconfig_log";

/**
 * Reads the config log level from the command line arguments.
 * @param command_line_config command line config definition
 * @return the config log level, or an empty optional if the config log argument is not present
 * @throws std::runtime_error if the config log argument has no value or if the value is not a name of a log level
 */
FUTURE_CONFIG_EXPORT std::optional<Config_log_level> get_command_line_log_level(
	const Command_line_config_definition& command_line_config
);

/**
 * Collects the information about the config loading and logs it according to the config log level. The default logger
 * of the spdlog library is used for logging, with the info level.
 */
class FUTURE_CONFIG_EXPORT Config_load_log {
	struct Data;

	std::unique_ptr<Data> data;

public:
	explicit Config_load_log(Config_log_level level);

	~Config_load_log();

	Config_load_log(const Config_load_log&) = delete;

	Config_load_log& operator=(const Config_load_log&) = delete;

	/**
	 * Registers a config source. The sources have to be registered in the load order.
	 */
	void add_source(const Config_definition_base& config_definition);

	/**
	 * Logs the registered config sources.
	 */
	void log_sources() const;

	/**
	 * Registers the values provided by a config source.
	 * @param source_number number of the source in the load order, starting from 1
	 * @param config content of the source, placed in the object the source is bound to
	 */
	void add_values(unsigned source_number, const Config_object& config);

	/**
	 * Logs the configuration values together with the sources they come from.
	 * @param final_config resulting configuration with all sources merged and all variables resolved
	 */
	void log_values(const Config_object& final_config) const;
};

/**
 * Loads the configuration from the config definitions, in the order of the definitions.
 * @param config_definitions config definitions
 * @param log_level config log level. If the config log level is set in the command line arguments
 * (`--fconfig_log <level>`), the level from the command line is used instead.
 * @return the resulting configuration
 */
template<Input_pointer_range<Config_definition_base> I>
Config_object load_config(const I& config_definitions, Config_log_level log_level = Config_log_level::NONE) {
	// the log level from the command line has a priority
	for(const auto& config_definition: config_definitions) {
		if(config_definition->type == Config_type::COMMAND_LINE) {
			const auto command_line_log_level = get_command_line_log_level(
				dynamic_cast<const Command_line_config_definition&>(*config_definition)
			);
			if(command_line_log_level) {
				log_level = *command_line_log_level;
			}
		}
	}

	// the sources are logged before the loading, so that they are available even if the loading fails
	Config_load_log load_log(log_level);
	for(const auto& config_definition: config_definitions) {
		load_log.add_source(*config_definition);
	}
	load_log.log_sources();

	unsigned counter = 1;
	std::optional<Config_object> result_config;

	// The master config (main and dependency configs) determines the types of the properties. The type reference is
	// the master config with resolved variables, it is created when it is needed for the first time.
	std::optional<Config_object> master_config;
	std::optional<Config_object> type_reference;
	const auto get_current_type_reference = [&master_config, &type_reference]() -> const Config_object* {
		if(!master_config) {
			return nullptr;
		}
		if(!type_reference) {
			type_reference = get_type_reference(master_config.value());
		}
		return &type_reference.value();
	};

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
				if(file_config.type != Config_type::LOCAL) {
					check_master_config_not_empty(config);
				}
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

		// the values are converted to the types determined by the master config loaded so far
		const auto source_description = get_source_description(*config_definition);
		if(const auto* current_type_reference = get_current_type_reference()) {
			convert_types(config, *current_type_reference, source_description);
		}

		// the relative paths are relative to the location of the config source
		make_paths_absolute(config, get_base_directory(*config_definition));

		if(config_definition->type == Config_type::MAIN || config_definition->type == Config_type::DEPENDENCY) {
			check_master_types(config, source_description);
			if(master_config) {
				master_config = merge_configs(master_config.value(), config);
			}
			else {
				master_config = config;
			}
			type_reference.reset();
		}

		load_log.add_values(counter, config);

		if(result_config){
			result_config = merge_configs(result_config.value(), config);
		}
		else{
			result_config = config;
		}
		++counter;
	}

	process_variables(result_config.value(), get_current_type_reference());

	load_log.log_values(result_config.value());

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
			check_master_config_not_empty(config);
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

		// all the configs are master configs here: the types are determined by the configs loaded so far
		const auto source_description = get_source_description(*config_definition);
		check_master_types(config, source_description);
		if(result_config){
			convert_types(config, get_type_reference(result_config.value()), source_description);
		}
		make_paths_absolute(config, get_base_directory(*config_definition));

		if(result_config){
			result_config = merge_configs(result_config.value(), config);
		}
		else{
			result_config = config;
		}
	}

	// the types of the values with variables are determined from the resolved values
	process_variables(result_config.value(), nullptr);

	return result_config.value();
}
}
