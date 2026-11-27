//
// Created by Fido on 2024-04-14.
//

#pragma once


#include <string>
#include <filesystem>

#include "future-config/Config_object.h"
    
    
namespace fc {    

    

namespace fs = std::filesystem;

class Parser {
public:
	Config_object parse(const std::string& yaml_content);

	Config_object parse(const fs::path& yaml_file_path);

	/**
	 * Determines the type of a scalar value given as a text (not as a part of a YAML document), using the same rules
	 * as for the values in the YAML files. Texts that are not integers, floating point numbers, or booleans are strings.
	 */
	Config_scalar parse_scalar(const std::string& text);

	/**
	 * Checks if the text has a form of a file system path. Only the unambiguous forms are recognized:
	 * - absolute paths: `/dir/file`, `C:\dir\file`, `C:/dir/file`, `\\server\share`
	 * - paths relative to the current or parent directory: `.`, `..`, `./file`, `../file`
	 * - paths relative to the home directory: `~`, `~/file`
	 */
	static bool is_path(const std::string& text);

	/**
	 * Checks if the path has an absolute form (the first group in the `is_path` function). The check is independent
	 * of the operating system.
	 */
	static bool is_absolute_path(const std::string& text);

	/**
	 * Checks if the path is relative to the home directory (the last group in the `is_path` function).
	 */
	static bool is_home_path(const std::string& text);
};

        
}
    