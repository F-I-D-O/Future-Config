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
		const std::unordered_map<std::string, std::unique_ptr<Config>>& dependency_config_map,
		const fs::path& output_dir
	);


	void build_config();

private:
	const YAML::Node& config;

	const std::unordered_map<std::string,std::unique_ptr<Config>>& dependency_config_map;

	const fs::path& output_dir;

	void clean_build_dir();

	void generate_config_class(const YAML::Node& config_object, const std::string& key, bool root = false);
};
