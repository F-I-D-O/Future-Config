//
// Created by Fido on 2025-05-11.
//

#include "Command_line_parser.h"
#include <algorithm>
#include <vector>
#include <string>
#include <memory> // For std::unique_ptr and std::make_unique
#include <stdexcept>

// Anonymous namespace for helper functions
namespace {

// Arguments that are always allowed, even if they are not present in the master config.
// The fconfig_log argument (config log level) is processed by the config loading.
const std::vector<std::string> default_allowed_arguments{"help", "version", "fconfig_log"};

// Recursive helper to find all paths to a key
void find_key_recursive(
    const fc::Config_object& current_config,
    const std::string& target_key,
    std::vector<std::string>& current_path,
    std::vector<std::vector<std::string>>& found_paths
) {
    for (const auto& pair : current_config) { // Uses begin()/end() iterators provided by Config_object
        const std::string& prop_key = pair.first;
        const fc::config_property_value& prop_value = pair.second;

        current_path.push_back(prop_key);

        if (prop_key == target_key) {
            found_paths.push_back(current_path);
        }

        if (std::holds_alternative<fc::config_object_property_value>(prop_value)) {
            const auto& nested_obj_uptr = std::get<fc::config_object_property_value>(prop_value);
            if (nested_obj_uptr) { // Check if unique_ptr is not null
                find_key_recursive(*nested_obj_uptr, target_key, current_path, found_paths);
            }
        }

        current_path.pop_back();
    }
}

// Joins the path segments with dots, e.g. {"object", "key"} -> "object.key"
std::string path_to_string(const std::vector<std::string>& path) {
    std::string result;
    for (const auto& segment : path) {
        if (!result.empty()) {
            result += ".";
        }
        result += segment;
    }
    return result;
}

// Main helper function to initiate the search for a key's path in the master_config.
// Returns an empty path if the key is not found, throws if the key is present more than once.
std::vector<std::string> find_path_to_key(const fc::Config_object& master_config, const std::string& target_key) {
    std::vector<std::string> current_path;
    std::vector<std::vector<std::string>> found_paths;
    find_key_recursive(master_config, target_key, current_path, found_paths);

    if (found_paths.empty()) {
        return {};
    }
    if (found_paths.size() > 1) {
        std::string paths_string;
        for (const auto& path : found_paths) {
            if (!paths_string.empty()) {
                paths_string += ", ";
            }
            paths_string += path_to_string(path);
        }
        throw std::runtime_error(format::format(
            "Command line argument --{} is ambiguous, the key is present multiple times in the configuration: {}. "
            "Only unique keys can be set from the command line.",
            target_key,
            paths_string
        ));
    }
    return found_paths.front();
}

// Checks if the argument is allowed even if it is not present in the master config
bool is_allowed_argument(const std::string& key_name, const std::vector<std::string>& allowed_arguments) {
    return std::ranges::find(default_allowed_arguments, key_name) != default_allowed_arguments.end()
        || std::ranges::find(allowed_arguments, key_name) != allowed_arguments.end();
}

// Checks if the argument is a key like --mykey
bool is_key_argument(const std::string& argument) {
    return argument.starts_with("--") && argument.length() > 2;
}

// Returns the property located at the given path in the config
const fc::config_property_value& get_property_at_path(
    const fc::Config_object& config,
    const std::vector<std::string>& path
) {
    const fc::Config_object* current_object = &config;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        current_object = std::get<fc::config_object_property_value>((*current_object)[path[i]]).get();
    }
    return (*current_object)[path.back()];
}

// Checks if the property is a boolean, i.e., if it can be set by a command line switch
bool is_boolean_property(const fc::config_property_value& property) {
    if (!std::holds_alternative<std::string>(property)) {
        return false;
    }
    const auto& value = std::get<std::string>(property);
    return value == "true" || value == "false";
}

} // end anonymous namespace

