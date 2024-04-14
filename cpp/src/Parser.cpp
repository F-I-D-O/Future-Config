//
// Created by Fido on 2024-04-14.
//

#include "Parser.h"
#include "yaml-cpp/yaml.h"

YAML::Node Parser::parse(const std::string& yaml_content) {
	YAML::Node config = YAML::Load(yaml_content);
	return config;
}
