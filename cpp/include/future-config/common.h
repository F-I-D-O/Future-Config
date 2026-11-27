//
// Created by Fido on 2024-04-21.
//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include "future-config/future-config_export.h"
#include "future-config/resources.h"
    
    
namespace fc {    

    


namespace fs = std::filesystem;


const std::string default_config_folder = "config";


struct Scalar_type {
	enum Value {
		STRING, INT, FLOAT, BOOL, PATH
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
	LOCAL,
	COMMAND_LINE
};

/**
 * Level of the config logging:
 * - NONE: nothing is logged
 * - SOURCES: the config sources are logged
 * - VALUES: the config sources and the config values with their origin (source) are logged
 * - CHAIN: the config sources and the config values are logged. For each value that was set by multiple sources, all
 *		these sources are listed (overwrite chain).
 */
enum class Config_log_level {
	NONE,
	SOURCES,
	VALUES,
	CHAIN
};

struct FUTURE_CONFIG_EXPORT Config_definition_base {
	const Config_type type;

	/**
	 * Path to the object in the configuration hierarchy this config source is bound to, with keys separated by dots,
	 * e.g., `server.network`. The content of the source is placed into that object. An empty path (default) binds the
	 * source to the root object.
	 */
	const std::string bind_path;

	explicit Config_definition_base(Config_type type, std::string bind_path = ""):
		type(type), bind_path(std::move(bind_path)) {
	}

	virtual ~Config_definition_base() = default;

	virtual std::unique_ptr<Config_definition_base> clone() const = 0 ;
};


struct FUTURE_CONFIG_EXPORT Config_definition: public Config_definition_base {

	const fs::path yaml_file_path;



	/**
	 * @param type type of the config
	 * @param yaml_file_path_par path to the config file
	 * @param bind_path path to the object in the configuration hierarchy the config file is bound to (root by default).
	 * Main config files can only be bound to the root.
	 */
	Config_definition(Config_type type, fs::path yaml_file_path_par, std::string bind_path = "");

	Config_definition(fs::path yaml_file_path):
		Config_definition(Config_type::MAIN, std::move(yaml_file_path))
		{}

	~Config_definition() override = default;

	virtual std::unique_ptr<Config_definition_base> clone() const override {
		return std::make_unique<Config_definition>(*this);
	}
};

/**
 * Definition of a config of a dependency project. The dependency config is bound to the object in the main config
 * determined by the `key_in_main_config` (available as `bind_path`).
 *
 * The `include_path` is the path to the generated config header of the dependency, as used in the include directive.
 * The name of the dependency root config class is determined from the name of this header.
 */
struct Dependency_config_definition: public Config_definition {
	const fs::path include_path;

	Dependency_config_definition(fs::path yaml_file_path, std::string key_in_main_config, fs::path include_path):
		Config_definition(Config_type::DEPENDENCY, std::move(yaml_file_path), std::move(key_in_main_config)),
		include_path(std::move(include_path)) {}

	std::unique_ptr<Config_definition_base> clone() const override {
		return std::make_unique<Dependency_config_definition>(*this);
	}
};

struct FUTURE_CONFIG_EXPORT Command_line_config_definition: public Config_definition_base {

	const int argc;
	const char** argv;

	/**
	 * Names (without the leading `--`) of the command line arguments that are not configuration keys, typically the
	 * arguments processed by the application itself. Unlike other unknown arguments, these do not raise an exception.
	 * The `help` and `version` arguments are always allowed.
	 */
	const std::vector<std::string> allowed_arguments;


	/**
	 * @param argc number of command line arguments
	 * @param argv command line arguments
	 * @param allowed_arguments see the `allowed_arguments` member
	 * @param bind_path path to the object in the configuration hierarchy the command line arguments are bound to (root
	 * by default). The keys passed as command line arguments are only searched in that object.
	 */
	Command_line_config_definition(
		int argc,
		const char** argv,
		std::vector<std::string> allowed_arguments = {},
		std::string bind_path = ""
	):
		Config_definition_base(Config_type::COMMAND_LINE, std::move(bind_path)),
		argc(argc),
		argv(argv),
		allowed_arguments(std::move(allowed_arguments)) {}


	~Command_line_config_definition() override = default;

	std::unique_ptr<Config_definition_base> clone() const override {
		return std::make_unique<Command_line_config_definition>(*this);
	}
};


struct Config_definitions {
	std::vector<std::unique_ptr<Config_definition_base>> config_definitions;

	Config_definitions() = default;

	Config_definitions(const Config_definitions& other) {
		config_definitions.reserve(other.config_definitions.size());
		for (const auto& config_definition : other.config_definitions) {
			config_definitions.emplace_back(config_definition->clone());
		}
	}

	Config_definitions& operator=(const Config_definitions& other) {
		if (this == &other) return *this;
		Config_definitions tmp(other);      // copy-then-swap for strong safety
		config_definitions.swap(tmp.config_definitions);
		return *this;
	}


	void add(std::unique_ptr<Config_definition_base>&& config_definition) {
		config_definitions.emplace_back(std::move(config_definition));
	};

	Config_definitions(Config_definitions&&) noexcept = default;
	Config_definitions& operator=(Config_definitions&&) noexcept = default;
};


std::string join(const std::vector<std::string>& v, const std::string& delimiter);

std::vector<std::unique_ptr<Config_definition>> parse_dependency_config_definitions(
	const std::vector<std::string>& dependency_config_strings
);


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