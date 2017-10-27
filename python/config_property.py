from string import Formatter


class ConfigProperty:

    def __init__(self, config_data_object, key, value):
        self.config_data_object = config_data_object
        self.key = key
        self.value = value

    def set_value(self, new_value):
        self.config_data_object[self.key] = new_value

    def get_path(self):
        if not self.config_data_object.getPath:
            return self.key
        elif isinstance(self.key, int):
            return "{}[{}]".format(self.config_data_object.getPath, self.key)
        else:
            return "{}.{}".format(self.config_data_object.getPath, self.key)
