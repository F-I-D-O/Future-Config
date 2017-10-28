
from typing import Callable
from config_property import ConfigProperty


class ConfigDataObject:

	def __init__(self, parent_config_object, key_in_parent, is_array=False):
		self.is_array = is_array
		self.config_object = {}
		self.parent_config_object = parent_config_object
		self.key_in_parent = key_in_parent
		self.path = self.create_path()

	def __iter__(self):
		return iter(self.config_object)

	def items(self):
		return self.config_object.items()

	def put(self, key, value):
		self.config_object[key] = value

	def get_size(self):
		return len(self.config_object)

	def move_to(self, parent_config_object, key_in_parent):
		self.parent_config_object = parent_config_object
		self.key_in_parent = key_in_parent
		self.create_path()

	def get_string_for_print(self):
		out = ""

		def append_line(config_property, out):
			out += "{}: {}%n".format(config_property.get_path(), config_property.value)

		self.iterate_properties(lambda x: True, append_line)

	def create_path(self):
		path = ""
		current_object = self
		while current_object.key_in_parent:
			key_in_parent = current_object.keyInParent
			if isinstance(key_in_parent, int):
				path = "[{}].{}".format(key_in_parent, path)
			else:
				if current_object == self:
					path = key_in_parent
				elif path.startsWith("["):
					path = key_in_parent + path
				else:
					path = key_in_parent + "." + path
			current_object = current_object.parent_config_object
		return path

	def iterate_properties(
			self, filter_function: Callable[any, bool], iter_function: Callable[ConfigProperty, any, None], out=None):
		for key, value in self.items():
			if isinstance(value, ConfigDataObject):
				value.iterate_properties(filter_function, iter_function)
			elif filter_function(value):
				config_property = ConfigProperty(self, key, value)
				iter_function(config_property, out)

	# class Entry:
	#     def __init__(self, key, value):
	#         self.key = key
	#         self.value = value
	#
	# class VariableIterator:
	#
	#     def __init__(self, self, vars_only):
	#         self.context_stack = []
	#         self.vars_only = vars_only
	#         self.current_context = self.VariableIteratorContext(self, 0)
	#         self.current_object = self;
	#         self.current_entry = None;
	#
	#     def __iter__(self):
	#         return self
	#
	#     def next(self):
	#         # if not self.current_entry is None and self.is_requested_type(self.current_entry.value))
	#         # self.check_iterator()
	#
	#         while not self.current_context.finished() or self.context_stack:
	#             if self.current_context.finished():
	#                 self.current_context = self.context_stack.pop()
	#                 self.current_object = self.current_context.self
	#             else:
	#                 self.current_entry =
	#                 current_value = self.current_entry.value
	#                 if self.is_requested_type(current_value):
	#                     return ConfigProperty(self.current_object, self.current_entry.key, current_value)
	#                 elif isinstance(current_value, ConfigDataObject):
	#                     self.context_stack.push(self.current_context)
	#                     self.current_contex = self.VariableIteratorContext(current_value, 0)
	#                 self.check_iterator()
	#
	#     def is_requested_type(self, value):
	#         if self.varsOnly:
	#             return parser.contains_variable(value)
	#         else:
	#             return not isinstance(value, ConfigDataObject)
	#
	#     def check_iterator(self):
	#         while self.current_context.finished() and self.context_stack:
	#             self.current_context = self.context_stack.pop()
	#             self.current_object = self.current_context.self
	#
	#
	#     class VariableIteratorContext:
	#
	#         def __init__(self, self, position):
	#             self.self = self
	#             self.position = position
	#
	#         def finished(self):
	#             return self.position >= len(self.self)
	#
	#         def increment(self):
	#             self.position += 1