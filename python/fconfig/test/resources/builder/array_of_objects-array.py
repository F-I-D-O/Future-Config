
from array_of_objects_item import ArrayOfObjectsItem


class Test:

    def __init__(self, properties: dict = None):
        self.array_of_objects = []

        for item in properties.get("array_of_objects"):
            self.array_of_objects.append(ArrayOfObjectsItem(item))
        pass
