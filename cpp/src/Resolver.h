//
// Created by Fido on 2024-05-05.
//

#pragma once

#include <string>
#include <queue>
#include <regex>
#include <optional>
#include "yaml-cpp/yaml.h"

enum class Resolve_status {
	FAILED,
	PARTIAL,
	COMPLETE
};


class Resolver {
	const std::regex variable_regex = std::regex(R"regex(\$\{([^\}]+)\})regex");

	const YAML::Node& yaml_config;

	std::queue<std::pair<YAML::Node, std::string>> unresolved_variables;

public:
	explicit Resolver(const YAML::Node& yaml_config):
		yaml_config(yaml_config)
	{}

	YAML::Node resolve();

	void add_all_variables_to_queue(const YAML::Node& yaml_config_object);

	void process_queue();

	[[nodiscard]] std::tuple<Resolve_status, std::string> resolve_value(YAML::Node & node) const;

	[[nodiscard]] std::string get_value(const std::string& var_name) const;
};
