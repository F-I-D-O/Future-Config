//
// Created by david on 2024-06-06.
//
#include <format>
#include <filesystem>

#include "Config_object.h"

namespace fs = std::filesystem;

const std::string default_config_folder = "config";


//Config_object parent_config: Tuple[C, str]
void generate_config(std::string& executable_name, fs::path& source_dir){
	std::string root_config_object_name = std::format("{}_config", executable_name);

	fs::path output_dir = source_dir / default_config_folder;

	Builder(
		config_package,
		executable_name,
		root_config_object_name,
		loader.DEFAULT_GENERATED_CONFIG_PACKAGE,
		parent_config
	).build_config()
}