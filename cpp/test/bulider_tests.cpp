//
// Created by david on 2024-04-28.
//
#include <algorithm>
#include <cctype>

#include <gtest/gtest.h>

#include "Builder.h"

const auto TEST_DATA_DIR = fs::path("data/test/builder");

std::string remove_empty_lines(const std::string& str) {
	std::string result;
	std::istringstream iss(str);
	std::string line;
	while (std::getline(iss, line)) {
		if (!line.empty() && !std::all_of(line.begin(), line.end(),isspace)) {
			result += line + "\n";
		}
	}
	return result;
}

void compare_generated_config_content(const std::string& expected, const std::string& actual) {
	ASSERT_EQ(remove_empty_lines(expected), remove_empty_lines(actual));
}

void check_generated_config(const YAML::Node& config, const fs::path& expected_file) {
	fs::path output_dir = fs::temp_directory_path() / "test_output";
	std::string root_object_name = std::format("{}_config", expected_file.stem().string());
	Builder builder(config, output_dir, root_object_name);
	builder.build_config();

	fs::path actual_file = output_dir / std::format("{}_config.h", root_object_name);
	ASSERT_TRUE(fs::exists(actual_file));

	std::ifstream expected_file_stream(expected_file);
	std::ifstream actual_file_stream(actual_file);

	std::string expected_content(std::istreambuf_iterator<char>{expected_file_stream}, {});
	std::string actual_content(std::istreambuf_iterator<char>{actual_file_stream}, {});

	compare_generated_config_content(expected_content, actual_content);
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
	check_generated_config(config, TEST_DATA_DIR / "array.h");
}
