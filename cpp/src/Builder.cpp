//
// Created by Fido on 2024-04-21.
//

#include <ranges>
#include <tuple>

#include "Builder.h"
#include "future-config/format.h"
#include "future-config/resources.h"
    
    
namespace fc {    

    


namespace {

// Type of the member of the generated config class for the scalar value
Scalar_type get_scalar_type(const Config_scalar& scalar) {
	if(std::holds_alternative<std::string>(scalar)) {
		return Scalar_type::STRING;
	}
	if(std::holds_alternative<std::int64_t>(scalar)) {
		return Scalar_type::INT;
	}
	if(std::holds_alternative<double>(scalar)) {
		return Scalar_type::FLOAT;
	}
	if(std::holds_alternative<bool>(scalar)) {
		return Scalar_type::BOOL;
	}
	return Scalar_type::PATH;
}

} // namespace

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
		property_data["name"] = sanitize_key(child_key);

		switch(child_object.index()) {
			case scalar_index:
//			case int_index:
//			case double_index:
//			case bool_index:
			{
				property_data["mode"] = "scalar";
				const auto& scalar = std::get<Config_scalar>(child_object);
				property_data["type"] = get_scalar_type(scalar).cpp_source_string();
				property_data["value"] = to_string(scalar);
				non_array_properties_count++;
				break;
			}
			case object_index: {
				std::string child_class_name;
				bool dependency = false;

				// dependency configs are identified by the full path of the object they are bound to
				const auto child_path_string = join(child_path, ".");
				if(dependency_config_map.contains(child_path_string)) {
					std::string include_string;
					std::tie(include_string, child_class_name) = dependency_config_map.at(child_path_string);
					dependency = true;
					template_data["includes"].push_back(include_string);
				}
				else {
					child_class_name = get_template_data_for_class(
						*std::get<config_object_property_value>(child_object),
						child_key,
						child_path,
						template_data
					);
				}
				property_data["mode"] = "object";
				property_data["type"] = "fc::Config_object&";
				property_data["class_name"] = child_class_name;
				non_array_properties_count++;
				break;
			}
			// case object_array_index:
			case array_index:
//			case int_array_index:
//			case double_array_index:
//			case bool_array_index:
			{
				std::string item_type;
				bool scalar_item = true;


//				// if the array contains objects, generate a class for those objects
//				if(child_object.index() == object_array_index) {
//					item_type = "fc::Config_object";
//					auto class_name = format::format("{}_item", join(child_path, "_"));
//					class_name[0] = static_cast<char>(std::toupper(class_name.at(0)));
//					property_data["array_type_name"] = class_name;
//					scalar_item = false;
//					get_template_data_for_class(
//						std::get<std::vector<Config_object>>(child_object)[0],
//						class_name,
//						child_path,
//						template_data
//					);
//				}
//				else {
					item_type = get_scalar_type(std::get<std::vector<Config_scalar>>(child_object)[0]).cpp_source_string();
					property_data["array_type_name"] = item_type;
//				}
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

std::string Builder::sanitize_key(const std::string& root_object_name) {
	std::string sanitized_root_object_name = root_object_name;
	std::ranges::replace(sanitized_root_object_name, '-', '_');
	return sanitized_root_object_name;
}

void Builder::generate_config() {
	fs::create_directories(output_dir);
	inja::Environment env;

	// environment configuration
	env.set_trim_blocks(true); // remove newlines after jinja control statements
	env.set_lstrip_blocks(true); // remove leading whitespace before jinja control statements

	const auto template_filepath = get_resource_path("config.jinja");

	try {
		inja::Template config_template = env.parse_template(template_filepath.string());

		inja::json template_data;
		template_data["includes"] = inja::json::array();
		template_data["class_data"] = inja::json::array();
		template_data["empty_body"] = "{}";

		get_template_data_for_class(config, sanitize_key(root_object_name), {}, template_data);
		auto out_path = output_dir / format::format("{}.h", root_object_name);
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
			// The name of the dependency root config class is determined from the name of the dependency config
			// header, using the same rules as when generating the header: <name>.h contains the class <Name>.
			const auto dependency_class_name = get_class_name(
				sanitize_key(dependency_config_definition.include_path.stem().string())
			);
			dependency_config_map[dependency_config_definition.bind_path] = {
				dependency_config_definition.include_path.generic_string(),
				dependency_class_name
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
    