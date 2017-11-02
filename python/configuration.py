from builder import Builder

DEFAUL_CONFIG_FILE_PATH = "config.cfg"

DEFAUL_CONFIG_OUTPUT_DIR = "config"


def generate_config(root_class_name:str ):
	Builder(DEFAUL_CONFIG_FILE_PATH, root_class_name, DEFAUL_CONFIG_OUTPUT_DIR).buildConfig()