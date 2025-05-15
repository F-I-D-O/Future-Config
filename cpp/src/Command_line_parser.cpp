//
// Created by Fido on 2025-05-11.
//

#include "Command_line_parser.h"
#include <vector>
#include <string>
#include <memory> // For std::unique_ptr and std::make_unique
#include <yaml-cpp/yaml.h> // For YAML::Node

// Anonymous namespace for helper functions
namespace {

// Recursive helper to find the path to a key
bool find_key_recursive(const fc::Config_object& current_config, const std::string& target_key, std::vector<std::string>& path) {
    for (const auto& pair : current_config) { // Uses begin()/end() iterators provided by Config_object
        const std::string& prop_key = pair.first;
        const fc::config_property_value& prop_value = pair.second;

        if (prop_key == target_key) {
            // Key found at this level.
            path.push_back(prop_key);
            return true;
        }

        if (std::holds_alternative<fc::config_object_property_value>(prop_value)) {
            const auto& nested_obj_uptr = std::get<fc::config_object_property_value>(prop_value);
            if (nested_obj_uptr) { // Check if unique_ptr is not null
                path.push_back(prop_key); // Add current segment to path before recursing
                if (find_key_recursive(*nested_obj_uptr, target_key, path)) {
                    return true; // Key found in nested object
                }
                path.pop_back(); // Backtrack: key not found in this branch
            }
        }
    }
    return false; // Key not found in current_config or its children
}

// Main helper function to initiate the search for a key's path in the master_config
std::vector<std::string> find_path_to_key(const fc::Config_object& master_config, const std::string& target_key) {
    std::vector<std::string> path;
    if (find_key_recursive(master_config, target_key, path)) {
        return path;
    }
    return {}; // Return empty path if key is not found
}

} // end anonymous namespace

fc::Config_object Command_line_parser::parse(int argc, const char* argv[], const fc::Config_object& master_config) {
	fc::Config_object result_config; // Initialize with the new default constructor

    for (int i = 1; i < argc; ++i) { // Start from 1 to skip program name
        std::string arg_key_str = argv[i];

        if (arg_key_str.rfind("--", 0) == 0 && arg_key_str.length() > 2) { // Check if it's a key like --mykey
            std::string key_name = arg_key_str.substr(2);

            if (i + 1 < argc) { // Check if there is a value for this key
                const char* value_cstr = argv[i + 1];
                std::string value_str(value_cstr);
                i++; // Consume the value argument, so the next iteration skips it

                // 1) Find the corresponding key in the master config.
                std::vector<std::string> path_in_master = find_path_to_key(master_config, key_name);

                if (!path_in_master.empty()) {
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
                // If key_name is not found in master_config (path_in_master is empty), it's ignored.
            } else {
                // Key like --option was specified without a subsequent value.
                // Currently ignored. One could add a warning here.
            }
        }
        // Else: argument is not of the form --key, so ignore it (it might be a value already consumed or an unrelated arg).
    }
    return result_config;
}
