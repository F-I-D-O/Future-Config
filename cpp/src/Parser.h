//
// Created by Fido on 2024-04-14.
//

#pragma once


#include <string>
#include <yaml-cpp/node/node.h>

#include "Config_object.h"

class Parser {
	Config_object parse(const std::string& yaml_content);
};
