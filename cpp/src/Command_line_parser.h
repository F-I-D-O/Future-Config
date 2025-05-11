//
// Created by Fido on 2025-05-11.
//

#pragma once
#include "future-config/Config_object.h"


class Command_line_parser {
public:
	fc::Config_object parse(int argc, const char* argv[], const fc::Config_object& master_config);
};
