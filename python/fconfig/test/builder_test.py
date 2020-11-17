import pkgutil
import mako.exceptions

from fconfig.builder import Builder
from fconfig.config_data_object import ConfigDataObject


def check_generated_code_for_config_object(config_object: ConfigDataObject, reference_filename: str, root=False):
    builder = Builder("config_package", "root_module_name", "root_config_object_name_name",
                      "loader.DEFAULT_GENERATED_CONFIG_PACKAGE", [])
    resource = pkgutil.get_data("fconfig.test.resources.builder", reference_filename)
    expected_content = resource.decode("utf-8")

    try:
        builder._generate_config(config_object, "test", is_root=root)
        out = builder.rendered_config_objects["test"]
    except:
        error = mako.exceptions.text_error_template().render()
        print(error)

    assert out == expected_content


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

