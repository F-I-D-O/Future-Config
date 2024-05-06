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

TEST(Resolver, test_multiple_variables) {
	YAML::Node yaml_config = YAML::Load(
		R"({
			string: String,
			string2: "${string} composed",
			string4: "${string}${string3}",
			string3: "composed ${string}",
			string5: "${string} ${string}",
			string6: "another ${string} composed"
		})"
	);
	Resolver resolver(yaml_config);
	YAML::Node resolved_config = resolver.resolve();

	ASSERT_EQ("String", resolved_config["string"].as<std::string>());
	ASSERT_EQ("String composed", resolved_config["string2"].as<std::string>());
	ASSERT_EQ("composed String", resolved_config["string3"].as<std::string>());
	ASSERT_EQ("Stringcomposed String", resolved_config["string4"].as<std::string>());
	ASSERT_EQ("String String", resolved_config["string5"].as<std::string>());
	ASSERT_EQ("another String composed", resolved_config["string6"].as<std::string>());
}