//
// Created by david on 5/21/2025.
//

#include "future-config/loading.h"

#include "Command_line_parser.h"
#include "Parser.h"
#include "Merger.h"
#include "Resolver.h"

namespace fc {
Config_object load_config(const std::vector<std::unique_ptr<Config_definition_base>>& config_definitions) {
	std::vector<Config_object> configs;
	unsigned counter = 1;
	Config_object* master_config = nullptr;
	for(const auto& config_definition: config_definitions) {

		// command line config
		if(config_definition->type == Config_type::COMMAND_LINE) {
			// first check if master config has been loaded
			if(master_config == nullptr) {
				throw std::runtime_error(format::format(
					"Trying to load command line config before master config ({}).",
					counter
				));
			}

			const auto& command_line_config
				= dynamic_cast<const Command_line_config_definition&>(*config_definition);
			configs.push_back(Command_line_parser().parse(
				command_line_config.argc,
				command_line_config.argv,
				*master_config
			));
		}
		else {
			const auto& file_config = dynamic_cast<const Config_definition&>(*config_definition);
			try{
				configs.push_back(Parser().parse(file_config.yaml_file_path));
				if(config_definition->type == Config_type::MAIN){
					master_config = &configs.back();
				}
			} catch(const std::runtime_error& e) {
				throw std::runtime_error(format::format(
					"Failed to parse the {}. configuration ({}): {}",
					counter,
					file_config.yaml_file_path.string(),
					e.what()
				));
			}
		}
	}
	auto merged_config = Merger().merge(configs);
	Resolver(merged_config).resolve();
	return merged_config;
}

}