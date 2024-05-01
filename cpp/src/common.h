//
// Created by Fido on 2024-04-21.
//

#pragma once

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

class Config;

std::string join(const std::vector<std::string>& v, const std::string& delimiter);

std::string get_scalar_type_from_yaml_node(const YAML::Node& node);