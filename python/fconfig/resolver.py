import logging
import fconfig.parser as parser
import re
import sys
from collections import deque

from fconfig.config_data_object import ConfigDataObject

from fconfig.config_property import ConfigProperty


class Resolver:
	STRING_VALUE_PATTERN_STRING = r"(?:'[^']*'|\"[^']*\")"

	OPERATOR_EXPRESSION_PATTERN \
		= re.compile(r"\s*({}|{})\s*([+])?".format(parser.NAME_PATTERN_STRING, STRING_VALUE_PATTERN_STRING))

	@staticmethod
	def _resolve_string_expression(operands_parsed: list, operators: list) -> str:
		result_sting = ""
		for operand in operands_parsed:
			result_sting += operand
		return result_sting

	@staticmethod
	def _parse_references(value: str) -> list:
		references = []
		matches = parser.REFERENCE_PATTERN.findall(value)
		for match in matches:
			references.append(match[0])
		return references

	@staticmethod
	def _terminate():
		sys.exit(1)

	def __init__(self, config_data_object: ConfigDataObject):
		self.root_object = config_data_object
		self.reference_queue = deque()

	def resolve_variables(self):
		self._add_all_variables_to_queue(self.root_object)
		self._process_queue()
		return self.root_object

	def _add_all_variables_to_queue(self, config_data_object: ConfigDataObject):
		def add_to_queue(config_property: ConfigProperty, reference_queue: deque):
			reference_queue.append(config_property)
			config_property.config_data_object.put(config_property.key, None)

		config_data_object.iterate_properties(
			lambda x: parser.contains_variable(x), add_to_queue, self.reference_queue)

	def _process_queue(self):
		last_queue_length = len(self.reference_queue)
		check_counter = last_queue_length
		while self.reference_queue:
			config_property = self.reference_queue.popleft()
			variable_value = self._parse_expression_with_references(config_property.value)
			if variable_value:
				config_property.config_data_object.put(config_property.key, variable_value)
			else:
				self.reference_queue.append(config_property)

			# check for unresolvable references
			if check_counter == 0:
				if last_queue_length == len(self.reference_queue):
					logging.critical("None of the remaining variables can be resolved. Remaining variables: %s",
							self.reference_queue)
					self._terminate()

				last_queue_length = len(self.reference_queue)
				check_counter = last_queue_length

				check_counter -= check_counter

	def _parse_expression_with_references(self, value: str):
		references = self._parse_references(value)
		for reference in references:
			variable = self._get_referenced_value(reference)
			if not variable:
				return None

			# now String variables only
			value = value.replace("$" + reference, "'" + variable + "'")

		if parser.OPERATOR_PATTERN.search(value):
			return self._parse_expression_with_operators(value)
		else:
			return parser.parse_simple_value(value)

	def _parse_expression_with_operators(self, value: str):
		operands = []
		operators = []
		match_list = Resolver.OPERATOR_EXPRESSION_PATTERN.findall(value)
		for match in match_list:
			operands.append(match[0])
			if len(match) > 1:
				operators.append(match[1])

		operands_parsed = []
		for operand in operands:
			operands_parsed.append(parser.parse_simple_value(operand))

		if isinstance(operands_parsed[0], str):
			return self._resolve_string_expression(operands_parsed, operators)

	def _get_referenced_value(self, reference: str):
		current_object = self.root_object
		parts = reference.split(".")

		for i, part in enumerate(parts):
			if current_object.get(part):
				if i < len(parts) - 1:
					current_object = current_object.get(part)
				else:
					return current_object.get(part)
			else:
				return None


		# class QueueEntry:
		# 	def __init__(self, key: any, value: str, parent: ConfigDataObject):
		# 		self.key = key
		# 		self.value = value
		# 		self.parent = parent
		#
		# 	def to_string():
		# 		return new ConfigProperty(parent, key, value).getPath() + ": " + value;