import pkgutil
import mako.exceptions
import os

from typing import List

from fconfig.builder import Builder
from fconfig.config_data_object import ConfigDataObject


def remove_empty_lines(string: str) -> str:
    return os.linesep.join([s for s in string.splitlines() if s.strip()])


def compare_outputs(actual_output: str, expected_output: str):
    assert remove_empty_lines(actual_output) == remove_empty_lines(expected_output)


def check_generated_code_for_config_object(config_object: ConfigDataObject, reference_filename: str, root=False):
    check_generated_code_for_config_objects(config_object, [reference_filename], ["test"], root)


def check_generated_code_for_config_objects(
        config_object: ConfigDataObject,
        reference_filenames: List[str],
        config_keys: List[str] = ["test"],
        root=False):
    builder = Builder("config_package", "root_module_name", "root_config_object_name",
                      "some_package", [])
    try:
        builder._generate_config(config_object, config_keys[0], is_root=root)
    except:
        error = mako.exceptions.text_error_template().render()
        print(error)

    if builder.rendered_config_objects:
        for index, reference_filename in enumerate(reference_filenames):
            resource = pkgutil.get_data("fconfig.test.resources.builder", reference_filename)
            expected_content = resource.decode("utf-8")
            out = builder.rendered_config_objects[config_keys[index]]
            compare_outputs(out, expected_content)


def test_one_var():
    config_inner = {"var": "test_var"}
    config_map = ConfigDataObject(config_object=config_inner, is_array=False)
    check_generated_code_for_config_object(config_map, "one_var.py")


def test_array():
    array = ConfigDataObject(True)
    array.put(0, 1)
    array.put(1, 2)
    array.put(2, 3)
    config_inner = {"array": array}
    config_map = ConfigDataObject(config_object=config_inner, is_array=False)
    check_generated_code_for_config_object(config_map, "array_test.py")


def test_array_root():
    array = ConfigDataObject(True)
    array.put(0, 1)
    array.put(1, 2)
    array.put(2, 3)
    config_inner = {"array": array}
    config_map = ConfigDataObject(config_object=config_inner, is_array=False)
    check_generated_code_for_config_object(config_map, "array_test_root.py", True)


def test_string_array_in_object():
    array = ConfigDataObject(True)
    array.put(0, 'IH-constraint4min-capacity1')
    array.put(1, 'IH-constraint4min')
    array.put(2, 'vga-constraint4min')
    object = ConfigDataObject(False)
    object.put("experiment_names", array)

    config_inner = {"comparison": object}
    config_map = ConfigDataObject(config_object=config_inner, is_array=False)
    check_generated_code_for_config_objects(
        config_map,
        ["string_array_in_object-root.py", "string_array_in_object-array.py"],
        ["test", "comparison"],
        False)


def test_array_of_objects():
    object_1 = ConfigDataObject(False)
    object_1.put("property", "object's 1 property")
    object_2 = ConfigDataObject(False)
    object_2.put("property", "object's 2 property")

    array = ConfigDataObject(True)
    array.put(0, object_1)
    array.put(1, object_2)

    config_inner = {"array_of_objects": array}
    config_map = ConfigDataObject(config_object=config_inner, is_array=False)
    check_generated_code_for_config_objects(
        config_map,
        ["array_of_objects-array.py", "array_of_objects-object.py"],
        ["test", "array_of_objects_item"],
        False)