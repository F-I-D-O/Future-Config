//
// Created by david on 2024-06-07.
//

#pragma once

#include <string>
#include <filesystem>
#include "yaml-cpp/yaml.h"

#include "Config_object.h"
#include "common.h"






Config_object load_config(const std::vector<std::unique_ptr<Config_definition>>& config_definitions);