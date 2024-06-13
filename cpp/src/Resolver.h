//
// Created by Fido on 2024-05-05.
//

#pragma once

#include <string>
#include <queue>
#include <regex>
#include <optional>
#include "yaml-cpp/yaml.h"

#include "Config_object.h"
    
    
namespace fc {    

    


enum class Resolve_status {
	FAILED,
	PARTIAL,
	COMPLETE
};


class Resolver {
	const std::regex variable_regex = std::regex(R"regex(\$\{([^\}]+)\})regex");

	Config_object& config_object;

	std::queue<std::pair<Config_object&, std::string>> unresolved_variables;

public:
	explicit Resolver(Config_object& config):
		config_object(config)
	{}

	void resolve();

	void add_all_variables_to_queue(Config_object& config_object);

	void process_queue();

	[[nodiscard]] std::tuple<Resolve_status, std::string> resolve_value(config_property_value& config_property_val) const;

	[[nodiscard]] std::string get_value(const std::string& var_name) const;
};

        
}
    