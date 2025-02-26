//
// Created by Fido on 2025-02-26.
//

#include "gtest/gtest.h"

#include "future-config/Config_object.h"


TEST(Config_object, object_in_object) {
	YAML::Node yaml_config = YAML::Load(R"(
		{
			test: {
				test2: {

					test3: 3
				}
			}
		}
	)");

	fc::Config_object config(yaml_config);

	auto& test2 = config.get<fc::Config_object&>("test2");
	auto test3 = test2.get<int>("test3");

	ASSERT_EQ(test3, 3);
}
