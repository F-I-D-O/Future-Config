#include "gtest/gtest.h"

#include <vector>

#include "Command_line_parser.h"
#include "./common.h"

namespace fc {

namespace {

Config_object parse_arguments(const std::vector<const char*>& arguments, const Config_object& master_config) {
	// the first argument is the program name
	std::vector<const char*> argv{"program_name"};
	argv.insert(argv.end(), arguments.begin(), arguments.end());
	return Command_line_parser().parse(static_cast<int>(argv.size()), argv.data(), master_config);
}

const Config_object master_config(YAML::Load(
	R"({
	flag: false,
	enabled_flag: true,
	number: 1,
	name: test,
	list: [1, 2],
	object: {nested_flag: false}
})"
));

} // namespace

TEST(Command_line_parser, key_with_value) {
	const auto config = parse_arguments({"--number", "5"}, master_config);

	const Config_object expected(YAML::Load("{number: 5}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, switch_as_last_argument) {
	const auto config = parse_arguments({"--number", "5", "--flag"}, master_config);

	const Config_object expected(YAML::Load("{number: 5, flag: true}"));
	compare_config_objects(expected, config);
	EXPECT_TRUE(config.get<bool>("flag"));
}

TEST(Command_line_parser, switch_followed_by_another_key) {
	const auto config = parse_arguments({"--flag", "--number", "5"}, master_config);

	const Config_object expected(YAML::Load("{flag: true, number: 5}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, multiple_switches) {
	const auto config = parse_arguments({"--flag", "--enabled_flag"}, master_config);

	const Config_object expected(YAML::Load("{flag: true, enabled_flag: true}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, nested_switch) {
	const auto config = parse_arguments({"--nested_flag"}, master_config);

	const Config_object expected(YAML::Load("{object: {nested_flag: true}}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, boolean_with_explicit_value) {
	const auto config = parse_arguments({"--enabled_flag", "false", "--flag", "true"}, master_config);

	const Config_object expected(YAML::Load("{enabled_flag: false, flag: true}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, switch_for_non_boolean_property_throws) {
	EXPECT_THROW(parse_arguments({"--number"}, master_config), std::runtime_error);
	EXPECT_THROW(parse_arguments({"--name", "--flag"}, master_config), std::runtime_error);
	EXPECT_THROW(parse_arguments({"--list"}, master_config), std::runtime_error);
	EXPECT_THROW(parse_arguments({"--object"}, master_config), std::runtime_error);
}

TEST(Command_line_parser, unknown_switch_is_ignored) {
	const auto config = parse_arguments({"--unknown", "--flag"}, master_config);

	const Config_object expected(YAML::Load("{flag: true}"));
	compare_config_objects(expected, config);
}

} // namespace fc
