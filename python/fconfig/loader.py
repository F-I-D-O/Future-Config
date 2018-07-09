import pkgutil
import fconfig.parser as parser
import fconfig.merger as merger

from typing import Tuple, List, TypeVar
from pkg_resources import resource_string
from fconfig.config_data_object import ConfigDataObject
from fconfig.config_property import ConfigProperty
from fconfig.resolver import Resolver
from fconfig.config import Config
from fconfig.parser import Parser

DEFAULT_CONFIG_FILE_NAME = "config.cfg"
DEFAULT_CONFIG_PACKAGE = "resources"
DEFAULT_GENERATED_CONFIG_PACKAGE = "config"

C = TypeVar('P', bound=Config)


class ConfigSource:

	def __init__(self, source: str, path_in_config: str=None):
		self.source = source
		self.path_in_config = path_in_config


def load_config_data(*config_source_definitions: ConfigSource, use_builder_directives=False):
	config_data_list = []

	for config_source_definition in config_source_definitions:
		source = config_source_definition.source
		config_map_from_source = Parser(use_builder_directives).parse_config_source(source)

		if config_source_definition.path_in_config:
			config_map_from_source \
				= _change_config_context(config_map_from_source, config_source_definition.path_in_config)

		config_data_list.append(config_map_from_source)

	config_root = Resolver(merger.merge(config_data_list)).resolve_values()

	# LOGGER.debug(configRoot.getStringForPrint());

	return config_root


def get_config_sources_from_def(parent_config: List[Tuple[C, str]]) -> List[ConfigSource]:
	config_sources: List[ConfigSource] = []
	for config_info in parent_config:
		config_content = get_master_config_content(generated_config=config_info[0])
		config_source = ConfigSource(config_content, config_info[1])
		config_sources.append(config_source)

	return config_sources


def get_master_config_content(package: str=None, generated_config: C=None) -> str:
	if not package:
		class_module: str = generated_config.__module__
		class_package = '.'.join(class_module.split('.')[:-1])
		package = class_package.replace(DEFAULT_GENERATED_CONFIG_PACKAGE, DEFAULT_CONFIG_PACKAGE)
	content = resource_string(package, DEFAULT_CONFIG_FILE_NAME).decode("utf-8").split("\n")
	return content


def _change_config_context(config_map_from_source: ConfigDataObject, path: str):

	def add_prefix(config_property: ConfigProperty, object_name: str):
		result = parser.REFERENCE_PATTERN.sub(r"\$" + object_name + r".\1", config_property.value)
		config_property.set_value(result)

	for object_name in reversed(path.split(".")):

		# add prefix to all variables path_in_config
		config_map_from_source.iterate_properties(lambda x: parser.contains_variable(x), add_prefix, object_name)

		# move object to new parent
		parent_map = ConfigDataObject(False)
		parent_map.put(object_name, ConfigDataObject(config_map_from_source.is_array, parent_map, object_name,
													 config_map_from_source.config_object))
		config_map_from_source = parent_map

	return config_map_from_source


def get_config_content(source: str):

	# source defined as path
	if source.endswith(".cfg"):
		with open(source) as f:
			content = f.readlines()

	# source defined as resource
	else:
		content = pkgutil.get_data(source, DEFAULT_CONFIG_FILE_NAME).decode("utf-8").split("\n")

	return content


