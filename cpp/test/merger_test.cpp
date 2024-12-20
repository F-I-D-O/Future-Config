\
#include "gtest/gtest.h"

#include "Merger.h"
#include "./common.h"

namespace fc {

static_assert(std::is_default_constructible_v<std::allocator<Config_object>>);

TEST(Merger, single_config) {
	Merger merger;
	YAML::Node config = YAML::Load(
		R"({
		key1: value1,
		key2: value2,
		key3: {key4: value4}
	})"
	);

	std::vector<Config_object> configs;
	configs.emplace_back(config);
	auto merged_config = merger.merge(configs);

	compare_config_objects(configs[0], merged_config);
}

TEST(Merger, two_configs) {
	Merger merger;
	YAML::Node config1 = YAML::Load(
		R"({
		key1: value1,
		key2: value2,
		key3: {key4: value4}
	})"
	);

	YAML::Node config2 = YAML::Load(
		R"({
		key1: value1_new,
		key3: {key4: value4_new}
	})"
	);

	YAML::Node expected_merged_config_yaml = YAML::Load(
		R"({
		key1: value1_new,
		key2: value2,
		key3: {key4: value4_new}
	})"
	);
	auto expected_merged_config = Config_object(expected_merged_config_yaml);

	std::vector<Config_object> configs;
	configs.emplace_back(config1);
	configs.emplace_back(config2);
	auto merged_config = merger.merge(configs);

	compare_config_objects(expected_merged_config, merged_config);
}

} // namespace fc
