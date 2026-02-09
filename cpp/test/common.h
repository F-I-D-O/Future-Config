//
// Created by david on 2024-05-08.
//

#pragma once

#include <gtest/gtest.h>

#include "future-config/Config_object.h"

namespace fc {

struct CommandResult {
	int exit_code;
	std::string output;
};

void compare_config_objects(const Config_object& expected, const Config_object& actual);

CommandResult run_command(const std::string& command);

} // namespace fc


