//
// Created by david on 2024-06-06.
//
#include "Config_object.h"

void generate_config(Config_object parent_config: Tuple[C, str]):
"""
At this time, this function has to be called from the project root!
"""
root_module_name = os.path.basename(os.path.normpath(os.path.dirname(sys.modules['__main__'].__file__)))
root_config_object_name_name = root_module_name + "_config"

config_package: str = ".".join((root_module_name, loader.DEFAULT_CONFIG_PACKAGE))

Builder(config_package, root_module_name, root_config_object_name_name,
	loader.DEFAULT_GENERATED_CONFIG_PACKAGE, parent_config).build_config()