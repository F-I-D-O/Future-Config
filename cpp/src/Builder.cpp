//
// Created by Fido on 2024-04-21.
//

#include <any>

#include "Builder.h"


Builder::Builder(
	const YAML::Node& config,
	const std::unordered_map<std::string, std::unique_ptr<Config>>& dependency_config_map,
	const fs::path& output_dir
) : config(config), dependency_config_map(dependency_config_map), output_dir(output_dir) {}

void Builder::clean_build_dir() {
	if(exists(output_dir) && !output_dir.empty()){
		fs::remove_all(output_dir);
	}
}

void Builder::build_config() {
	clean_build_dir();
}

void Builder::generate_config_class(const YAML::Node& config_object, const std::string& key, bool root) {
	std::unordered_map<std::string,std::any> properties;
	std::unordered_map<std::string,std::tuple<std::string,std::any>> object_properties;
	std::unordered_map<std::string,std::any> array_properties;

	for(YAML::const_iterator it=config_object.begin(); it!=config_object.end(); ++it) {
		auto key = it->first.as<std::string>();
		auto value = it->second;
		switch (value.Type()) {
			case YAML::NodeType::Map:
				// parent config test
				if(value.path
				in
				self.parent_config_map:
				import_module = self.parent_config_map[value.path].__module__
				class_name = self.parent_config_map[value.path].__name__
				is_parent = True
				else:
				self._generate_config(value, key)
				import_module = "{}.{}.{}".format(self.main_module_name, self.output_dir, key)
				class_name = get_class_name(key)
				is_parent = False
				object_properties[key] = (import_module, class_name, value, is_parent)
				else:
		}
		if isinstance(value, ConfigDataObject):
		if value.is_array:
		item_name = "plain"
		if isinstance(value.get(0), ConfigDataObject):
		item_name \
 = config_map.path + "_" + key + "_item"
		if config_map.path else key + "_item"
		self._generate_config(value.get(0), item_name)
		array_properties[key] = (value, item_name)

		properties[key] = value
	}

	template_filename = 'config_template.txt'
	template_data = pkgutil.get_data("fconfig.templates", template_filename)
	lookup = TemplateLookup(module_directory="/tmp")
	class_template = Template(template_data, lookup=lookup)

	if not os.path.exists(self.output_dir):
	os.makedirs(self.output_dir)

	class_name = get_class_name(map_name)
	if is_root:
		parent_parameter_strings = []
	for pc in self.parent_config:
	parent_parameter_strings.append("({}, '{}')".format(pc[0].__name__, pc[1]))
	parent_parameter_strings.append("({}, None)".format(class_name))
	self.rendered_config_objects[map_name] = class_template.render(
		is_root=True,
		properties=properties,
		object_properties=object_properties,
		array_properties=array_properties,
		class_name=class_name,
		map_name=map_name,
		parent_parameter_string=", ".join(parent_parameter_strings))
	else:
	self.rendered_config_objects[map_name] = class_template.render(
		is_root=False,
		properties=properties,
		object_properties=object_properties,
		array_properties=array_properties,
		class_name=class_name)
}
