//
// Created by david on 2024-05-08.
//

#pragma once

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <format>
#include <yaml-cpp/yaml.h>

#include "future-config_export.h"
    
    
namespace fc {    

    


class Config_object;

using config_property_value = std::variant<
	// object and object array types
	Config_object,
	std::vector<Config_object>,

	// scalar and scalar array types
	std::string,
	std::vector<std::string>
//	int,
//	std::vector<int>,
//	double,
//	std::vector<double>,
//	bool,
//	std::vector<bool>
>;

constexpr unsigned short object_index = 0;
constexpr unsigned short object_array_index = 1;
constexpr unsigned short string_index = 2;
constexpr unsigned short string_array_index = 3;
//constexpr unsigned short int_index = 4;
//constexpr unsigned short int_array_index = 5;
//constexpr unsigned short double_index = 6;
//constexpr unsigned short double_array_index = 7;
//constexpr unsigned short bool_index = 8;
//constexpr unsigned short bool_array_index = 9;

//static_assert(std::is_default_constructible_v<config_property_value>);




//class Config_object_iterator {
//private:
//	using it_value_type = std::pair<const std::string, config_property_value>;
//	using it_reference_type = it_value_type&;
//	using it_pointer_type = it_value_type*;
//
//	it_pointer_type current_value;
//public:
//	it_reference_type operator*() const {
//		return *current_value;
//	}
//
//	Config_object_iterator& operator++() {
//		current_value++;
//		return *this;
//	}
//
//}
//
//static_assert(std::input_iterator<Config_object_iterator>);


class FUTURE_CONFIG_EXPORT Config_object {
	std::unordered_map<std::string, config_property_value> properties;

public:
	using iterator = std::unordered_map<std::string, config_property_value>::iterator;
	using const_iterator = std::unordered_map<std::string, config_property_value>::const_iterator;



	Config_object(const YAML::Node& yaml_config);



	iterator begin() {
		return properties.begin();
	}

	iterator end() {
		return properties.end();
	}

	[[nodiscard]] const_iterator begin() const {
		return properties.begin();
	}

	[[nodiscard]] const_iterator end() const {
		return properties.end();
	}

	[[nodiscard]] bool contains(const std::string& key) const {
		return properties.contains(key);
	}

//	void insert(const std::string& key, const config_property_value& value) {
//		properties.insert({key, value});
//	}
//
//	void force_insert(const std::string& key, const config_property_value& value) {
//		properties[key] = value;
//	}
//
	config_property_value& operator[](const std::string& key) {
		return properties.at(key);
	}

	const config_property_value& operator[](const std::string& key) const {
		return properties.at(key);
	}

	[[nodiscard]] unsigned short size() const {
		return static_cast<unsigned short>(properties.size());
	}

	template<typename T>
	T transform_value(const std::string& string_value) const {
		if constexpr(std::is_same_v<T, std::string>) {
			return string_value;
		}
		else if constexpr(std::is_same_v<T, int>) {
			return std::stoi(string_value);
		}
		else if constexpr(std::is_same_v<T, double>) {
			return std::stod(string_value);
		}
		else if constexpr(std::is_same_v<T, bool>) {
			if(string_value == "true") {
				return true;
			}
			else if(string_value == "false") {
				return false;
			}
			else {
				throw std::runtime_error(std::format("Value is not a boolean", string_value));
			}
		}
	}

	template<typename T>
	T get(const std::string& key) const {
		if(std::holds_alternative<std::string>(properties.at(key))) {
			const auto& string_value = std::get<std::string>(properties.at(key));
			return transform_value<T>(string_value);
		}
		else {
			throw std::runtime_error(std::format("Property {} is not a scalar", key));
		}
	}

	template<typename T>
	std::vector<T> get_array(const std::string& key) const {
		if(std::holds_alternative<std::vector<std::string>>(properties.at(key))) {
			std::vector<T> result;
			std::ranges::transform(
				std::get<std::vector<std::string>>(properties.at(key)),
				std::back_inserter(result),
				[this](const std::string& string_value) {
					return transform_value<T>(string_value);
				}
			);
			return result;
		}
		else {
			throw std::runtime_error(std::format("Property {} is not an array of scalars", key));
		}
	}


};



static_assert(std::ranges::input_range<Config_object>);

        
}
    