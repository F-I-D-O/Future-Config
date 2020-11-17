
import fconfig.configuration

from fconfig.config import Config


class Test(Config):

    def __init__(self, properties: dict = None):
        self.array = [array_member for array_member in properties.get("array")]
        pass


config: Test = fconfig.configuration.load((Test, None))
