//
// Created by Fido on 2024-04-14.
//

#include "Parser.h"
#include "yaml-cpp/yaml.h"

Config_object Parser::parse(const std::string& yaml_content) {
	YAML::Node config = YAML::Load(yaml_content);
	return config;
}

Config_object Parser::parse(const fs::path& yaml_file_path) {
	return YAML::LoadFile(yaml_file_path.string());
}
