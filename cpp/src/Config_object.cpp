//
// Created by david on 2024-05-08.
//

#include "format.h"
#include "Config_object.h"
    
    
namespace fc {    

    

Config_object::Config_object(const YAML::Node& yaml_config) {
	for(YAML::const_iterator it = yaml_config.begin(); it != yaml_config.end(); ++it) {
		auto key = it->first.as<std::string>();
		auto value = it->second;

		if(value.IsScalar()) {
			properties.emplace(key, new config_property_value (value.as<std::string>()));
		}
		else if(value.IsSequence()) {
			if(value[0].IsScalar()) {
				properties.emplace(key, new config_property_value (value.as<std::vector<std::string>>()));
			}
//			else if(value[0].IsMap()) {
//				std::vector<Config_object> objects;
//				for(const auto& yaml_object: value) {
//					objects.emplace_back(yaml_object);
//				}
//				properties.emplace(key, new config_property_value(objects));
//			}
			else {
				throw std::runtime_error(format::format("Unsupported as array member type"));
			}
		}
		else if(value.IsMap()) {
			properties.emplace(key, new config_property_value(Config_object(value)));
		}
	}
}

/**
 * Specialization for Config_object property
 * @param key key of the property in parent object
 * @return Config_object
 */
template<>
Config_object Config_object::get<Config_object>(const std::string& key) const {
	if(std::holds_alternative<Config_object>(*properties.at(key))) {
		return std::get<Config_object>(*properties.at(key));
	}
	else {
		throw std::runtime_error(format::format("Property {} is not an object", key));
	}
}

///**
// * Specialization for array of objects
// * @param key key of the property in parent object
// * @return vector of Config_object
// */
//template<>
//std::vector<Config_object> Config_object::get_array(const std::string& key) const {
//	if(std::holds_alternative<std::vector<Config_object>>(*properties.at(key))) {
//		return std::get<std::vector<Config_object>>(*properties.at(key));
//	}
//	else {
//		throw std::runtime_error(format::format("Property {} is not an array of objects", key));
//	}
//}

        
}
    