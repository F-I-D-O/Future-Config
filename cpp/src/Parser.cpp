//
// Created by Fido on 2024-04-14.
//

#include "Parser.h"
#include "yaml-cpp/yaml.h"

Config_object Parser::parse(const std::string& yaml_content) {
	YAML::Node config = YAML::Load(yaml_content);
	return config;
}
