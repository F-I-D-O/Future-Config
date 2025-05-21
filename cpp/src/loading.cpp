//
// Created by david on 5/21/2025.
//

#include "future-config/loading.h"

#include "Command_line_parser.h"
#include "Parser.h"
#include "Merger.h"
#include "Resolver.h"

namespace fc {
// Config_object load_config(const std::vector<std::unique_ptr<Config_definition_base>>& config_definitions) {
//
//
// }


Config_object parse_config(const Config_definition& config_definition) {
	return Parser().parse(config_definition.yaml_file_path);
}

Config_object merge_configs(Config_object& result_config, Config_object& new_config) {
	return Merger().merge(result_config, new_config);
}

// void process_variables(Config_object& config) {
// 	Resolver(config).resolve();
// }

// Config_object parse_command_line_config(
// 	const Command_line_config_definition& command_line_config,
// 	const std::optional<Config_object>::value_type& result_config
// ) {
// 	return Command_line_parser().parse(command_line_config.argc, command_line_config.argv, result_config);
// }
}
