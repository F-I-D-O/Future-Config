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

#include "future-config/Config_object.h"
#include "future-config/future-config_export.h"
    
    
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

struct FUTURE_CONFIG_EXPORT Config_definition {
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

Config_object FUTURE_CONFIG_EXPORT load_config(const std::vector<std::unique_ptr<Config_definition>>& config_definitions);

std::filesystem::path FUTURE_CONFIG_EXPORT check_path(const std::filesystem::path& path);
        
}

/**
 * Indirect iterator for iterating pointers as if they were values. Boost have this functionality, but it's a bit heavy.
 * It's not reasonable to have a dependency on boost just for this.
 * @tparam Iterator
 */
template <typename Iterator>
class Indirect_iterator {
public:
	using value_type = typename std::iterator_traits<Iterator>::value_type::element_type;
	using pointer = value_type*;
	using reference = value_type&;
	using difference_type = typename std::iterator_traits<Iterator>::difference_type;
	using iterator_category = std::forward_iterator_tag;

	explicit Indirect_iterator(Iterator it) : it_(it) {}

	reference operator*() const { return **it_; }
	pointer operator->() const { return &**it_; }

	Indirect_iterator& operator++() {
		++it_;
		return *this;
	}

	Indirect_iterator operator++(int) {
		Indirect_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	bool operator==(const Indirect_iterator& other) const { return it_ == other.it_; }
	bool operator!=(const Indirect_iterator& other) const { return *this != other; }

private:
	Iterator it_;
};

template <typename MapIterator>
class Indirect_map_iterator {
public:
	using KeyType = typename MapIterator::value_type::first_type;
	using ValueType = typename MapIterator::value_type::second_type::element_type;
	using reference = std::pair<const KeyType&, ValueType&>;
	using pointer = std::pair<const KeyType*, ValueType*>;
	using difference_type = typename std::iterator_traits<MapIterator>::difference_type;
	using iterator_category = std::forward_iterator_tag;

	explicit Indirect_map_iterator(MapIterator it) : it_(it) {}

	reference operator*() const {
		return {it_->first, *(it_->second)};
	}

	pointer operator->() const {
		return {&(it_->first), it_->second.get()};
	}

	Indirect_map_iterator& operator++() {
		++it_;
		return *this;
	}

	Indirect_map_iterator operator++(int) {
		Indirect_map_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	bool operator==(const Indirect_map_iterator& other) const { return it_ == other.it_; }
	bool operator!=(const Indirect_map_iterator& other) const { return *this != other; }

private:
	MapIterator it_;
};