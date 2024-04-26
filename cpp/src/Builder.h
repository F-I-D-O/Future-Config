//
// Created by Fido on 2024-04-21.
//

#pragma once

#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <unordered_map>

#include "common.h"

namespace fs = std::filesystem;

class Builder {

public:
	Builder(
		const YAML::Node& config,
		const std::unordered_map<std::string, std::tuple<std::string, std::string>>& dependency_config_map,
		const fs::path& output_dir
	);


	void build_config();

private:
	const YAML::Node& config;

	const std::unordered_map<std::string, std::tuple<std::string, std::string>>& dependency_config_map;

	const fs::path& output_dir;

	void clean_build_dir();

	void generate_config_class(
		const YAML::Node& config_object,
		const std::string& key,
		const std::vector<std::string>& path
	);

	static std::string get_class_name(const std::string& basic_string);
};
