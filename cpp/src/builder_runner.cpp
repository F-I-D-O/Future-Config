//
// Created by david on 2024-06-11.
//
#include <tclap/CmdLine.h>
#include <filesystem>

#include "builder_runner.h"
#include "common.h"
#include "Builder.h"
#include "format.h"
    

    

namespace fs = std::filesystem;


void generate_config(
	std::string& main_config_name,
	fs::path& source_dir,
	std::vector<std::unique_ptr<fc::Config_definition>>& config_definitions
) {
	// config loading
	auto config_object = load_config(config_definitions);

	fs::path output_dir = source_dir / fc::default_config_folder;

	std::string root_config_object_name = format::format("{}_config", main_config_name);


	fc::Builder(config_object, output_dir, root_config_object_name, config_definitions).build_config();
}


int main(int argc, char* argv[]) {
	try {
		TCLAP::CmdLine cmd("Future Config configuration system", ' ', "0.1");

		TCLAP::ValueArg<std::string> name_arg(
			"n",
			"name",
			"Name of the main configuration file. It should represent the name of the project it configures.",
			true,
			"",
			"string"
		);
		cmd.add(name_arg);

		TCLAP::ValueArg<std::string> source_dir_path_arg(
			"s",
			"source_dir",
			"Path to main project source directory",
			true,
			"",
			"string"
		);
		cmd.add(source_dir_path_arg);

		TCLAP::ValueArg<std::string> main_config_path_arg(
			"m",
			"main",
			"Path to main configuration file",
			false,
			"../config.yaml",
			"string"
		);
		cmd.add(main_config_path_arg);

		TCLAP::UnlabeledMultiArg<std::string> dependency_config_definitions_arg(
			"dependency_config_definitions",
			"Dependency config definitions specified as tuples: <config path>,<key in main config>,<include path>",
			false,
			"string"
		);
		cmd.add(dependency_config_definitions_arg);

		cmd.parse(argc, argv);

		std::string main_config_name = name_arg.getValue();

		fs::path source_dir = source_dir_path_arg.getValue();

		std::vector<std::unique_ptr<fc::Config_definition>> config_definitions = fc::parse_dependency_config_definitions(
			dependency_config_definitions_arg.getValue()
		);

		// add main config definition
		config_definitions.emplace_back(std::make_unique<fc::Config_definition>(main_config_path_arg.getValue()));

		generate_config(main_config_name, source_dir, config_definitions);

		return 0;
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}


    