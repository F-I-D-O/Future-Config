//
// Created by david on 5/21/2025.
//

#pragma once

#include "future-config/future-config_export.h"
#include "future-config/Config_object.h"
#include "future-config/common.h"

namespace fc {

Config_object FUTURE_CONFIG_EXPORT load_config(const std::vector<std::unique_ptr<Config_definition_base>>& config_definitions);

}