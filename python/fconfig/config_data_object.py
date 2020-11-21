
from typing import Callable, Union
from io import StringIO

from fconfig.config_property import ConfigProperty


class ConfigDataObject:

	def __init__(self, is_array: bool, parent_config_object=None, key_in_parent: Union[str, int]=None, config_object: dict=None):
		self.is_array = is_array
		self.config_object = config_object if config_object else {}
		self.parent_config_object = parent_config_object
		self.key_in_parent = key_in_parent
		self.path = self.create_path()

	def __iter__(self):
		return iter(self.config_object)

	def __getitem__(self, item):
		return self.get(item)

	def items(self):
		return self.config_object.items()

	def put(self, key, value):
		self.config_object[key] = value

	def contains(self, key: str) -> bool:
		return key in self.config_object

	def get(self, key: str):
		return self.config_object[key]

	def get_size(self):
		return len(self.config_object)

	def move_to(self, parent_config_object, key_in_parent):
		self.parent_config_object = parent_config_object
		self.key_in_parent = key_in_parent
		self.create_path()

	def get_string_for_print(self):
		string_builder = StringIO()

		def append_line(config_property, string_builder):
			print("{}: {}".format(config_property.get_path(), config_property.value), file=string_builder)

		self.iterate_properties(lambda x: True, append_line, string_builder)

		return string_builder.getvalue()

	def create_path(self):
		path = ""
		current_object = self
		while current_object.key_in_parent:
			key_in_parent = current_object.key_in_parent
			if isinstance(key_in_parent, int):
				path = "[{}].{}".format(key_in_parent, path)
			else:
				if current_object == self:
					path = key_in_parent
				elif path.startswith("["):
					path = key_in_parent + path
				else:
					path = key_in_parent + "." + path
			current_object = current_object.parent_config_object
		return path

	def iterate_properties(self, filter_function: Callable[[any], bool],
					iter_function: Callable[[ConfigProperty, any], None], out=None):
		for key, value in self.items():
			if isinstance(value, ConfigDataObject):
				value.iterate_properties(filter_function, iter_function, out)
			elif filter_function(value):
				config_property = ConfigProperty(self, key, value)
				iter_function(config_property, out)

	def get_internal_objects(self):
		out = {}
		for key, value in self.items():
			if isinstance(value, self.__class__):
				out[key] = value.get_internal_objects()
			else:
				out[key] = value

		return out
