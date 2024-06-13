//
// Created by Fido on 2024-04-14.
//

#pragma once


#include <string>
#include <filesystem>

#include "Config_object.h"
    
    
namespace fc {    

    

namespace fs = std::filesystem;

class Parser {
public:
	Config_object parse(const std::string& yaml_content);

	Config_object parse(const fs::path& yaml_file_path);
};

        
}
    