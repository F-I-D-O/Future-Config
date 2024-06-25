//
// Created by Fido on 2024-05-05.
//

#include <gtest/gtest.h>

#include "Resolver.h"

namespace fc {

TEST(Resolver, test_single_variable) {
	Config_object config = YAML::Load(
		R"({
			var: value,
			var_2: "${var}_2"
		})"
	);
	Resolver resolver(config);
	resolver.resolve();
	EXPECT_EQ(std::get<std::string>(config["var_2"]), "value_2");
}

TEST(Resolver, test_multiple_variables) {
	Config_object config = YAML::Load(
		R"({
			string: String,
			string2: "${string} composed",
			string4: "${string}${string3}",
			string3: "composed ${string}",
			string5: "${string} ${string}",
			string6: "another ${string} composed"
		})"
	);
	Resolver resolver(config);
	resolver.resolve();

	ASSERT_EQ("String", std::get<std::string>(config["string"]));
	ASSERT_EQ("String composed", std::get<std::string>(config["string2"]));
	ASSERT_EQ("composed String", std::get<std::string>(config["string3"]));
	ASSERT_EQ("Stringcomposed String", std::get<std::string>(config["string4"]));
	ASSERT_EQ("String String", std::get<std::string>(config["string5"]));
	ASSERT_EQ("another String composed", std::get<std::string>(config["string6"]));
}

} // namespace fc