fc::Config_object Command_line_parser::parse(
    int argc,
    const char* argv[],
    const fc::Config_object& master_config,
    const std::vector<std::string>& allowed_arguments
) {
	fc::Config_object result_config; // Initialize with the new default constructor

    for (int i = 1; i < argc; ++i) { // Start from 1 to skip program name
        std::string arg_key_str = argv[i];

        if (is_key_argument(arg_key_str)) {
            std::string key_name = arg_key_str.substr(2);

            // 1) Find the corresponding key in the master config.
            std::vector<std::string> path_in_master = find_path_to_key(master_config, key_name);

            // A key that is not followed by a value (it is the last argument, or another key follows) is a switch.
            const bool is_switch = i + 1 >= argc || is_key_argument(argv[i + 1]);

            if (path_in_master.empty()) {
                // Key not found in master_config: this is only permitted for the allowed arguments, which are skipped.
                if (!is_allowed_argument(key_name, allowed_arguments)) {
                    throw std::runtime_error(format::format(
                        "Unknown command line argument --{}: there is no such key in the configuration.",
                        key_name
                    ));
                }
                if (!is_switch) {
                    i++; // Skip the value of the allowed argument
                }
                continue;
            }

            std::string value_str;
            if (is_switch) {
                // Switches are only supported for boolean properties: passing the switch sets the property to true.
                if (!is_boolean_property(get_property_at_path(master_config, path_in_master))) {
                    throw std::runtime_error(format::format(
                        "Command line argument --{} was passed without a value, but it is not a boolean property.",
                        key_name
                    ));
                }
                value_str = "true";
            }
            else {
                value_str = argv[i + 1];
                i++; // Consume the value argument, so the next iteration skips it
            }

            // Key found in master_config, proceed to create hierarchy and insert value.
            fc::Config_object* current_target_obj_ptr = &result_config;

            // 2) Create the necessary hierarchy in result_config to mimic master_config.
            // Loop through path segments, excluding the final key itself.
            for (size_t j = 0; j < path_in_master.size() - 1; ++j) {
                const std::string& segment_name = path_in_master[j];
                
                bool segment_navigated_or_created = false;
                if (current_target_obj_ptr->contains(segment_name)) {
                    auto& existing_prop_variant = current_target_obj_ptr->operator[](segment_name);
                    if (std::holds_alternative<fc::config_object_property_value>(existing_prop_variant)) {
                        // Segment exists and is an object, navigate into it.
                        auto& uptr_val = std::get<fc::config_object_property_value>(existing_prop_variant);
                        current_target_obj_ptr = uptr_val.get();
                        // Basic check, unique_ptr's get() should not be null if it was properly constructed.
                        if (!current_target_obj_ptr) {
                             // This indicates an issue, e.g. a null unique_ptr was stored.
                             // For robustness, one might throw or break. For now, assume valid objects.
                            break; 
                        }
                        segment_navigated_or_created = true;
                    }
                    // If segment exists but is not an object, it will be overwritten by the logic below.
                }

                if (!segment_navigated_or_created) {
                    // Segment does not exist, or exists but is not an object: create a new Config_object for it.
                    auto new_nested_obj_uptr = std::make_unique<fc::Config_object>(); // Use default constructor
                    fc::Config_object* new_nested_obj_raw_ptr = new_nested_obj_uptr.get(); // Get raw ptr before move
                    current_target_obj_ptr->emplace(segment_name, std::move(new_nested_obj_uptr));
                    current_target_obj_ptr = new_nested_obj_raw_ptr; // Navigate into the newly created object
                }
                 if (!current_target_obj_ptr) break; // Safety break if navigation/creation failed
            }
             if (!current_target_obj_ptr) continue; // Skip this cmd arg if hierarchy creation failed

            // 3) Insert the parameter value into the new config at the determined location.
            const std::string& final_key_name = path_in_master.back();
            current_target_obj_ptr->emplace(final_key_name, value_str); // Values are stored as std::string
        }
        // Else: argument is not of the form --key (e.g., a positional argument), so it is ignored.
    }
    return result_config;
}
