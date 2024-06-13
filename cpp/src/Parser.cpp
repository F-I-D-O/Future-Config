//
// Created by Fido on 2024-04-14.
//

#include "yaml-cpp/yaml.h"

#include "Parser.h"
#include "common.h"
    
    
namespace fc {    

    


Config_object Parser::parse(const std::string& yaml_content) {
	YAML::Node config = YAML::Load(yaml_content);
	return config;
}

Config_object Parser::parse(const fs::path& yaml_file_path) {
	auto canonical_path = check_path(yaml_file_path);
	return YAML::LoadFile(canonical_path.string());
}

        
}
    