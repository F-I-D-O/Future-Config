//
// Created by Fido on 2024-04-14.
//

#include "yaml-cpp/yaml.h"

#include "format.h"
#include "Parser.h"
#include "common.h"
    
    
namespace fc {    

    


Config_object Parser::parse(const std::string& yaml_content) {
	YAML::Node config = YAML::Load(yaml_content);
	return config;
}

Config_object Parser::parse(const fs::path& yaml_file_path) {
	auto canonical_path = check_path(yaml_file_path);
	auto yaml_content = YAML::LoadFile(canonical_path.string());

	// check that the root object is of the correct type
	auto type = yaml_content.Type();
	if (type != YAML::NodeType::Map) {
		std::string type_str = "";
		switch(type){
			case YAML::NodeType::Undefined:
				type_str = "Undefined";
				break;
			case YAML::NodeType::Null:
				type_str = "Null";
				break;
			case YAML::NodeType::Scalar:
				type_str = "Scalar";
				break;
			case YAML::NodeType::Sequence:
				type_str = "Sequence";
				break;
		}

		throw std::runtime_error(format::format(
			"The root node of configuration file must be a map. Root node: {}", type_str));
	}

	return yaml_content;
}

        
}
    