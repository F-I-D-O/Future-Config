//
// Created by david on 2024-06-06.
//
#include <format>
#include <fstream>

#include "configuration.h"
#include "Parser.h"
#include "Merger.h"
#include "Resolver.h"
#include "Builder.h"


namespace fs = std::filesystem;


const std::string default_config_folder = "config";


Config_object load_config(const std::vector<std::unique_ptr<Config_definition>>& config_definitions) {
	std::vector<Config_object> configs;
	for(const auto& config_definition: config_definitions) {
		auto config_object = Parser().parse(config_definition->yaml_file_path);
		configs.push_back(config_object);
	}
	auto merged_config = Merger().merge(configs);
	Resolver(merged_config).resolve();
	return merged_config;
}


//Config_object parent_config: Tuple[C, str]
void generate_config(
	std::string& executable_name,
	fs::path& source_dir,
	std::vector<std::unique_ptr<Config_definition>>& dependency_config_definitions
) {
	// add main config definition
	dependency_config_definitions.emplace_back(
		std::make_unique<Config_definition>(Config_type::MAIN, source_dir / "config.yaml")
	);

	// config loading
	auto config_object = load_config(dependency_config_definitions);

	fs::path output_dir = source_dir / default_config_folder;

	std::string root_config_object_name = std::format("{}_config", executable_name);


	Builder(config_object, output_dir, root_config_object_name, dependency_config_definitions).build_config();
}
