//
// Created by david on 2024-05-08.
//

#pragma once

#include <vector>

#include "future-config/Config_object.h"
    
    
namespace fc {    

    


class Merger {
public:
	Config_object merge(std::vector<Config_object>& configs);
	Config_object merge(Config_object& base_config, Config_object& override_config);

private:
	void override_level(Config_object& config, Config_object& overriding_config);
};

        
}
    