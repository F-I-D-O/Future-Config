//
// Created by david on 2024-05-08.
//

#include <format>
#include "Config_object.h"

Config_object::Config_object(const YAML::Node& yaml_config) {
	for(YAML::const_iterator it = yaml_config.begin(); it != yaml_config.end(); ++it) {
		auto key = it->first.as<std::string>();
		auto value = it->second;

		if(value.IsScalar()) {
//			Scalar_type scalar_type = get_scalar_type_from_yaml_node(value);
//			switch(scalar_type()) {
//				case Scalar_type::STRING:
//					properties[key] = value.as<std::string>();
//					break;
//				case Scalar_type::INT:
//					properties[key] = value.as<int>();
//					break;
//				case Scalar_type::FLOAT:
//					properties[key] = value.as<double>();
//					break;
//				case Scalar_type::BOOL:
//					properties[key] = value.as<bool>();
//					break;
//			}
			properties.emplace(key, value.as<std::string>());
		}
		else if(value.IsSequence()) {
			if(value[0].IsScalar()) {
//				Scalar_type scalar_type = get_scalar_type_from_yaml_node(value[0]);
//				switch(scalar_type()) {
//					case Scalar_type::STRING:
//						properties[key] = value.as<std::vector<std::string>>();
//						break;
//					case Scalar_type::INT:
//						properties[key] = value.as<std::vector<int>>();
//						break;
//					case Scalar_type::FLOAT:
//						properties[key] = value.as<std::vector<double>>();
//						break;
//					case Scalar_type::BOOL:
//						properties[key] = value.as<std::vector<bool>>();
//						break;
//				}
				properties.emplace(key, value.as<std::vector<std::string>>());
			}
			else if(value[0].IsMap()) {
				std::vector<Config_object> objects;
				for(const auto& yaml_object: value) {
					objects.emplace_back(yaml_object);
				}
				properties.emplace(key, objects);
			}
			else {
				throw std::runtime_error(std::format("Unsupported as array member type"));
			}
		}
		else if(value.IsMap()) {
			properties.emplace(key, Config_object(value));
		}
	}
}
