//
// Created by david on 2024-05-08.
//

#include "future-config/Config_object.h"

#include <cmath>


namespace fc {

namespace {

// Text representation of a floating point number that is read back as a floating point number
std::string float_to_string(double value) {
	if(std::isnan(value)) {
		return ".nan";
	}
	if(std::isinf(value)) {
		return value > 0 ? ".inf" : "-.inf";
	}
	auto string = format::format("{}", value);
	if(string.find_first_of(".e") == std::string::npos) {
		string += ".0";
	}
	return string;
}

} // namespace

std::string get_type_name(const Config_scalar& scalar) {
	if(std::holds_alternative<std::string>(scalar)) {
		return "string";
	}
	if(std::holds_alternative<std::int64_t>(scalar)) {
		return "int";
	}
	if(std::holds_alternative<double>(scalar)) {
		return "float";
	}
	if(std::holds_alternative<bool>(scalar)) {
		return "bool";
	}
	return "path";
}

std::string to_string(const Config_scalar& scalar) {
	if(std::holds_alternative<std::string>(scalar)) {
		return std::get<std::string>(scalar);
	}
	if(std::holds_alternative<std::int64_t>(scalar)) {
		return std::to_string(std::get<std::int64_t>(scalar));
	}
	if(std::holds_alternative<double>(scalar)) {
		return float_to_string(std::get<double>(scalar));
	}
	if(std::holds_alternative<bool>(scalar)) {
		return std::get<bool>(scalar) ? "true" : "false";
	}
	return std::get<std::filesystem::path>(scalar).string();
}

Config_object::Config_object(const Config_object& other) {
	for(const auto&[key, value]: other.properties) {
		if(std::holds_alternative<config_object_property_value>(value)) {
			properties.emplace(key, std::make_unique<Config_object>(*std::get<config_object_property_value>(value)));
		}
		else if(std::holds_alternative<Config_scalar>(value)) {
			properties.emplace(key, std::get<Config_scalar>(value));
		}
		else {
			properties.emplace(key, std::get<std::vector<Config_scalar>>(value));
		}
	}
}

Config_object& Config_object::operator=(const Config_object& other) {
	if(this == &other) return *this;
	Config_object copy(other);
	properties = std::move(copy.properties);
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


}
