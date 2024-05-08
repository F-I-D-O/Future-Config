//
// Created by david on 2024-05-08.
//

#pragma once

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>


class Config_object;

using config_property_value = std::variant<
	// scalar types
	std::string,
	int,
	double,
	bool,

	// array types
	std::vector<std::string>,
	std::vector<int>,
	std::vector<double>,
	std::vector<bool>,
	std::vector<Config_object>,

	// object type
	Config_object
>;


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


class Config_object {
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
		return properties[key];
	}

	const config_property_value& operator[](const std::string& key) const {
		return properties.at(key);
	}

	[[nodiscard]] unsigned short size() const {
		return properties.size();
	}
};

static_assert(std::ranges::input_range<Config_object>);
