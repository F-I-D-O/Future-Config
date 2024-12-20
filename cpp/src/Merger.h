//
// Created by david on 2024-05-08.
//

#pragma once

#include <vector>
#include <yaml-cpp/yaml.h>

#include "Config_object.h"
    
    
namespace fc {    

    


class Merger {
public:
	Config_object merge(std::vector<Config_object>& configs);

private:
	void override_level(Config_object& config, Config_object& overriding_config);
};

        
}
    