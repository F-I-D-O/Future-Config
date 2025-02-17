//
// Created by Fido on 2025-02-10.
//

#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include <future-config/configuration.h>

#include "config/future-config-usage-test_config.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  	// first argument is the path to the local configuration file
    fs::path local_config_path = argv[1];

	std::vector<std::unique_ptr<fc::Config_definition>> config_definitions;
	config_definitions.emplace_back(std::make_unique<fc::Config_definition>());
	auto config = load<Future_config_usage_test_config>(config_definitions, local_config_path);

	std::cout << config.test_par << std::endl;
}
