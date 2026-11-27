#include "gtest/gtest.h"

#include <memory>
#include <string>
#include <vector>

#include "future-config/configuration.h"
#include "future-config/loading.h"
#include "future-config/resources.h"
#include "./common.h"

namespace fc {

namespace {

// config class for the binding test master config
struct Binding_test_config {
	int root_port;
	int network_port;
	int network_timeout;
	bool network_secure;

	explicit Binding_test_config(const Config_object& config_object):
		root_port(config_object.get<int>("port")),
		network_port(network(config_object).get<int>("port")),
		network_timeout(network(config_object).get<int>("timeout")),
		network_secure(network(config_object).get<bool>("secure"))
	{};

private:
	static const Config_object& network(const Config_object& config_object) {
		return config_object.get<Config_object&>("server").get<Config_object&>("network");
	}
};

Config_definitions get_master_config_definitions() {
	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(get_resource_path("test/binding/master.yaml")));
	return config_definitions;
}

const Config_object& get_network(const Config_object& config) {
	return config.get<Config_object&>("server").get<Config_object&>("network");
}

const Config_object hierarchy_config(parse_yaml("{key: 1, object: {inner_object: {inner_key: 2}, scalar: 3}}"));

} // namespace


// bind_config

TEST(Binding, bind_config_to_root) {
	const Config_object config(parse_yaml("{key: value}"));

	const auto bound_config = bind_config(config, "");

	compare_config_objects(config, bound_config);
}

TEST(Binding, bind_config_to_object) {
	const Config_object config(parse_yaml("{key: value}"));

	const auto bound_config = bind_config(config, "object");

	const Config_object expected(parse_yaml("{object: {key: value}}"));
	compare_config_objects(expected, bound_config);
}

TEST(Binding, bind_config_to_nested_object) {
	const Config_object config(parse_yaml("{key: value, inner: {inner_key: inner_value}}"));

	const auto bound_config = bind_config(config, "object.nested.deep");

	const Config_object expected(parse_yaml(
		"{object: {nested: {deep: {key: value, inner: {inner_key: inner_value}}}}}"
	));
	compare_config_objects(expected, bound_config);
}


// get_bound_object

TEST(Binding, get_bound_object_root) {
	const auto& bound_object = get_bound_object(hierarchy_config, "", "test source");

	EXPECT_EQ(&hierarchy_config, &bound_object);
}

TEST(Binding, get_bound_object_nested) {
	const auto& bound_object = get_bound_object(hierarchy_config, "object.inner_object", "test source");

	const Config_object expected(parse_yaml("{inner_key: 2}"));
	compare_config_objects(expected, bound_object);
}

TEST(Binding, get_bound_object_missing_path_throws) {
	EXPECT_THROW(get_bound_object(hierarchy_config, "missing", "test source"), std::runtime_error);
	EXPECT_THROW(get_bound_object(hierarchy_config, "object.missing", "test source"), std::runtime_error);
	// the path is not searched in the hierarchy, it has to start in the root object
	EXPECT_THROW(get_bound_object(hierarchy_config, "inner_object", "test source"), std::runtime_error);
}

TEST(Binding, get_bound_object_scalar_in_path_throws) {
	EXPECT_THROW(get_bound_object(hierarchy_config, "key", "test source"), std::runtime_error);
	EXPECT_THROW(get_bound_object(hierarchy_config, "object.scalar", "test source"), std::runtime_error);
	EXPECT_THROW(get_bound_object(hierarchy_config, "object.scalar.deeper", "test source"), std::runtime_error);
}

TEST(Binding, main_config_cannot_be_bound) {
	const auto master_config_path = get_resource_path("test/binding/master.yaml");

	EXPECT_THROW(Config_definition(Config_type::MAIN, master_config_path, "server"), std::runtime_error);
}


// local config files

TEST(Binding, local_config_bound_to_object) {
	auto config_definitions = get_master_config_definitions();
	config_definitions.add(std::make_unique<Config_definition>(
		Config_type::LOCAL,
		get_resource_path("test/binding/network_local.yaml"),
		"server.network"
	));

	const auto config = load_config(config_definitions.config_definitions);

	// only the bound object is changed
	const auto& network = get_network(config);
	EXPECT_EQ(network.get<int>("timeout"), 30);
	EXPECT_EQ(network.get<int>("port"), 80);
	EXPECT_EQ(config.get<int>("port"), 1);
	EXPECT_EQ(config.get<Config_object&>("server").get<int>("port"), 8080);
	EXPECT_FALSE(config.contains("timeout"));

	// variables in the bound config are resolved from the root object
	EXPECT_EQ(network.get<std::string>("label"), "app-local");
}

TEST(Binding, local_config_bound_to_root_by_default) {
	auto config_definitions = get_master_config_definitions();
	config_definitions.add(std::make_unique<Config_definition>(
		Config_type::LOCAL,
		get_resource_path("test/binding/network_local.yaml")
	));

	const auto config = load_config(config_definitions.config_definitions);

	// the content of the local config is placed to the root object, the network object is unchanged
	EXPECT_EQ(config.get<int>("timeout"), 30);
	EXPECT_EQ(get_network(config).get<int>("timeout"), 10);
}

TEST(Binding, local_config_bound_to_missing_object_throws) {
	const auto local_config_path = get_resource_path("test/binding/network_local.yaml");

	for(const std::string bind_path: {"missing", "server.missing", "server.network.timeout", "network"}) {
		auto config_definitions = get_master_config_definitions();
		config_definitions.add(std::make_unique<Config_definition>(Config_type::LOCAL, local_config_path, bind_path));

		EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error) << bind_path;
	}
}

