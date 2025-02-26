//
// Created by Fido on 2025-02-26.
//

#include "gtest/gtest.h"

#include "future-config/Config_object.h"


TEST(Config_object, object_in_object) {
	YAML::Node yaml_config = YAML::Load(R"(
		{
			test: {
				test2: 3
			}
		}
	)");

	fc::Config_object config(yaml_config);

	auto& test = config.get<fc::Config_object&>("test");
	auto test2 = test.get<int>("test2");

	ASSERT_EQ(test2, 3);
}
