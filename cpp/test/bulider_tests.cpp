//
// Created by david on 2024-04-28.
//
#include <algorithm>
#include <cctype>

#include <gtest/gtest.h>

#include "Builder.h"
#include "format.h"
#include "resources.h"

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
	YAML::Node config = YAML::Load("{var: test_var}");
	check_generated_config(config, TEST_DATA_DIR / "one_var.h");
}

TEST(Builder, test_array) {
	YAML::Node config = YAML::Load("{array: [1, 2, 3]}");
	check_generated_config(config, TEST_DATA_DIR / "array.h");
}

TEST(Builder, test_string_array_in_object) {
	YAML::Node config = YAML::Load(
		R"({comparison:
					{
						experiment_names: ['IH-constraint4min-capacity1', 'IH-constraint4min', 'vga-constraint4min']
					}
				}
		)"
	);
	check_generated_config(config, TEST_DATA_DIR / "string_array_in_object.h");
}

// not supported yet
// TEST(Builder, test_array_of_objects) {
// 	YAML::Node config = YAML::Load(
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
	YAML::Node config = YAML::Load(
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

} // namespace fc