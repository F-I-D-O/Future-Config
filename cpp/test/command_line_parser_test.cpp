#include "gtest/gtest.h"

#include <vector>

#include "Command_line_parser.h"
#include "./common.h"

namespace fc {

namespace {

Config_object parse_arguments(
	const std::vector<const char*>& arguments,
	const Config_object& master_config,
	const std::vector<std::string>& allowed_arguments = {}
) {
	// the first argument is the program name
	std::vector<const char*> argv{"program_name"};
	argv.insert(argv.end(), arguments.begin(), arguments.end());
	return Command_line_parser().parse(static_cast<int>(argv.size()), argv.data(), master_config, allowed_arguments);
}

const Config_object master_config(parse_yaml(
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

	const Config_object expected(parse_yaml("{number: 5}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, switch_as_last_argument) {
	const auto config = parse_arguments({"--number", "5", "--flag"}, master_config);

	const Config_object expected(parse_yaml("{number: 5, flag: true}"));
	compare_config_objects(expected, config);
	EXPECT_TRUE(config.get<bool>("flag"));
}

TEST(Command_line_parser, switch_followed_by_another_key) {
	const auto config = parse_arguments({"--flag", "--number", "5"}, master_config);

	const Config_object expected(parse_yaml("{flag: true, number: 5}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, multiple_switches) {
	const auto config = parse_arguments({"--flag", "--enabled_flag"}, master_config);

	const Config_object expected(parse_yaml("{flag: true, enabled_flag: true}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, nested_switch) {
	const auto config = parse_arguments({"--nested_flag"}, master_config);

	const Config_object expected(parse_yaml("{object: {nested_flag: true}}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, boolean_with_explicit_value) {
	const auto config = parse_arguments({"--enabled_flag", "false", "--flag", "true"}, master_config);

	const Config_object expected(parse_yaml("{enabled_flag: false, flag: true}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, switch_for_non_boolean_property_throws) {
	EXPECT_THROW(parse_arguments({"--number"}, master_config), std::runtime_error);
	EXPECT_THROW(parse_arguments({"--name", "--flag"}, master_config), std::runtime_error);
	EXPECT_THROW(parse_arguments({"--list"}, master_config), std::runtime_error);
	EXPECT_THROW(parse_arguments({"--object"}, master_config), std::runtime_error);
}

TEST(Command_line_parser, unknown_argument_throws) {
	EXPECT_THROW(parse_arguments({"--unknown", "--flag"}, master_config), std::runtime_error);
	EXPECT_THROW(parse_arguments({"--flag", "--unknown", "value"}, master_config), std::runtime_error);
	// allowing one argument does not allow the others
	EXPECT_THROW(parse_arguments({"--unknown"}, master_config, {"local_config"}), std::runtime_error);
}

TEST(Command_line_parser, default_allowed_arguments_are_skipped) {
	const auto config = parse_arguments({"--help", "--flag", "--version"}, master_config);

	const Config_object expected(parse_yaml("{flag: true}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, allowed_arguments_are_skipped) {
	// allowed arguments are skipped together with their values, the default allowed arguments are still allowed
	const auto config = parse_arguments(
		{"--local_config", "path/to/config.yaml", "--number", "5", "--dry_run", "--help"},
		master_config,
		{"local_config", "dry_run"}
	);

	const Config_object expected(parse_yaml("{number: 5}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, positional_arguments_are_ignored) {
	const auto config = parse_arguments({"positional", "--number", "5", "-s", "another"}, master_config);

	const Config_object expected(parse_yaml("{number: 5}"));
	compare_config_objects(expected, config);
}

TEST(Command_line_parser, ambiguous_key_throws) {
	const Config_object ambiguous_config(parse_yaml(
		R"({
		key: 1,
		unique: 2,
		object: {key: 3, other_object: {deep: 4}},
		another_object: {deep: 5}
	})"
	));

	EXPECT_THROW(parse_arguments({"--key", "5"}, ambiguous_config), std::runtime_error);
	EXPECT_THROW(parse_arguments({"--deep", "5"}, ambiguous_config), std::runtime_error);
	// ambiguous keys cannot be allowed
	EXPECT_THROW(parse_arguments({"--key", "5"}, ambiguous_config, {"key"}), std::runtime_error);

	// unique keys in the same config still work
	const auto config = parse_arguments({"--unique", "7"}, ambiguous_config);
	const Config_object expected(parse_yaml("{unique: 7}"));
	compare_config_objects(expected, config);
}

} // namespace fc