TEST(Binding, local_config_bound_before_master_config_throws) {
	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(
		Config_type::LOCAL,
		get_resource_path("test/binding/network_local.yaml"),
		"server.network"
	));

	EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error);
}

TEST(Binding, load_options_local_config_bind_path) {
	const auto config = load<Binding_test_config>({
		.config_definitions = get_master_config_definitions(),
		.local_config_path = get_resource_path("test/binding/network_local.yaml"),
		.local_config_bind_path = "server.network"
	});

	EXPECT_EQ(config.network_timeout, 30);
	EXPECT_EQ(config.network_port, 80);
	EXPECT_EQ(config.root_port, 1);
}


// command line

TEST(Binding, command_line_bound_to_object) {
	const char* argv[] = {"program_name", "--port", "5", "--secure"};
	auto config_definitions = get_master_config_definitions();
	config_definitions.add(std::make_unique<Command_line_config_definition>(
		4,
		argv,
		std::vector<std::string>{},
		"server.network"
	));

	const auto config = load_config(config_definitions.config_definitions);

	// the port key is only unique in the bound object
	const auto& network = get_network(config);
	EXPECT_EQ(network.get<int>("port"), 5);
	EXPECT_TRUE(network.get<bool>("secure"));
	EXPECT_EQ(config.get<int>("port"), 1);
	EXPECT_EQ(config.get<Config_object&>("server").get<int>("port"), 8080);
}

TEST(Binding, command_line_ambiguous_key_in_bound_object_throws) {
	const char* argv[] = {"program_name", "--port", "5"};

	// port is present multiple times in both the root object and the server object
	for(const std::string bind_path: {"", "server"}) {
		auto config_definitions = get_master_config_definitions();
		config_definitions.add(std::make_unique<Command_line_config_definition>(
			3,
			argv,
			std::vector<std::string>{},
			bind_path
		));

		EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error) << bind_path;
	}
}

TEST(Binding, command_line_key_outside_bound_object_throws) {
	// name is a key in the root object, but not in the bound object
	const char* argv[] = {"program_name", "--name", "other"};
	auto config_definitions = get_master_config_definitions();
	config_definitions.add(std::make_unique<Command_line_config_definition>(
		3,
		argv,
		std::vector<std::string>{},
		"server.network"
	));

	EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error);
}

TEST(Binding, command_line_allowed_arguments_in_bound_object) {
	const char* argv[] = {"program_name", "--name", "other", "--timeout", "20"};
	auto config_definitions = get_master_config_definitions();
	config_definitions.add(std::make_unique<Command_line_config_definition>(
		5,
		argv,
		std::vector<std::string>{"name"},
		"server.network"
	));

	const auto config = load_config(config_definitions.config_definitions);

	EXPECT_EQ(get_network(config).get<int>("timeout"), 20);
	EXPECT_EQ(config.get<std::string>("name"), "app");
}

TEST(Binding, command_line_bound_to_missing_object_throws) {
	const char* argv[] = {"program_name", "--timeout", "20"};
	auto config_definitions = get_master_config_definitions();
	config_definitions.add(std::make_unique<Command_line_config_definition>(
		3,
		argv,
		std::vector<std::string>{},
		"server.missing"
	));

	EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error);
}

TEST(Binding, load_options_command_line_bind_path) {
	const char* argv[] = {"program_name", "--port", "5", "--secure"};

	const auto config = load<Binding_test_config>({
		.config_definitions = get_master_config_definitions(),
		.command_line_arguments = std::make_pair(4, argv),
		.command_line_bind_path = "server.network"
	});

	EXPECT_EQ(config.network_port, 5);
	EXPECT_TRUE(config.network_secure);
	EXPECT_EQ(config.root_port, 1);
}

TEST(Binding, local_config_and_command_line_bound_to_different_objects) {
	const char* argv[] = {"program_name", "--name", "other"};

	const auto config = load<Binding_test_config>({
		.config_definitions = get_master_config_definitions(),
		.local_config_path = get_resource_path("test/binding/network_local.yaml"),
		.local_config_bind_path = "server.network",
		.command_line_arguments = std::make_pair(3, argv)
	});

	EXPECT_EQ(config.network_timeout, 30);
}


// dependency configs

TEST(Binding, dependency_config_bound_to_key_in_main_config) {
	std::vector<std::unique_ptr<Config_definition>> config_definitions;
	config_definitions.emplace_back(std::make_unique<Dependency_config_definition>(
		get_resource_path("test/binding/dependency.yaml"),
		"parent_config",
		"dependency.h"
	));
	config_definitions.emplace_back(std::make_unique<Config_definition>(
		get_resource_path("test/binding/main_with_dependency.yaml")
	));

	const auto config = load_config_for_builder(config_definitions);

	// the dependency config is placed to the bound object, where it is merged with the main config
	const Config_object expected(parse_yaml(
		"{main_var: 1, parent_config: {var: dependency_var, extra: main_value}}"
	));
	compare_config_objects(expected, config);
}

TEST(Binding, dependency_config_bound_to_nested_object) {
	std::vector<std::unique_ptr<Config_definition>> config_definitions;
	config_definitions.emplace_back(std::make_unique<Dependency_config_definition>(
		get_resource_path("test/binding/dependency.yaml"),
		"libraries.parent_config",
		"dependency.h"
	));

	const auto config = load_config_for_builder(config_definitions);

	const Config_object expected(parse_yaml("{libraries: {parent_config: {var: dependency_var}}}"));
	compare_config_objects(expected, config);
}

} // namespace fc
