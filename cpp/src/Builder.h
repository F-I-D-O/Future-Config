//
// Created by Fido on 2024-04-21.
//

#pragma once

#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include "inja/inja.hpp"

#include "common.h"
#include "Config_object.h"
#include "configuration.h"


namespace fs = std::filesystem;

class Builder {

public:
	Builder(
		const Config_object& config,
		const fs::path& output_dir,
		const std::string& root_object_name,
		const std::vector<std::unique_ptr<Config_definition>>& config_definitions
	);

	Builder(
		const Config_object& config,
		const fs::path& output_dir,
		const std::string& root_object_name,
		const std::unordered_map<std::string, std::tuple<std::string, std::string>>& dependency_config_map
	);


	void build_config();

private:
	const Config_object& config;

	const std::unordered_map<std::string, std::tuple<std::string, std::string>> dependency_config_map;

	const fs::path& output_dir;

	const std::string& root_object_name;

	void clean_build_dir();

	std::string get_template_data_for_class(
		const Config_object& config_object,
		const std::string& key,
		const std::vector<std::string>& path,
		inja::json& template_data
	);

	void generate_config();

	static std::string get_class_name(const std::string& basic_string);

//	static std::string get_type(const std::string& value);
	static std::unordered_map<std::string, std::tuple<std::string, std::string>>
		generate_dependency_config_map(const std::vector<std::unique_ptr<Config_definition>>& config_definitions);
};
