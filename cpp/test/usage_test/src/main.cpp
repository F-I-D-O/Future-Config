//
// Created by Fido on 2025-02-10.
//

#include <iostream>
#include <memory>
#include <vector>
#include <future-config/configuration.h>

#include "config/future-config-usage-test_config.h"

int main() {
	std::vector<std::unique_ptr<fc::Config_definition>> config_definitions;
	config_definitions.emplace_back(
		std::make_unique<fc::Config_definition>(fc::Config_type::MAIN, "data/config.yaml"));
	auto config = load<Future_config_usage_test_config>(config_definitions, "config.yaml");

	std::cout << config.test_par << std::endl;
}
