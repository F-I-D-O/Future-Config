import pkgutil
import os
import sys
import inspect
import fconfig.loader as loader

from os.path import dirname as dirname
from typing import Tuple, TypeVar, Type
from fconfig.builder import Builder
from fconfig.config import Config
from fconfig.loader import ConfigSource


DEFAULT_CONFIG_OUTPUT_DIR = "config"

C = TypeVar('C', bound=Config)
CC = TypeVar('CC', bound=Config)


def generate_config():
	"""
	At this time, this function has to be called from the project root!
	"""

	root_module_name = os.path.basename(os.path.normpath(os.path.dirname(sys.modules['__main__'].__file__))) + "_config"
	default_config_file_name = root_module_name + ".cfg"
	Builder(loader.DEFAULT_CONFIG_FILE_NAME, root_module_name, DEFAULT_CONFIG_OUTPUT_DIR).build_config()

	# /**
	#  * Loads configuration using config file, client config file and local config file.
	#  * @param <C> Config type
	#  * @param <CC> Client config type
	#  * @param generatedConfig Config class
	#  * @param clientGeneratedConfig Client config class
	#  * @param clientLocalConfigFile Local config file
	#  * @param keyInClient Config object name in client config.
	#  */


def load(generated_config: Type[C], client_generated_config: Type[CC]=None, client_local_config_file: str=None,
		 key_in_client: str=None) -> Tuple[C,CC]:
	config_sources = []

	default_config_source = ConfigSource(_get_project_main_module_name(generated_config), key_in_client)
	config_sources.append(default_config_source)

	if(client_generated_config):
		default_client_config_source = ConfigSource(_get_project_main_module_name(client_generated_config))
		config_sources.append(default_client_config_source)

	if client_local_config_file:
		local_client_config_source = ConfigSource(client_local_config_file)
		config_sources.append(local_client_config_source)

	config = loader.load_config_data(*config_sources)

	config_dict = config.get_internal_objects()

	if client_generated_config:
		config_instance = generated_config(config_dict[key_in_client])
		client_config_instance = client_generated_config(config_dict)

		return client_config_instance, config_instance
	else:
		return None, generated_config(config_dict)


def _get_project_main_module_name(generated_config: type):
	return generated_config.__module__.split(".")[0]
	# return pkgutil.get_data("roadmaptools", DEFAULT_CONFIG_FILE_NAME)




# def _get_config_file_name(generated_config: type) -> str:
# 	os.path.basename(os.path.normpath(dirname(dirname(inspect.getfile(generated_config))))) + "_config"

# def _get_config_file_path(generated_config: type) -> str:
# 	dirname(dirname(inspect.getfile(generated_config))) + "/" + DEFAULT_CONFIG_FILE_NAME

