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
		std::make_unique<Config_definition>(source_dir / "config.yaml")
	);

	// config loading
	auto config_object = load_config(dependency_config_definitions);

	fs::path output_dir = source_dir / default_config_folder;

	std::string root_config_object_name = std::format("{}_config", executable_name);


	Builder(config_object, output_dir, root_config_object_name, dependency_config_definitions).build_config();
}

std::vector<std::unique_ptr<Config_definition>> parse_dependency_config_definitions(
	int argc, char* argv[], const fs::path& source_dir
) {
	std::vector<std::unique_ptr<Config_definition>> dependency_config_definitions;
	for(int i = 1; i < argc; i++) {
		// parse the dependency config definition tuple argument
		auto parts_view = std::ranges::views::split(std::string(argv[i]), ',');
		std::vector<std::string> parts;
		for(const auto& part: parts_view) {
			parts.emplace_back(part.begin(), part.end());
		}

		dependency_config_definitions.emplace_back(std::make_unique<Dependency_config_definition>(
			fs::path(parts.at(0)),
			parts.at(1),
			fs::path(parts.at(2))
		));
	}
	return dependency_config_definitions;
}

int main(int argc, char* argv[]) {
	std::string executable_name = argv[0];
	fs::path source_dir = fs::current_path();

	std::vector<std::unique_ptr<Config_definition>> dependency_config_definitions = parse_dependency_config_definitions(
		argc, argv, source_dir
	);

	generate_config(executable_name, source_dir, dependency_config_definitions);

	return 0;
}