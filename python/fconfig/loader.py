import pkgutil
import fconfig.parser as parser
from fconfig.parser import Parser
from typing import Tuple

import fconfig.merger as merger
from fconfig.config_data_object import ConfigDataObject
from fconfig.config_property import ConfigProperty

from fconfig.resolver import Resolver

DEFAULT_CONFIG_FILE_NAME = "config.cfg"


class ConfigSource:

	def __init__(self, source: str, *path_in_config: str):
		self.source = source
		self.path_in_config = path_in_config

def load_config_data(*config_source_definitions: ConfigSource, use_builder_directives=False):
	config_data_list = []

	for config_source_definition in config_source_definitions:
		source = config_source_definition.source
		config_map_from_source = Parser(use_builder_directives).parse_config_source(_get_config_content(source))

		if config_source_definition.path_in_config:
			config_map_from_source \
				= _change_config_context(config_map_from_source, config_source_definition.path_in_config)

		config_data_list.append(config_map_from_source)

	config_root = Resolver(merger.merge(config_data_list)).resolve_variables()

	# LOGGER.debug(configRoot.getStringForPrint());

	return config_root


def _change_config_context(config_map_from_source: ConfigDataObject, path: Tuple[str,...]):

	def add_prefix(config_property: ConfigProperty, object_name: str):
		result = parser.REFERENCE_PATTERN.sub(config_property.value, r"\$" + object_name + ".$1")
		config_property.set_value(result)

	for object_name in reversed(path):

		# add prefix to all variables path_in_config
		config_map_from_source.iterate_properties(lambda x: parser.contains_variable(x), add_prefix)

		# move object to new parent
		parent_map = ConfigDataObject(False)
		parent_map.put(object_name, ConfigDataObject(config_map_from_source.is_array, parent_map, object_name,
													 config_map_from_source.config_object))
		config_map_from_source = parent_map

	return config_map_from_source


def _get_config_content(source: str):

	# source defined as path
	if source.endswith(".cfg"):
		with open(source) as f:
			content = f.readlines()

	# source defined as resource
	else:
		content = pkgutil.get_data(source, DEFAULT_CONFIG_FILE_NAME).decode("utf-8").split("\n")

	return content


