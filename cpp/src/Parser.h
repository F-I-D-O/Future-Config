//
// Created by Fido on 2024-04-14.
//

#pragma once


#include <string>
#include <yaml-cpp/node/node.h>

class Parser {
	YAML::Node parse(const std::string& yaml_content);
};
