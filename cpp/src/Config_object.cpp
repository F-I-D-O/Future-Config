//
// Created by david on 2024-05-08.
//

#include "future-config/Config_object.h"

// namespace std {
//
// // custom copy constructor for config_property_value
// template<>
// std::variant<int,double>::variant(const std::variant<int,double>& other) {
// 	if(std::holds_alternative<std::string>(other)) {
// 		*this(std::get<std::string>(other));
// 	}
// 	else (std::holds_alternative<std::vector<std::string>>(other)) {
// 		*this = std::get<std::vector<std::string>>(other);
// 	}
// }
//
// }
    
    
namespace fc {




    

Config_object::Config_object(const Config_object& other) {
	for(const auto&[key, value]: other.properties) {
		if(std::holds_alternative<std::string>(value)) {
			properties.emplace(key, std::get<std::string>(value));
		}
		else if(std::holds_alternative<std::vector<std::string>>(value)) {
			properties.emplace(key, std::get<std::vector<std::string>>(value));
		}
		else if(std::holds_alternative<config_object_property_value>(value)) {
			properties.emplace(key, std::make_unique<Config_object>(*std::get<config_object_property_value>(value)));
		}
	}
}

Config_object& Config_object::operator=(const Config_object& other) {
	if(this == &other) return *this;
	for(const auto&[key, value]: other.properties) {
		if(std::holds_alternative<std::string>(value)) {
			properties.emplace(key, std::get<std::string>(value));
		}
		else if(std::holds_alternative<std::vector<std::string>>(value)) {
			properties.emplace(key, std::get<std::vector<std::string>>(value));
		}
		else if(std::holds_alternative<config_object_property_value>(value)) {
			properties.emplace(key, std::make_unique<Config_object>(*std::get<config_object_property_value>(value)));
		}
	}
	return *this;
}



/**
 * Specialization for Config_object property
 * @param key key of the property in parent object
 * @return Config_object
 */
template<>
Config_object& Config_object::get<Config_object&>(const std::string& key) const {
	if(std::holds_alternative<config_object_property_value>(properties.at(key))) {
		return *std::get<config_object_property_value>(properties.at(key));
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
//	if(std::holds_alternative<std::vector<Config_object>>(properties.at(key))) {
//		return std::get<std::vector<Config_object>>(properties.at(key));
//	}
//	else {
//		throw std::runtime_error(format::format("Property {} is not an array of objects", key));
//	}
//}

        
}
    