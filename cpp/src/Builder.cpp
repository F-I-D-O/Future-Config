//
// Created by Fido on 2024-04-21.
//

#include <any>
#include <ranges>
#include <tuple>

#include "Builder.h"
    
    
namespace fc {    

    


Builder::Builder(
	const Config_object& config,
	const fs::path& output_dir,
	const std::string& root_object_name,
	const std::vector<std::unique_ptr<Config_definition>>& config_definitions
)
	: config(config), output_dir(output_dir), root_object_name(root_object_name),
	  dependency_config_map(generate_dependency_config_map(config_definitions)) {}

void Builder::clean_build_dir() {
	if(exists(output_dir) && !output_dir.empty()) {
		fs::remove_all(output_dir);
	}
}

void Builder::build_config() {
	clean_build_dir();
	generate_config();
}

std::string Builder::get_template_data_for_class(
	const Config_object& config_object,
	const std::string& key,
	const std::vector<std::string>& path,
	inja::json& template_data
) {
	inja::json class_data;
	class_data["properties"] = inja::json::array();

	unsigned short non_array_properties_count = 0;
	unsigned short array_properties_count = 0;
	for(const auto& [child_key, child_object]: config_object) {
		auto child_path = path;
		child_path.push_back(child_key);

		inja::json property_data{{"key", child_key},};

		switch(child_object.index()) {
			case string_index:
//			case int_index:
//			case double_index:
//			case bool_index:
			{
				property_data["mode"] = "scalar";
				auto string_value = std::get<std::string>(child_object);
				property_data["type"] = get_scalar_type_from_string(string_value).cpp_source_string();
				property_data["value"] = string_value;
				non_array_properties_count++;
				break;
			}
			case object_index: {
				std::string child_class_name;
				bool dependency = false;

				if(dependency_config_map.contains(child_key)) {
					std::string include_string;
					std::tie(include_string, child_class_name) = dependency_config_map.at(child_key);
					dependency = true;
					template_data["includes"].push_back(include_string);
				}
				else {
					child_class_name = get_template_data_for_class(
						std::get<Config_object>(child_object),
						child_key,
						child_path,
						template_data
					);
				}
				property_data["mode"] = "object";
				property_data["type"] = "fc::Config_object";
				property_data["class_name"] = child_class_name;
				non_array_properties_count++;
				break;
			}
			case object_array_index:
			case string_array_index:
//			case int_array_index:
//			case double_array_index:
//			case bool_array_index:
			{
				std::string item_type;
				bool scalar_item = true;


				// if the array contains objects, generate a class for those objects
				if(child_object.index() == object_array_index) {
					item_type = std::format("{}_item", join(child_path, "_"));
					item_type[0] = static_cast<char>(std::toupper(item_type.at(0)));
					scalar_item = false;
					get_template_data_for_class(
						std::get<std::vector<Config_object>>(child_object)[0],
						item_type,
						child_path,
						template_data
					);
				}
				else {
					item_type = get_scalar_type_from_string(std::get<std::vector<std::string>>(child_object)[0]).cpp_source_string();
				}
				property_data["mode"] = "array";
				property_data["scalar_item"] = scalar_item;
				property_data["item_type"] = item_type;
				array_properties_count++;
				break;
			}
			default: {
				throw std::runtime_error("Unsupported property type");
			}
		}
		class_data["properties"].push_back(property_data);
		class_data["non_array_properties_count"] = non_array_properties_count;
		class_data["array_properties_count"] = array_properties_count;
	}

	std::string class_name = get_class_name(key);
	class_data["class_name"] = class_name;

	template_data["class_data"].push_back(class_data);

	return class_name;
}

std::string Builder::get_class_name(const std::string& snake_case_property_name) {
	auto class_name = snake_case_property_name;
	class_name[0] = static_cast<char>(std::toupper(class_name.at(0)));
	return class_name;
}

void Builder::generate_config() {
	fs::create_directories(output_dir);
	inja::Environment env;

	// environment configuration
	env.set_trim_blocks(true); // remove newlines after jinja control statements
	env.set_lstrip_blocks(true); // remove leading whitespace before jinja control statements

	const auto template_filepath = "data/config.jinja";
	try {
		inja::Template config_template = env.parse_template(template_filepath);

		inja::json template_data;
		template_data["includes"] = inja::json::array();
		template_data["class_data"] = inja::json::array();
		template_data["empty_body"] = "{}";

		get_template_data_for_class(config, root_object_name, {}, template_data);
		auto out_path = output_dir / std::format("{}.h", root_object_name);
		try {
			env.write(config_template, template_data, out_path.string());
		} catch(const inja::RenderError& e) {
			throw std::runtime_error(e.what());
		}
	} catch(const inja::ParserError& e) {
		throw std::runtime_error(e.what());
	}
}

std::unordered_map<std::string, std::tuple<std::string, std::string>> Builder::generate_dependency_config_map(
	const std::vector<std::unique_ptr<Config_definition>>& config_definitions
) {
	std::unordered_map<std::string, std::tuple<std::string, std::string>> dependency_config_map;
	for(const auto& config_definition: config_definitions) {
		if(config_definition->type == Config_type::DEPENDENCY) {
			const auto& dependency_config_definition = dynamic_cast<const Dependency_config_definition&>(*config_definition);
			dependency_config_map[dependency_config_definition.key_in_main_config] = {
				dependency_config_definition.include_path.string(),
				dependency_config_definition.key_in_main_config
			};
		}
	}
	return dependency_config_map;
}

Builder::Builder(
	const Config_object& config,
	const fs::path& output_dir,
	const std::string& root_object_name,
	const std::unordered_map<std::string, std::tuple<std::string, std::string>>& dependency_config_map
):
	config(config),
	output_dir(output_dir),
	root_object_name(root_object_name),
   	dependency_config_map(dependency_config_map)
{}

        
}
    