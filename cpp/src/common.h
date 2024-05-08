//
// Created by Fido on 2024-04-21.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>


struct Scalar_type {
	enum Value {
		STRING, INT, FLOAT, BOOL
	};

	Scalar_type(Value value) : value(value) {}



	[[nodiscard]] Value operator()() const {
		return value;
	}

	[[nodiscard]] const std::string& cpp_source_string() const {
		return cpp_source_type_string_map.at(value);
	}

private:
	static const std::unordered_map<Value, std::string> cpp_source_type_string_map;


	Value value;
};

std::string join(const std::vector<std::string>& v, const std::string& delimiter);

Scalar_type get_scalar_type_from_yaml_node(const YAML::Node& node);