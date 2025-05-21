//
// Created by Gemini on 2024-05-23.
//

#include "future-config/loading.h"
#include "Command_line_parser.h" // For Command_line_parser
#include "future-config/Config_object.h" // For Config_object
#include "future-config/common.h" // For Command_line_config_definition

namespace fc {

// Definition of parse_command_line_config, matching the declaration in loading.h
Config_object parse_command_line_config(
    const Command_line_config_definition& command_line_config, // Parameter name and type match declaration
    const Config_object& result_config                        // Type matches declaration (std::optional::value_type -> const ref)
) {
    return Command_line_parser().parse(command_line_config.argc, command_line_config.argv, result_config);
}

} // namespace fc 