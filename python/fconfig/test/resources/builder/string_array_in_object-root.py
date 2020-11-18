
from root_module_name.some_package.comparison import Comparison


class Test:

    def __init__(self, properties: dict = None):
        self.comparison = Comparison(properties.get("comparison"))
        pass
