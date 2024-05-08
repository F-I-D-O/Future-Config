//
// Created by david on 2024-05-08.
//

#pragma once

#include <vector>
#include <yaml-cpp/yaml.h>

#include "Config_object.h"


class Merger {
public:
	Config_object merge(const std::vector<Config_object>& configs);

private:
	void override_level(Config_object& config, const Config_object& overriding_config);
};
