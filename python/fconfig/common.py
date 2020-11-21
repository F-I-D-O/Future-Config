

class ConfigException(Exception):

    def __init__(self, message: str, path: str):
        self.message = message
        self.path = path
