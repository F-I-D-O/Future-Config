import os
import pkgutil

from fconfig.config_data_object import ConfigDataObject
from mako.lookup import TemplateLookup
from mako.template import Template

from fconfig import loader
from fconfig.loader import ConfigSource


def get_class_name(snake_case_property_name):
	components = snake_case_property_name.split('_')
	return "".join(x.title() for x in components)


class Builder:

	# class ObjectProperty:
	#
	# 	def __init__(self, key):
	# 		self.key = key
	# 		self.class_name = Builder.get_className(key)
	# 		self.module_name = Builder._get_module_name(key)

	def __init__(self, config_file_path: str, root_class_name: str, output_dir: str):
		self.config_file_path = config_file_path
		self.root_class_name = root_class_name
		self.output_dir = output_dir

	def build_config(self):
		"""
		Starts the building process.
		"""

		self._delete_old_files()

		config_map = loader.load_config_data(ConfigSource(self.config_file_path))
		self._generate_config(config_map, self.root_class_name)

	def _delete_old_files(self):
		if os.path.isdir(self.output_dir):
			for the_file in os.listdir(self.output_dir):
				file_path = os.path.join(self.output_dir, the_file)
				try:
					if os.path.isfile(file_path):
						os.unlink(file_path)
					# elif os.path_in_config.isdir(file_path): shutil.rmtree(file_path)
				except Exception as e:
					print(e)

	def _generate_config(self, config_map: ConfigDataObject, map_name: str):
		properties = {}
		object_properties = {}
		array_properties = {}
		for key, value in config_map.items():
			if isinstance(value, ConfigDataObject):
				if value.is_array:
					if isinstance(value[0], ConfigDataObject):
						item_name \
							= config_map.path + "_" + key + "_item" if config_map.path else key + "_item"
						self._generate_config(value[0], item_name)
					array_properties[key] = value
				else:
					self._generate_config(value, key)
					object_properties[key] = value
			else:
				properties[key] = value

		teplate_data = pkgutil.get_data("fconfig.configuration", 'config_template.txt')
		lookup = TemplateLookup(module_directory="/tmp")
		class_template = Template(teplate_data, lookup=lookup)

		if not os.path.exists(self.output_dir):
			os.makedirs(self.output_dir)

		output_file = open("{}/{}.py".format(self.output_dir, map_name), 'w')
		output_file.write(class_template.render(properties=properties, object_properties=object_properties,
												array_properties=array_properties, class_name=get_class_name(map_name)))


