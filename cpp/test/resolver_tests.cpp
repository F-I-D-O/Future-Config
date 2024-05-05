//
// Created by Fido on 2024-05-05.
//

#include <gtest/gtest.h>

#include "Resolver.h"

TEST(Resolver, test_single_variable) {
	YAML::Node yaml_config = YAML::Load(
		R"({
			var: value,
			var_2: "${var}_2"
		})"
	);
	Resolver resolver(yaml_config);
	YAML::Node resolved_config = resolver.resolve();
	EXPECT_EQ(resolved_config["var_2"].as<std::string>(), "value_2");
}