//
// Created by Fido on 2024-04-21.
//

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "Config_object.h"
    
    
namespace fc {    

    


namespace fs = std::filesystem;


const std::string default_config_folder = "config";


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

enum class Config_type {
	MAIN,
	DEPENDENCY,
	LOCAL
};

struct Config_definition {
	const Config_type type;
	const fs::path yaml_file_path;



	Config_definition(Config_type type, fs::path yaml_file_path) : type(type), yaml_file_path(std::move(yaml_file_path)) {}

	explicit Config_definition(fs::path yaml_file_path):
		Config_definition(Config_type::MAIN, std::move(yaml_file_path))
		{}

	virtual ~Config_definition() = default;

protected:
	Config_definition(const Config_definition&) = default;
	Config_definition(Config_definition&&) = default;
};

struct Dependency_config_definition: public Config_definition {
	const std::string key_in_main_config;
	const fs::path include_path;

	Dependency_config_definition(fs::path yaml_file_path, std::string key_in_main_config, fs::path include_path):
		Config_definition(Config_type::DEPENDENCY, std::move(yaml_file_path)),
		key_in_main_config(std::move(key_in_main_config)),
		include_path(std::move(include_path)) {}
};


std::string join(const std::vector<std::string>& v, const std::string& delimiter);

Scalar_type get_scalar_type_from_string(const std::string& string);

Scalar_type get_scalar_type_from_yaml_node(const YAML::Node& node);

std::vector<std::unique_ptr<Config_definition>> parse_dependency_config_definitions(
	const std::vector<std::string>& dependency_config_strings
);

Config_object load_config(const std::vector<std::unique_ptr<Config_definition>>& config_definitions);

std::filesystem::path check_path(const std::filesystem::path& path);
        
}
    