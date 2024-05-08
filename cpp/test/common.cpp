//
// Created by david on 2024-05-08.
//

#include "common.h"

void compare_config_objects(const Config_object& expected, const Config_object& actual) {
	ASSERT_EQ(expected.size(), actual.size());

	for(const auto&[key, value]: expected) {
		ASSERT_TRUE(actual.contains(key));
		const auto& actual_value = actual[key];

		// scalar or array values: we can compare directly
		if(std::holds_alternative<std::string>(value)) {
			ASSERT_TRUE(std::holds_alternative<std::string>(actual_value));
			ASSERT_EQ(std::get<std::string>(value), std::get<std::string>(actual_value));
		}
		else if(std::holds_alternative<int>(value)) {
			ASSERT_TRUE(std::holds_alternative<int>(actual_value));
			ASSERT_EQ(std::get<int>(value), std::get<int>(actual_value));
		}
		else if(std::holds_alternative<double>(value)) {
			ASSERT_TRUE(std::holds_alternative<double>(actual_value));
			ASSERT_EQ(std::get<double>(value), std::get<double>(actual_value));
		}
		else if(std::holds_alternative<bool>(value)) {
			ASSERT_TRUE(std::holds_alternative<bool>(actual_value));
			ASSERT_EQ(std::get<bool>(value), std::get<bool>(actual_value));
		}
		else if(std::holds_alternative<std::vector<std::string>>(value)) {
			ASSERT_TRUE(std::holds_alternative<std::vector<std::string>>(actual_value));
			ASSERT_EQ(std::get<std::vector<std::string>>(value), std::get<std::vector<std::string>>(actual_value));
		}
		else if(std::holds_alternative<std::vector<int>>(value)) {
			ASSERT_TRUE(std::holds_alternative<std::vector<int>>(actual_value));
			ASSERT_EQ(std::get<std::vector<int>>(value), std::get<std::vector<int>>(actual_value));
		}
		else if(std::holds_alternative<std::vector<double>>(value)) {
			ASSERT_TRUE(std::holds_alternative<std::vector<double>>(actual_value));
			ASSERT_EQ(std::get<std::vector<double>>(value), std::get<std::vector<double>>(actual_value));
		}
		else if(std::holds_alternative<std::vector<bool>>(value)) {
			ASSERT_TRUE(std::holds_alternative<std::vector<bool>>(actual_value));
			ASSERT_EQ(std::get<std::vector<bool>>(value), std::get<std::vector<bool>>(actual_value));
		}

		// object values: we need to compare recursively
		if(std::holds_alternative<Config_object>(value)) {
			ASSERT_TRUE(std::holds_alternative<Config_object>(actual_value));
			compare_config_objects(std::get<Config_object>(value), std::get<Config_object>(actual_value));
		}
	}


}
