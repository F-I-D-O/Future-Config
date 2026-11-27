//
// Created by david on 2024-05-08.
//

#include "common.h"

#include "Parser.h"

namespace fc {

Config_object parse_yaml(const std::string& yaml_content) {
	return Parser().parse(yaml_content);
}

void compare_config_objects(const Config_object& expected, const Config_object& actual) {
	ASSERT_EQ(expected.size(), actual.size());

	for(const auto&[key, value]: expected) {
		ASSERT_TRUE(actual.contains(key));
		const auto& actual_value = actual[key];

		// scalar or array values: we can compare directly, both the types and the values have to match
		if(std::holds_alternative<Config_scalar>(value)) {
			ASSERT_TRUE(std::holds_alternative<Config_scalar>(actual_value)) << key;
			const auto& expected_scalar = std::get<Config_scalar>(value);
			const auto& actual_scalar = std::get<Config_scalar>(actual_value);
			ASSERT_EQ(get_type_name(expected_scalar), get_type_name(actual_scalar)) << key;
			ASSERT_EQ(to_string(expected_scalar), to_string(actual_scalar)) << key;
		}
		else if(std::holds_alternative<std::vector<Config_scalar>>(value)) {
			ASSERT_TRUE(std::holds_alternative<std::vector<Config_scalar>>(actual_value)) << key;
			ASSERT_EQ(std::get<std::vector<Config_scalar>>(value), std::get<std::vector<Config_scalar>>(actual_value))
				<< key;
		}

		// object values: we need to compare recursively
		if(std::holds_alternative<config_object_property_value>(value)) {
			ASSERT_TRUE(std::holds_alternative<config_object_property_value>(actual_value));
			compare_config_objects(*std::get<config_object_property_value>(value), *std::get<config_object_property_value>(actual_value));
		}
	}


}

CommandResult run_command(const std::string& command) {
#ifdef _WIN32
	const char* popen_cmd = "_popen";
	FILE* pipe = _popen((command + " 2>&1").c_str(), "r");
#else
	FILE* pipe = popen((command + " 2>&1").c_str(), "r");
#endif

	if (!pipe) {
		throw std::runtime_error("popen() failed");
	}

	std::array<char, 256> buffer{};
	std::string output;

	while (fgets(buffer.data(), buffer.size(), pipe)) {
		output += buffer.data();
	}

#ifdef _WIN32
	int rc = _pclose(pipe);
	int exit_code = rc;  // already the exit code
#else
	int rc = pclose(pipe);
	int exit_code = -1;
	if (WIFEXITED(rc)) {
		exit_code = WEXITSTATUS(rc);
	}
#endif

	return {exit_code, output};
}
} // namespace fc
