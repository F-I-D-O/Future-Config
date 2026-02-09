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

} // namespace fc
