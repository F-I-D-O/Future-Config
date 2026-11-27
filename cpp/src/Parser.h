//
// Created by Fido on 2024-04-14.
//

#pragma once


#include <string>
#include <filesystem>

#include "future-config/Config_object.h"
    
    
namespace fc {    

    

namespace fs = std::filesystem;

class Parser {
public:
	Config_object parse(const std::string& yaml_content);

	Config_object parse(const fs::path& yaml_file_path);

	/**
	 * Determines the type of a scalar value given as a text (not as a part of a YAML document), using the same rules
	 * as for the values in the YAML files. Texts that are not integers, floating point numbers, or booleans are strings.
	 */
	Config_scalar parse_scalar(const std::string& text);
};

        
}
    