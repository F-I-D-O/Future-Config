//
// Created by Fido on 2025-05-11.
//

#pragma once

#include <string>
#include <vector>

#include "future-config/Config_object.h"


class Command_line_parser {
public:
	/**
	 * Parses the command line arguments of form `--key value` or `--key` (switch for boolean properties) into a config
	 * object.
	 * @param argc number of command line arguments
	 * @param argv command line arguments
	 * @param master_config config object in which the keys are looked up
	 * @param allowed_arguments names (without the leading `--`) of the arguments that do not have to be present in the
	 * master config. These arguments are skipped by the parser. The default allowed arguments (`help` and `version`) are
	 * always skipped.
	 * @throws std::runtime_error if some `--key` argument is neither in the master config nor allowed, if the key is
	 * present in the master config more than once, or if a switch is used for a non-boolean property.
	 */
	fc::Config_object parse(
		int argc,
		const char* argv[],
		const fc::Config_object& master_config,
		const std::vector<std::string>& allowed_arguments = {}
	);
};
