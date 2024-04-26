//
// Created by Fido on 2024-04-21.
//

#include <any>
#include <ranges>

#include "Builder.h"


Builder::Builder(
	const YAML::Node& config,
	const std::unordered_map<std::string, std::tuple<std::string,std::string>>& dependency_config_map,
	const fs::path& output_dir
) : config(config), dependency_config_map(dependency_config_map), output_dir(output_dir) {}

void Builder::clean_build_dir() {
	if (exists(output_dir) && !output_dir.empty()) {
		fs::remove_all(output_dir);
	}
}

void Builder::build_config() {
	clean_build_dir();
}

void Builder::generate_config_class(
	const YAML::Node& config_object,
	const std::string& key,
	const std::vector<std::string>& path
) {
	std::vector<std::string> includes;
	std::unordered_map<std::string, std::any> properties; // the value is one of the supported scalar types
	std::unordered_map<std::string, std::string> object_properties; // the value is the class name
	std::unordered_map<std::string, YAML::Node> array_properties;

	for (YAML::const_iterator it = config_object.begin(); it != config_object.end(); ++it) {
		const auto& child_key = it->first.as<std::string>();
		const auto& child_object = it->second;
		switch (child_object.Type()) {
			case YAML::NodeType::Scalar:
				properties[child_key] = child_object;
				break;
			case YAML::NodeType::Map:
				auto child_path= path;
				child_path.push_back(child_key);
				std::string class_name;
				bool dependency = false;

				if(dependency_config_map.contains(child_key)) {
					std::string include_string;
					std::tie(include_string, class_name) = dependency_config_map.at(child_key);
					dependency = true;
					includes.push_back(include_string);
				}
				else {
					generate_config_class(child_object, child_key, child_path);
					class_name = get_class_name(child_key);
				}
				object_properties[child_key] = class_name;
				break;
			case YAML::NodeType::Sequence:
				std::string item_name = "plain";

				// if the array contains objects, generate a class for those objects
				if(child_object[0].Type() == YAML::NodeType::Map) {
					item_name = std::format("{}_item", join(child_path, "_"));
					generate_config_class(child_object[0], item_name, child_path);
				}
				array_properties[child_key] = child_object;
				break;
			default:
				throw std::runtime_error("Unsupported YAML node type");
		}
	}

	template_filename = 'config_template.txt'
	template_data = pkgutil.get_data("fconfig.templates", template_filename)
	lookup = TemplateLookup(module_directory = "/tmp")
	class_template = Template(template_data, lookup = lookup)

	if not os.path.exists(self.output_dir):
	os.makedirs(self.output_dir)

	class_name = get_class_name(map_name)
	if is_root:
		parent_parameter_strings = []
	for
	pc
	in
	self.parent_config:
	parent_parameter_strings.append("({}, '{}')".format(pc[0].__name__, pc[1]))
	parent_parameter_strings.append("({}, None)".format(class_name))
	self.rendered_config_objects[map_name] = class_template.render(
		is_root = True,
		properties = properties,
		object_properties = object_properties,
		array_properties = array_properties,
		class_name = class_name,
		map_name = map_name,
		parent_parameter_string = ", ".join(parent_parameter_strings))
	else:
	self.rendered_config_objects[map_name] = class_template.render(
		is_root = False,
		properties = properties,
		object_properties = object_properties,
		array_properties = array_properties,
		class_name = class_name
	)
}

std::string Builder::get_class_name(const std::string& snake_case_property_name) {
	auto class_name = snake_case_property_name;
	class_name[0] = static_cast<char>(std::toupper(class_name.at(0)));
	return class_name;
}
