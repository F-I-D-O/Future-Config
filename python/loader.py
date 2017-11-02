import merger
import parser

from typing import Tuple
from parser import Parser
from resolver import Resolver
from config_data_object import ConfigDataObject
from config_property import ConfigProperty


# def load_config_data(Object... configSources) throws IOException {
# 		ConfigSource[] configSourceDefinitions = new ConfigSource[configSources.length];
# 		for (int i = 0; i < configSources.length; i++) {
# 			configSourceDefinitions[i] = new ConfigSource(configSources[i], (String[]) null);
# 		}
# 		return loadConfigData(configSourceDefinitions);
# 	}

class ConfigSource:

	def __init__(self, filepath: str, *path_in_config: str):
		self.filepath = filepath
		self.path_in_config = path_in_config


def load_config_data(*config_source_definitions: ConfigSource, use_builder_directives=False):
	config_data_list = []

	for config_source_definition in config_source_definitions:
		source = config_source_definition.filepath
		config_map_from_source = Parser(use_builder_directives).parse_config_file(source)

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
		parent_map = ConfigDataObject()
		parent_map.put(object_name, ConfigDataObject(config_map_from_source.config_object, parent_map, object_name))
		config_map_from_source = parent_map

	return config_map_from_source



