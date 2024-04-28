//
// Created by Fido on 2024-04-21.
//

#include <any>
#include <ranges>
#include <tuple>

#include "Builder.h"


Builder::Builder(
	const YAML::Node& config,
	const fs::path& output_dir,
	const std::string& root_object_name,
	const std::unordered_map<std::string, std::tuple<std::string, std::string>>& dependency_config_map
) :
config(config),
output_dir(output_dir),
root_object_name(root_object_name),
dependency_config_map(dependency_config_map){}

void Builder::clean_build_dir() {
	if (exists(output_dir) && !output_dir.empty()) {
		fs::remove_all(output_dir);
	}
}

void Builder::build_config() {
	clean_build_dir();
	generate_config();
}

std::string Builder::get_template_data_for_class(
	const YAML::Node& config_object,
	const std::string& key,
	const std::vector<std::string>& path,
	inja::json& template_data
) {
	inja::json class_data;
	class_data["properties"] = inja::json::array();

	for (YAML::const_iterator it = config_object.begin(); it != config_object.end(); ++it) {
		const auto& child_key = it->first.as<std::string>();
		const auto& child_object = it->second;
		auto child_path = path;
		child_path.push_back(child_key);

		inja::json property_data{
			{"key", child_key},
		};

		switch (child_object.Type()) {
			case YAML::NodeType::Scalar: {
				property_data["mode"] = "scalar";
				auto value = child_object.as<std::string>();
				property_data["type"] = get_type(value);
				property_data["value"] = value;
				break;
			}
			case YAML::NodeType::Map: {
				std::string child_class_name;
				bool dependency = false;

				if (dependency_config_map.contains(child_key)) {
					std::string include_string;
					std::tie(include_string, child_class_name) = dependency_config_map.at(child_key);
					dependency = true;
					template_data["includes"].push_back(include_string);
				} else {
					child_class_name = get_template_data_for_class(child_object, child_key, child_path, template_data);
				}
				property_data["mode"] = "object";
				property_data["class_name"] = child_class_name;
				break;
			}
			case YAML::NodeType::Sequence: {
				std::string item_name = "plain";

				// if the array contains objects, generate a class for those objects
				if (child_object[0].Type() == YAML::NodeType::Map) {
					item_name = std::format("{}_item", join(child_path, "_"));
					get_template_data_for_class(child_object[0], item_name, child_path, template_data);
				}
				property_data["mode"] = "array";
				property_data["item_name"] = item_name;
				break;
			}
			default: {
				throw std::runtime_error("Unsupported YAML node type");
			}
		}
		class_data["properties"].push_back(property_data);
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
	const auto template_filename = "config.jinja";
	try{
		inja::Template config_template = env.parse_template(template_filename);

		inja::json template_data;
		template_data["includes"] = inja::json::array();
		template_data["class_data"] = inja::json::array();

	//	std::string root_class_name = root_object_name;
	//	root_class_name[0] = static_cast<char>(std::toupper(root_class_name.at(0)));

		get_template_data_for_class(config, root_object_name, {}, template_data);
		auto out_path= output_dir / std::format("{}_config.h", root_object_name);
		env.write(config_template, template_data, out_path.string());
	}
	catch(const inja::ParserError& e){
		throw std::runtime_error(e.what());
	}
}

std::string Builder::get_type(const std::string& value) {
	if(value == "true" || value == "false") {
		return "bool";
	}
	if(value.find('.') != std::string::npos){
		return "double";
	}
	if(value.find_first_not_of("0123456789") == std::string::npos) {
		return "int";
	}
	return "std::string";
}
