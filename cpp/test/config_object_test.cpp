//
// Created by Fido on 2025-02-26.
//

#include "gtest/gtest.h"

#include "future-config/Config_object.h"
#include "./common.h"


TEST(Config_object, object_in_object) {
	// the document starts right with the flow mapping: tabs are not allowed as an indentation in YAML
	fc::Config_object config = fc::parse_yaml(R"({
			test: {
				test2: 3
			}
		}
	)");

	auto& test = config.get<fc::Config_object&>("test");
	auto test2 = test.get<int>("test2");

	ASSERT_EQ(test2, 3);
}
