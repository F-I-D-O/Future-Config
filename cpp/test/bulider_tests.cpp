//
// Created by david on 2024-04-28.
//
#include <algorithm>
#include <cctype>

#include <gtest/gtest.h>

#include "Builder.h"
#include "./common.h"
#include "future-config/format.h"
#include "future-config/loading.h"
#include "future-config/resources.h"

namespace fc {

const auto TEST_DATA_DIR = get_resource_path("test/builder");

std::string remove_empty_lines(const std::string& str) {
	std::string result;
	std::istringstream iss(str);
	std::string line;
	while(std::getline(iss, line)) {
		if(!line.empty() && !std::all_of(line.begin(), line.end(), isspace)) {
			result += line + "\n";
		}
	}
	return result;
}

std::string unify_line_endings(const std::string& str) {
	std::string result = str;

	// replace all "\r\n" with just "\n"
	std::string::size_type pos = 0;
	while((pos = result.find("\r\n", pos)) != std::string::npos) {
		result.replace(pos, 2, "\n");
	}

	return result;
}

std::string cleanup_newlines(const std::string& str) {
	std::string empty_lines_removed = remove_empty_lines(str);
	std::string unified_line_endings = unify_line_endings(empty_lines_removed);
	return unified_line_endings;
}

void compare_generated_config_content(const std::string& expected, const std::string& actual) {
	ASSERT_EQ(cleanup_newlines(expected), cleanup_newlines(actual));
}

void check_generated_config(
	const Config_object& config,
	const fs::path& expected_file,
	std::unordered_map<std::string, std::tuple<std::string, std::string>>& dependency_config_map
) {
	fs::path output_dir = fs::temp_directory_path() / "test_output";
	std::string root_object_name = format::format("{}_config", expected_file.stem().string());
	Builder builder(config, output_dir, root_object_name, dependency_config_map);
	builder.build_config();

	fs::path actual_file = output_dir / format	::format("{}.h", root_object_name);
	ASSERT_TRUE(fs::exists(actual_file));

	std::ifstream expected_file_stream(expected_file);
	std::ifstream actual_file_stream(actual_file);

	std::string expected_content(std::istreambuf_iterator<char>{expected_file_stream}, {});
	std::string actual_content(std::istreambuf_iterator<char>{actual_file_stream}, {});

	compare_generated_config_content(expected_content, actual_content);
}

void check_generated_config(const Config_object& config, const fs::path& expected_file) {
	std::unordered_map<std::string, std::tuple<std::string, std::string>> dependency_config_map;
	check_generated_config(config, expected_file, dependency_config_map);
}

TEST(Builder, test_one_var) {
	Config_object config = parse_yaml("{var: test_var}");
	check_generated_config(config, TEST_DATA_DIR / "one_var.h");
}

TEST(Builder, test_array) {
	Config_object config = parse_yaml("{array: [1, 2, 3]}");
	check_generated_config(config, TEST_DATA_DIR / "array.h");
}

TEST(Builder, test_string_array_in_object) {
	Config_object config = parse_yaml(
		R"({comparison:
					{
						experiment_names: ['IH-constraint4min-capacity1', 'IH-constraint4min', 'vga-constraint4min']
					}
				}
		)"
	);
	check_generated_config(config, TEST_DATA_DIR / "string_array_in_object.h");
}

TEST(Builder, test_hyphen_in_key) {
	Config_object config = parse_yaml("{test-var: 123}");
	check_generated_config(config, TEST_DATA_DIR / "hyphen_in_key.h");
}

// not supported yet
// TEST(Builder, test_array_of_objects) {
// 	Config_object config = parse_yaml(
// 		R"({array_of_objects:
// 					[
// 						{property: "object's 1 property"},
// 						{property: "object's 2 property"}
// 					]
// 				}
// 		)"
// 	);
// 	check_generated_config(config, TEST_DATA_DIR / "array_of_objects.h");
// }

TEST(Builder, test_parent_config) {
	Config_object config = parse_yaml(
		R"({parent_config:
					{
						var: test_var
					}
				}
		)"
	);

	std::unordered_map<std::string, std::tuple<std::string, std::string>>
		dependency_config_map{{"parent_config", {"dependency.h", "Parent_config"}}};

	check_generated_config(config, TEST_DATA_DIR / "dependency_test.h", dependency_config_map);
}

TEST(Builder, test_nested_parent_config) {
	Config_object config = parse_yaml(
		R"({libraries:
					{
						parent_config: {var: test_var}
					}
				}
		)"
	);

	// the dependency is bound to the nested object
	std::unordered_map<std::string, std::tuple<std::string, std::string>>
		dependency_config_map{{"libraries.parent_config", {"dependency.h", "Parent_config"}}};

	check_generated_config(config, TEST_DATA_DIR / "dependency_nested_test.h", dependency_config_map);
}

TEST(Builder, test_parent_config_key_outside_bind_path) {
	Config_object config = parse_yaml(
		R"({parent_config:
					{
						var: test_var
					}
				}
		)"
	);

	// the dependency is bound to a different object than the root parent_config, which is therefore a regular object
	std::unordered_map<std::string, std::tuple<std::string, std::string>>
		dependency_config_map{{"libraries.parent_config", {"dependency.h", "Parent_config"}}};

	check_generated_config(config, TEST_DATA_DIR / "dependency_outside_bind_path_test.h", dependency_config_map);
}

TEST(Builder, test_parent_config_from_definitions) {
	// dependency config definition in the format used by the builder tool
	auto config_definitions = parse_dependency_config_definitions({format::format(
		"{},libraries.parent_config,parent-lib/config/parent-lib_config.h",
		get_resource_path("test/binding/dependency.yaml").string()
	)});
	const auto config = load_config_for_builder(config_definitions);

	fs::path output_dir = fs::temp_directory_path() / "test_output";
	std::string root_object_name = "dependency_from_definitions_test_config";
	Builder builder(config, output_dir, root_object_name, config_definitions);
	builder.build_config();

	fs::path actual_file = output_dir / format::format("{}.h", root_object_name);
	ASSERT_TRUE(fs::exists(actual_file));

	// the dependency class name is determined from the name of the dependency config header
	std::ifstream expected_file_stream(TEST_DATA_DIR / "dependency_from_definitions_test.h");
	std::ifstream actual_file_stream(actual_file);

	std::string expected_content(std::istreambuf_iterator<char>{expected_file_stream}, {});
	std::string actual_content(std::istreambuf_iterator<char>{actual_file_stream}, {});

	compare_generated_config_content(expected_content, actual_content);
}

TEST(Builder, test_root_object_name_sanitize) {
	Config_object config = parse_yaml(
		R"({var: test_var})"
	);

	fs::path output_dir = fs::temp_directory_path() / "test_output";
	std::string root_object_name = "test-project-name_config";
	std::unordered_map<std::string, std::tuple<std::string, std::string>> dependency_config_map;
	Builder builder(config, output_dir, root_object_name, dependency_config_map);
	builder.build_config();

	fs::path actual_file = output_dir / format::format("{}.h", root_object_name);
	ASSERT_TRUE(fs::exists(actual_file));

	std::ifstream expected_file_stream(TEST_DATA_DIR / "root_object_name_sanitization.h");
	std::ifstream actual_file_stream(actual_file);

	std::string expected_content(std::istreambuf_iterator<char>{expected_file_stream}, {});
	std::string actual_content(std::istreambuf_iterator<char>{actual_file_stream}, {});

	compare_generated_config_content(expected_content, actual_content);
}

} // namespace fc