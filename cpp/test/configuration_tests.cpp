#include <gtest/gtest.h>

#include "future-config/configuration.h"
#include "future-config/resources.h"
#include "config/test_config.h"

namespace fc {

TEST(configuration_test, load_with_default_options) {
	auto config = load<Test_config>();
	EXPECT_EQ(config.test_par, 1);
}

TEST(configuration_test, load_external_load_options) {
	Load_options load_options;
	auto config = load<Test_config>(load_options);
	EXPECT_EQ(config.test_par, 1);
}

TEST(configuration_test, load_with_local_config) {
	auto local_config_path = get_resource_path("test/local_config.yaml");
	auto config = load<Test_config>({.local_config_path = local_config_path});
	EXPECT_EQ(config.test_par, 2);
}

TEST(configuration_test, load_with_args) {
	int argc = 1;
	const char** argv = new const char*[argc];
	argv[0] = "program_name";
	Load_options load_options{.command_line_arguments = std::make_pair(argc, argv)};
	auto config = load<Test_config>(load_options);
	EXPECT_EQ(config.test_par, 1);
	delete[] argv;
}

TEST(configuration_test, load_with_command_line_value) {
	const char* argv[] = {"program_name", "--test_par", "3"};
	auto config = load<Test_config>({.command_line_arguments = std::make_pair(3, argv)});
	EXPECT_EQ(config.test_par, 3);
}

TEST(configuration_test, load_with_unknown_command_line_argument) {
	const char* argv[] = {"program_name", "--unknown", "3"};
	EXPECT_THROW(load<Test_config>({.command_line_arguments = std::make_pair(3, argv)}), std::runtime_error);
}

TEST(configuration_test, load_with_allowed_command_line_argument) {
	const char* argv[] = {"program_name", "--local_config", "config.yaml", "--test_par", "3"};
	auto config = load<Test_config>({
		.command_line_arguments = std::make_pair(5, argv),
		.allowed_command_line_arguments = {"local_config"}
	});
	EXPECT_EQ(config.test_par, 3);
}

} // namespace fc
