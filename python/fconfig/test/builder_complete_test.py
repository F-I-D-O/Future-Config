import fconfig.test.common as common

from fconfig.test.builder_test import check_generated_code_for_config_object, check_generated_code_for_config_objects


def test_one_var():
    config = common.load_test_config_file("one_var")
    check_generated_code_for_config_object(config, "one_var.py")


def test_array():
    config = common.load_test_config_file("simpleArray")
    check_generated_code_for_config_object(config, "array_test.py")


def test_array_root():
    config = common.load_test_config_file("simpleArray")
    check_generated_code_for_config_object(config, "array_test_root.py", True)


def test_string_array_in_object():
    config = common.load_test_config_file("string_array_in_object")
    check_generated_code_for_config_objects(
        config,
        ["string_array_in_object-root.py", "string_array_in_object-array.py"],
        ["test", "comparison"],
        False)
