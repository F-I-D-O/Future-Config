#include "gtest/gtest.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "future-config/loading.h"
#include "Parser.h"
#include "./common.h"

namespace fc {

namespace {

// type name and text representation of the value of the `key` property in a YAML document with a single property
std::string get_typed_value(const std::string& yaml_value) {
	const auto config = parse_yaml("key: " + yaml_value + "\n");
	const auto& scalar = std::get<Config_scalar>(config["key"]);
	return get_type_name(scalar) + " " + to_string(scalar);
}

// type name and text representation of the scalar property
std::string get_typed_value_of(const Config_object& config, const std::string& key) {
	const auto& scalar = std::get<Config_scalar>(config[key]);
	return get_type_name(scalar) + " " + to_string(scalar);
}

std::string get_typed_scalar(const std::string& text) {
	const auto scalar = Parser().parse_scalar(text);
	return get_type_name(scalar) + " " + to_string(scalar);
}

} // namespace


// types of the values in YAML

TEST(Typing, integers) {
	EXPECT_EQ(get_typed_value("8080"), "int 8080");
	EXPECT_EQ(get_typed_value("-3"), "int -3");
	EXPECT_EQ(get_typed_value("+3"), "int 3");
	EXPECT_EQ(get_typed_value("0x1F"), "int 31");
	EXPECT_EQ(get_typed_value("0o17"), "int 15");
	EXPECT_EQ(get_typed_value("9223372036854775807"), "int 9223372036854775807");
}

TEST(Typing, floats) {
	EXPECT_EQ(get_typed_value("1.5"), "float 1.5");
	EXPECT_EQ(get_typed_value("-0.25"), "float -0.25");
	EXPECT_EQ(get_typed_value(".5"), "float 0.5");
	EXPECT_EQ(get_typed_value("1."), "float 1.0");
	EXPECT_EQ(get_typed_value("2.0"), "float 2.0");
	EXPECT_EQ(get_typed_value("1e5"), "float 1e+05");
	// the text representation of a float is read back as a float
	EXPECT_EQ(get_typed_value("1e+05"), "float 1e+05");
	EXPECT_EQ(get_typed_value(".inf"), "float .inf");
	EXPECT_EQ(get_typed_value("-.inf"), "float -.inf");
	EXPECT_EQ(get_typed_value(".nan"), "float .nan");
}

TEST(Typing, booleans) {
	EXPECT_EQ(get_typed_value("true"), "bool true");
	EXPECT_EQ(get_typed_value("True"), "bool true");
	EXPECT_EQ(get_typed_value("TRUE"), "bool true");
	EXPECT_EQ(get_typed_value("false"), "bool false");
	EXPECT_EQ(get_typed_value("False"), "bool false");
}

TEST(Typing, strings) {
	EXPECT_EQ(get_typed_value("hello"), "string hello");
	EXPECT_EQ(get_typed_value("hello world"), "string hello world");
	EXPECT_EQ(get_typed_value("\"\""), "string ");

	// values that look like other types in other YAML versions or other languages
	EXPECT_EQ(get_typed_value("yes"), "string yes");
	EXPECT_EQ(get_typed_value("on"), "string on");
	EXPECT_EQ(get_typed_value("010"), "string 010");
	EXPECT_EQ(get_typed_value("1_000"), "string 1_000");
	EXPECT_EQ(get_typed_value("2026-11-27"), "string 2026-11-27");

	// values with variables
	EXPECT_EQ(get_typed_value("${port}"), "string ${port}");
	EXPECT_EQ(get_typed_value("\"${name}-x\""), "string ${name}-x");
}

TEST(Typing, quoted_values_are_strings) {
	EXPECT_EQ(get_typed_value("\"8080\""), "string 8080");
	EXPECT_EQ(get_typed_value("'8080'"), "string 8080");
	EXPECT_EQ(get_typed_value("\"true\""), "string true");
	EXPECT_EQ(get_typed_value("'1.5'"), "string 1.5");
}

TEST(Typing, tagged_values) {
	EXPECT_EQ(get_typed_value("!!str 8080"), "string 8080");
	EXPECT_EQ(get_typed_value("!!str true"), "string true");
	EXPECT_EQ(get_typed_value("!!int \"8080\""), "int 8080");
	EXPECT_EQ(get_typed_value("!!float 3"), "float 3.0");
}

TEST(Typing, arrays) {
	const auto config = parse_yaml("{ints: [1, 2], floats: [1.5, 2.5], strings: [a, b], bools: [true, false]}");

	EXPECT_EQ(config.get_array<int>("ints"), (std::vector<int>{1, 2}));
	EXPECT_EQ(config.get_array<double>("floats"), (std::vector<double>{1.5, 2.5}));
	EXPECT_EQ(config.get_array<std::string>("strings"), (std::vector<std::string>{"a", "b"}));
	EXPECT_EQ(config.get_array<bool>("bools"), (std::vector<bool>{true, false}));
}

TEST(Typing, unsupported_values_throw) {
	// null values
	EXPECT_THROW(parse_yaml("key: ~\n"), std::runtime_error);
	EXPECT_THROW(parse_yaml("key: null\n"), std::runtime_error);
	EXPECT_THROW(parse_yaml("key:\n"), std::runtime_error);
	EXPECT_THROW(parse_yaml("object:\n  key: ~\n"), std::runtime_error);
	EXPECT_THROW(parse_yaml("key: [1, ~]\n"), std::runtime_error);

	// empty arrays and arrays with other than scalar values
	EXPECT_THROW(parse_yaml("key: []\n"), std::runtime_error);
	EXPECT_THROW(parse_yaml("key: [[1, 2]]\n"), std::runtime_error);
	EXPECT_THROW(parse_yaml("key: [{a: 1}]\n"), std::runtime_error);

	// integers that do not fit to 64 bits
	EXPECT_THROW(parse_yaml("key: 9223372036854775808\n"), std::runtime_error);
}

TEST(Typing, error_contains_property_path) {
	try {
		parse_yaml("object:\n  inner:\n    key: ~\n");
		FAIL() << "exception expected";
	} catch(const std::runtime_error& e) {
		EXPECT_NE(std::string(e.what()).find("object.inner.key"), std::string::npos) << e.what();
	}
}


// paths

TEST(Typing, paths) {
	// absolute paths
	EXPECT_EQ(get_typed_value("/opt/app"), "path /opt/app");
	EXPECT_EQ(get_typed_value("/"), "path /");
	EXPECT_EQ(get_typed_value("C:/data/in.csv"), "path C:/data/in.csv");
	EXPECT_EQ(get_typed_value("C:\\data\\in.csv"), "path C:\\data\\in.csv");
	EXPECT_EQ(get_typed_value("\\\\server\\share"), "path \\\\server\\share");

	// paths relative to the current or parent directory
	EXPECT_EQ(get_typed_value("./results"), "path ./results");
	EXPECT_EQ(get_typed_value("../shared/data.csv"), "path ../shared/data.csv");
	EXPECT_EQ(get_typed_value(".\\results"), "path .\\results");
	EXPECT_EQ(get_typed_value("."), "path .");
	EXPECT_EQ(get_typed_value(".."), "path ..");

	// paths relative to the home directory
	EXPECT_EQ(get_typed_value("~/data"), "path ~/data");

	// paths with spaces
	EXPECT_EQ(get_typed_value("./my results/out.csv"), "path ./my results/out.csv");
}

TEST(Typing, values_that_are_not_paths) {
	EXPECT_EQ(get_typed_value("logs/app.log"), "string logs/app.log");
	EXPECT_EQ(get_typed_value("text/plain"), "string text/plain");
	EXPECT_EQ(get_typed_value("1/2"), "string 1/2");
	EXPECT_EQ(get_typed_value("11/27/2026"), "string 11/27/2026");
	EXPECT_EQ(get_typed_value("http://example.com/a"), "string http://example.com/a");
	EXPECT_EQ(get_typed_value("config.yaml"), "string config.yaml");
	EXPECT_EQ(get_typed_value("example.com"), "string example.com");
	EXPECT_EQ(get_typed_value(".hidden"), "string .hidden");
	EXPECT_EQ(get_typed_value("..."), "string ...");
	EXPECT_EQ(get_typed_value("a:b"), "string a:b");
	EXPECT_EQ(get_typed_value("\"C:\""), "string C:");
	EXPECT_EQ(get_typed_value("~user"), "string ~user");
}

TEST(Typing, path_tags) {
	// the !path tag makes a path from any string
	EXPECT_EQ(get_typed_value("!path logs/app.log"), "path logs/app.log");
	EXPECT_EQ(get_typed_value("!path results"), "path results");

	// the !!str tag keeps a string even if it has a form of a path
	EXPECT_EQ(get_typed_value("!!str ./results"), "string ./results");
	EXPECT_EQ(get_typed_value("!!str /opt/app"), "string /opt/app");
}

TEST(Typing, quoted_paths) {
	// the quoted and plain strings cannot be distinguished, only the !!str tag keeps a string
	EXPECT_EQ(get_typed_value("\"./results\""), "path ./results");
}

TEST(Typing, path_arrays) {
	const auto config = parse_yaml("{files: [./a.txt, /opt/b.txt]}");

	EXPECT_EQ(
		config.get_array<std::filesystem::path>("files"),
		(std::vector<std::filesystem::path>{"./a.txt", "/opt/b.txt"})
	);
}

TEST(Typing, get_path) {
	const auto config = parse_yaml("{out_dir: ./results, name: text}");

	EXPECT_EQ(config.get<std::filesystem::path>("out_dir"), std::filesystem::path("./results"));
	EXPECT_THROW(config.get<std::string>("out_dir"), std::runtime_error);
	EXPECT_THROW(config.get<std::filesystem::path>("name"), std::runtime_error);
}

TEST(Typing, convert_string_to_path) {
	const auto type_reference = parse_yaml("{out_dir: /opt/out, files: [/opt/a], name: text}");
	auto config = parse_yaml("{out_dir: results, files: [a.txt, sub/b.txt], name: ./relative}");

	convert_types(config, type_reference, "test source");

	// strings given for the path properties are paths, even without a form of a path
	EXPECT_EQ(get_typed_value_of(config, "out_dir"), "path results");
	EXPECT_EQ(
		config.get_array<std::filesystem::path>("files"),
		(std::vector<std::filesystem::path>{"a.txt", "sub/b.txt"})
	);
	// paths given for the string properties are strings
	EXPECT_EQ(get_typed_value_of(config, "name"), "string ./relative");
}

TEST(Typing, convert_other_types_to_path_throws) {
	const auto type_reference = parse_yaml("{out_dir: /opt/out}");

	for(const std::string value: {"5", "1.5", "true"}) {
		auto config = parse_yaml("{out_dir: " + value + "}");
		EXPECT_THROW(convert_types(config, type_reference, "test source"), std::runtime_error) << value;
	}
}

TEST(Typing, variables_in_paths) {
	auto config = parse_yaml(
		"{name: app, data_dir: /opt/data, in_path: \"./in/${name}\", from_variable: \"${data_dir}/out\", "
		"tagged: !path \"${data_dir}/out\", text: \"${name}/out\"}"
	);

	process_variables(config, nullptr);

	// a path with a variable stays a path
	EXPECT_EQ(get_typed_value_of(config, "in_path"), "path ./in/app");
	EXPECT_EQ(get_typed_value_of(config, "tagged"), "path /opt/data/out");
	// the type of a string with a variable is determined from the resolved value
	EXPECT_EQ(get_typed_value_of(config, "from_variable"), "path /opt/data/out");
	EXPECT_EQ(get_typed_value_of(config, "text"), "string app/out");
}

TEST(Typing, path_starting_with_variable_has_to_be_absolute) {
	// the variable does not provide an absolute path
	auto config = parse_yaml("{name: app, tagged: !path \"${name}/out\"}");
	EXPECT_THROW(process_variables(config, nullptr), std::runtime_error);

	config = parse_yaml("{relative_dir: !path data, out: \"${relative_dir}/out\"}");
	const auto type_reference = parse_yaml("{relative_dir: /opt/data, out: /opt/out}");
	EXPECT_THROW(process_variables(config, &type_reference), std::runtime_error);

	// paths relative to the home directory are accepted
	config = parse_yaml("{home_dir: ~/data, out: \"${home_dir}/out\"}");
	process_variables(config, nullptr);
	EXPECT_EQ(get_typed_value_of(config, "out"), "path ~/data/out");
}

TEST(Typing, make_paths_absolute) {
	auto config = parse_yaml(
		"{current: ./results, parent: ../shared/data.csv, bare: !path logs/app.log, dot: ., "
		"absolute: /opt/app, windows: C:/data, home: ~/data, variable: !path \"${dir}/out\", "
		"inner_variable: \"./out/${name}\", array: [./a.txt, /opt/b.txt], object: {nested: ./nested}, "
		"text: !!str ./text, number: 5}"
	);

	make_paths_absolute(config, "/base/dir");

	// relative paths
	EXPECT_EQ(get_typed_value_of(config, "current"), "path /base/dir/results");
	EXPECT_EQ(get_typed_value_of(config, "parent"), "path /base/shared/data.csv");
	EXPECT_EQ(get_typed_value_of(config, "bare"), "path /base/dir/logs/app.log");
	EXPECT_EQ(get_typed_value_of(config, "dot"), "path /base/dir/");
	EXPECT_EQ(get_typed_value_of(config, "inner_variable"), "path /base/dir/out/${name}");
	EXPECT_EQ(get_typed_value_of(config.get<Config_object&>("object"), "nested"), "path /base/dir/nested");
	EXPECT_EQ(
		config.get_array<std::filesystem::path>("array"),
		(std::vector<std::filesystem::path>{"/base/dir/a.txt", "/opt/b.txt"})
	);

	// paths that are not changed
	EXPECT_EQ(get_typed_value_of(config, "absolute"), "path /opt/app");
	EXPECT_EQ(get_typed_value_of(config, "windows"), "path C:/data");
	EXPECT_EQ(get_typed_value_of(config, "home"), "path ~/data");
	EXPECT_EQ(get_typed_value_of(config, "variable"), "path ${dir}/out");

	// other values
	EXPECT_EQ(get_typed_value_of(config, "text"), "string ./text");
	EXPECT_EQ(get_typed_value_of(config, "number"), "int 5");
}


// types of the values given as text

TEST(Typing, parse_scalar) {
	EXPECT_EQ(get_typed_scalar("5"), "int 5");
	EXPECT_EQ(get_typed_scalar("-5"), "int -5");
	EXPECT_EQ(get_typed_scalar("1.5"), "float 1.5");
	EXPECT_EQ(get_typed_scalar("true"), "bool true");
	EXPECT_EQ(get_typed_scalar("hello"), "string hello");
	EXPECT_EQ(get_typed_scalar(""), "string ");
	EXPECT_EQ(get_typed_scalar("./results"), "path ./results");
	EXPECT_EQ(get_typed_scalar("/opt/my app"), "path /opt/my app");
	EXPECT_EQ(get_typed_scalar("logs/app.log"), "string logs/app.log");
}

TEST(Typing, parse_scalar_text_with_yaml_syntax) {
	// texts that would not be scalars, or would be different scalars, if they were parsed as YAML documents
	EXPECT_EQ(get_typed_scalar("hello world"), "string hello world");
	EXPECT_EQ(get_typed_scalar("5 #comment"), "string 5 #comment");
	EXPECT_EQ(get_typed_scalar("a: b"), "string a: b");
	EXPECT_EQ(get_typed_scalar("a:b"), "string a:b");
	EXPECT_EQ(get_typed_scalar("[1,2]"), "string [1,2]");
	EXPECT_EQ(get_typed_scalar("{a}"), "string {a}");
	EXPECT_EQ(get_typed_scalar("[1"), "string [1");
	// a path relative to the home directory
	EXPECT_EQ(get_typed_scalar("~"), "path ~");
	EXPECT_EQ(get_typed_scalar("null"), "string null");
	EXPECT_EQ(get_typed_scalar("*alias"), "string *alias");
	EXPECT_EQ(get_typed_scalar("&anchor"), "string &anchor");
	EXPECT_EQ(get_typed_scalar("!tag"), "string !tag");
	EXPECT_EQ(get_typed_scalar("#5"), "string #5");
	EXPECT_EQ(get_typed_scalar("\"5\""), "string \"5\"");
	EXPECT_EQ(get_typed_scalar("${var}"), "string ${var}");
	EXPECT_EQ(get_typed_scalar("--"), "string --");
	EXPECT_EQ(get_typed_scalar("-"), "string -");
	EXPECT_EQ(get_typed_scalar("%x"), "string %x");
	EXPECT_EQ(get_typed_scalar("|"), "string |");
	EXPECT_EQ(get_typed_scalar(">"), "string >");
	EXPECT_EQ(get_typed_scalar("@x"), "string @x");
}


// reading the values

TEST(Typing, get_matching_types) {
	const auto config = parse_yaml("{integer: 42, float: 1.5, boolean: true, string: text}");

	EXPECT_EQ(config.get<int>("integer"), 42);
	EXPECT_EQ(config.get<std::int64_t>("integer"), 42);
	EXPECT_EQ(config.get<unsigned>("integer"), 42u);
	EXPECT_EQ(config.get<short>("integer"), 42);
	EXPECT_EQ(config.get<double>("float"), 1.5);
	EXPECT_EQ(config.get<float>("float"), 1.5f);
	EXPECT_EQ(config.get<bool>("boolean"), true);
	EXPECT_EQ(config.get<std::string>("string"), "text");

	// integers can be read as floating point numbers
	EXPECT_EQ(config.get<double>("integer"), 42.0);
}

TEST(Typing, get_different_type_throws) {
	const auto config = parse_yaml(
		"{integer: 42, float: 1.5, boolean: true, string: text, number_string: \"42\", array: [1, 2], object: {a: 1}}"
	);

	EXPECT_THROW(config.get<int>("float"), std::runtime_error);
	EXPECT_THROW(config.get<int>("string"), std::runtime_error);
	EXPECT_THROW(config.get<int>("boolean"), std::runtime_error);
	EXPECT_THROW(config.get<bool>("integer"), std::runtime_error);
	EXPECT_THROW(config.get<bool>("string"), std::runtime_error);
	EXPECT_THROW(config.get<double>("string"), std::runtime_error);
	EXPECT_THROW(config.get<std::string>("integer"), std::runtime_error);
	EXPECT_THROW(config.get<std::string>("boolean"), std::runtime_error);
	EXPECT_THROW(config.get<std::filesystem::path>("string"), std::runtime_error);

	// no conversion from text
	EXPECT_THROW(config.get<int>("number_string"), std::runtime_error);

	// not scalars
	EXPECT_THROW(config.get<int>("array"), std::runtime_error);
	EXPECT_THROW(config.get<int>("object"), std::runtime_error);
	EXPECT_THROW(config.get_array<int>("integer"), std::runtime_error);
	EXPECT_THROW(config.get_array<std::string>("array"), std::runtime_error);
}

TEST(Typing, get_error_contains_key_and_types) {
	const auto config = parse_yaml("{port: text}");
	try {
		config.get<int>("port");
		FAIL() << "exception expected";
	} catch(const std::runtime_error& e) {
		const std::string message = e.what();
		EXPECT_NE(message.find("port"), std::string::npos) << message;
		EXPECT_NE(message.find("string"), std::string::npos) << message;
		EXPECT_NE(message.find("int"), std::string::npos) << message;
	}
}

TEST(Typing, get_integer_out_of_range_throws) {
	const auto config = parse_yaml("{big: 3000000000, negative: -1, small: 100}");

	EXPECT_EQ(config.get<std::int64_t>("big"), 3000000000);
	EXPECT_EQ(config.get<unsigned>("big"), 3000000000u);
	EXPECT_THROW(config.get<int>("big"), std::runtime_error);
	EXPECT_THROW(config.get<unsigned>("negative"), std::runtime_error);
	EXPECT_THROW(config.get<std::int8_t>("big"), std::runtime_error);
	EXPECT_EQ(config.get<std::int8_t>("small"), 100);
}

// conversion to the types determined by the master config

namespace {

const std::string master_yaml = R"(
name: app
port: 8080
ratio: 1.5
debug: false
tags: [a, b]
limits: [1, 2]
server:
  host: localhost
  timeout: 10
port_copy: ${port}
url: "http://${server.host}:${port}"
)";

// type name and text representation of the scalar property
std::string get_typed_property(const Config_object& config, const std::string& key) {
	const auto& scalar = std::get<Config_scalar>(config[key]);
	return get_type_name(scalar) + " " + to_string(scalar);
}

// Converts the config to the types of the master config and returns the error message, or an empty string
std::string get_conversion_error(const std::string& yaml) {
	const auto type_reference = get_type_reference(parse_yaml(master_yaml));
	auto config = parse_yaml(yaml);
	try {
		convert_types(config, type_reference, "test source");
	} catch(const std::runtime_error& e) {
		return e.what();
	}
	return "";
}

/**
 * Creates config files in a temporary directory, so that the whole config loading can be tested.
 */
class Typing_loading: public ::testing::Test {
protected:
	fs::path directory;

	void SetUp() override {
		directory = fs::temp_directory_path() / "fconfig_typing_test";
		fs::remove_all(directory);
		fs::create_directories(directory);
	}

	void TearDown() override {
		fs::remove_all(directory);
	}

	[[nodiscard]] fs::path write_config(const std::string& file_name, const std::string& content) const {
		const auto path = directory / file_name;
		std::ofstream(path) << content;
		return path;
	}

	// loads the master config, the local config (if not empty), and the command line arguments (if not empty)
	[[nodiscard]] Config_object load(const std::string& local_yaml, const std::vector<const char*>& arguments = {}) const {
		Config_definitions config_definitions;
		config_definitions.add(std::make_unique<Config_definition>(write_config("master.yaml", master_yaml)));
		if(!local_yaml.empty()) {
			config_definitions.add(std::make_unique<Config_definition>(
				Config_type::LOCAL,
				write_config("local.yaml", local_yaml)
			));
		}
		std::vector<const char*> argv{"program_name"};
		argv.insert(argv.end(), arguments.begin(), arguments.end());
		if(!arguments.empty()) {
			config_definitions.add(std::make_unique<Command_line_config_definition>(
				static_cast<int>(argv.size()),
				argv.data()
			));
		}
		return load_config(config_definitions.config_definitions);
	}

	[[nodiscard]] std::string get_load_error(
		const std::string& local_yaml,
		const std::vector<const char*>& arguments = {}
	) const {
		try {
			load(local_yaml, arguments);
		} catch(const std::runtime_error& e) {
			return e.what();
		}
		return "";
	}
};

} // namespace

TEST(Typing, type_reference_resolves_variables) {
	const auto type_reference = get_type_reference(parse_yaml(master_yaml));

	// the type of a value with variables is determined from the resolved value
	EXPECT_EQ(get_typed_property(type_reference, "port_copy"), "int 8080");
	EXPECT_EQ(get_typed_property(type_reference, "url"), "string http://localhost:8080");
	EXPECT_EQ(get_typed_property(type_reference, "port"), "int 8080");
}

TEST(Typing, convert_same_types) {
	auto config = parse_yaml("{name: other, port: 1, ratio: 2.5, debug: true, tags: [x], limits: [5], server: {timeout: 3}}");

	convert_types(config, get_type_reference(parse_yaml(master_yaml)), "test source");

	const auto expected = parse_yaml(
		"{name: other, port: 1, ratio: 2.5, debug: true, tags: [x], limits: [5], server: {timeout: 3}}"
	);
	compare_config_objects(expected, config);
}

TEST(Typing, convert_any_scalar_to_string) {
	auto config = parse_yaml("{name: 123, tags: [1, true, 2.5, text]}");

	convert_types(config, get_type_reference(parse_yaml(master_yaml)), "test source");

	EXPECT_EQ(get_typed_property(config, "name"), "string 123");
	EXPECT_EQ(config.get_array<std::string>("tags"), (std::vector<std::string>{"1", "true", "2.5", "text"}));
}

TEST(Typing, convert_int_to_float) {
	auto config = parse_yaml("{ratio: 2}");

	convert_types(config, get_type_reference(parse_yaml(master_yaml)), "test source");

	EXPECT_EQ(get_typed_property(config, "ratio"), "float 2.0");
}

TEST(Typing, convert_unknown_property_keeps_type) {
	auto config = parse_yaml("{extra: 5, extra_object: {port: text}, server: {extra: true}}");

	convert_types(config, get_type_reference(parse_yaml(master_yaml)), "test source");

	EXPECT_EQ(get_typed_property(config, "extra"), "int 5");
	EXPECT_EQ(get_typed_property(config.get<Config_object&>("extra_object"), "port"), "string text");
	EXPECT_EQ(get_typed_property(config.get<Config_object&>("server"), "extra"), "bool true");
}

TEST(Typing, convert_value_with_variable_is_skipped) {
	auto config = parse_yaml("{port: \"${other_port}\", debug: \"${flag}\"}");

	convert_types(config, get_type_reference(parse_yaml(master_yaml)), "test source");

	EXPECT_EQ(get_typed_property(config, "port"), "string ${other_port}");
	EXPECT_EQ(get_typed_property(config, "debug"), "string ${flag}");
}

TEST(Typing, convert_wrong_scalar_type_throws) {
	EXPECT_NE(get_conversion_error("{port: abc}"), "");
	EXPECT_NE(get_conversion_error("{port: 1.5}"), "");
	EXPECT_NE(get_conversion_error("{port: true}"), "");
	EXPECT_NE(get_conversion_error("{port: \"8080\"}"), "");
	EXPECT_NE(get_conversion_error("{ratio: abc}"), "");
	EXPECT_NE(get_conversion_error("{ratio: true}"), "");
	EXPECT_NE(get_conversion_error("{debug: 1}"), "");
	EXPECT_NE(get_conversion_error("{debug: yes}"), "");
	EXPECT_NE(get_conversion_error("{server: {timeout: abc}}"), "");
	EXPECT_NE(get_conversion_error("{limits: [1, x]}"), "");
	EXPECT_NE(get_conversion_error("{limits: [1.5]}"), "");

	// the type of the port_copy property is determined from the resolved variable
	EXPECT_NE(get_conversion_error("{port_copy: abc}"), "");
	EXPECT_EQ(get_conversion_error("{port_copy: 5}"), "");
}

TEST(Typing, convert_wrong_structure_throws) {
	EXPECT_NE(get_conversion_error("{server: 5}"), "");
	EXPECT_NE(get_conversion_error("{server: [1, 2]}"), "");
	EXPECT_NE(get_conversion_error("{port: {a: 1}}"), "");
	EXPECT_NE(get_conversion_error("{port: [1, 2]}"), "");
	EXPECT_NE(get_conversion_error("{tags: x}"), "");
	EXPECT_NE(get_conversion_error("{tags: {a: 1}}"), "");
}

TEST(Typing, conversion_error_message) {
	const auto message = get_conversion_error("{server: {timeout: abc}}");

	// the property, the source, the value and both types are mentioned
	EXPECT_NE(message.find("server.timeout"), std::string::npos) << message;
	EXPECT_NE(message.find("test source"), std::string::npos) << message;
	EXPECT_NE(message.find("'abc'"), std::string::npos) << message;
	EXPECT_NE(message.find("string"), std::string::npos) << message;
	EXPECT_NE(message.find("int"), std::string::npos) << message;
}

TEST(Typing, master_array_with_different_types_throws) {
	EXPECT_THROW(check_master_types(parse_yaml("{array: [1, two]}"), "test source"), std::runtime_error);
	EXPECT_THROW(check_master_types(parse_yaml("{array: [1, 2.5]}"), "test source"), std::runtime_error);
	EXPECT_THROW(check_master_types(parse_yaml("{object: {array: [a, true]}}"), "test source"), std::runtime_error);
	EXPECT_NO_THROW(check_master_types(parse_yaml("{array: [1, 2], other: [a, b]}"), "test source"));
}

TEST(Typing, variables_are_replaced_by_text_of_typed_values) {
	auto config = parse_yaml(
		"{integer: 42, float: 1.5, boolean: true, text: \"${integer}-${float}-${boolean}\", copy: \"${float}\"}"
	);

	process_variables(config, nullptr);

	EXPECT_EQ(get_typed_property(config, "text"), "string 42-1.5-true");
	// without a type reference, the type is determined from the resolved value
	EXPECT_EQ(get_typed_property(config, "copy"), "float 1.5");
}


// whole config loading

TEST_F(Typing_loading, master_only) {
	const auto config = load("");

	EXPECT_EQ(get_typed_property(config, "port"), "int 8080");
	EXPECT_EQ(get_typed_property(config, "port_copy"), "int 8080");
	EXPECT_EQ(get_typed_property(config, "url"), "string http://localhost:8080");
	EXPECT_EQ(get_typed_property(config, "ratio"), "float 1.5");
	EXPECT_EQ(get_typed_property(config, "debug"), "bool false");
}

TEST_F(Typing_loading, local_config_values_are_converted) {
	const auto config = load("name: 123\nratio: 2\ntags: [1, true]\nextra: 5\nserver:\n  timeout: 30\n");

	EXPECT_EQ(get_typed_property(config, "name"), "string 123");
	EXPECT_EQ(get_typed_property(config, "ratio"), "float 2.0");
	EXPECT_EQ(config.get_array<std::string>("tags"), (std::vector<std::string>{"1", "true"}));
	EXPECT_EQ(get_typed_property(config, "extra"), "int 5");
	EXPECT_EQ(get_typed_property(config.get<Config_object&>("server"), "timeout"), "int 30");
}

TEST_F(Typing_loading, local_config_wrong_type_throws) {
	const auto message = get_load_error("port: abc\n");

	EXPECT_NE(message.find("'port'"), std::string::npos) << message;
	EXPECT_NE(message.find("local.yaml"), std::string::npos) << message;

	EXPECT_NE(get_load_error("debug: 1\n"), "");
	EXPECT_NE(get_load_error("server: 5\n"), "");
	EXPECT_NE(get_load_error("limits: [a]\n"), "");
}

TEST_F(Typing_loading, local_config_variables_are_converted) {
	// the variables in the overriding values are resolved and converted to the type of the property
	const auto config = load("server:\n  timeout: \"${port}\"\nname: \"${port}\"\nport: 9000\n");

	EXPECT_EQ(get_typed_property(config.get<Config_object&>("server"), "timeout"), "int 9000");
	EXPECT_EQ(get_typed_property(config, "name"), "string 9000");
	EXPECT_EQ(get_typed_property(config, "port_copy"), "int 9000");
}

TEST_F(Typing_loading, local_config_variable_with_wrong_type_throws) {
	const auto message = get_load_error("debug: \"${name}\"\n");

	EXPECT_NE(message.find("'debug'"), std::string::npos) << message;
	EXPECT_NE(message.find("variable"), std::string::npos) << message;
}

TEST_F(Typing_loading, command_line_values_are_converted) {
	const auto config = load("", {"--name", "123", "--port", "9000", "--ratio", "2", "--debug", "--timeout", "5"});

	// the value for a string property is not changed
	EXPECT_EQ(get_typed_property(config, "name"), "string 123");
	EXPECT_EQ(get_typed_property(config, "port"), "int 9000");
	EXPECT_EQ(get_typed_property(config, "ratio"), "float 2.0");
	EXPECT_EQ(get_typed_property(config, "debug"), "bool true");
	EXPECT_EQ(get_typed_property(config.get<Config_object&>("server"), "timeout"), "int 5");
}

TEST_F(Typing_loading, command_line_string_value_is_kept_as_written) {
	EXPECT_EQ(get_typed_property(load("", {"--name", "1.50"}), "name"), "string 1.50");
	EXPECT_EQ(get_typed_property(load("", {"--name", "True"}), "name"), "string True");
	EXPECT_EQ(get_typed_property(load("", {"--name", "0x1F"}), "name"), "string 0x1F");
	EXPECT_EQ(get_typed_property(load("", {"--host", "10"}).get<Config_object&>("server"), "host"), "string 10");
}

TEST_F(Typing_loading, command_line_wrong_type_throws) {
	const auto message = get_load_error("", {"--port", "abc"});

	EXPECT_NE(message.find("'port'"), std::string::npos) << message;
	EXPECT_NE(message.find("command line"), std::string::npos) << message;

	EXPECT_NE(get_load_error("", {"--port", "1.5"}), "");
	EXPECT_NE(get_load_error("", {"--debug", "1"}), "");
	EXPECT_NE(get_load_error("", {"--ratio", "abc"}), "");
	// switch for a property that is not boolean
	EXPECT_NE(get_load_error("", {"--port"}), "");
	EXPECT_NE(get_load_error("", {"--name"}), "");
}

TEST_F(Typing_loading, command_line_value_for_property_with_variable) {
	// the type of the properties with variables is determined from the resolved master config
	EXPECT_EQ(get_typed_property(load("", {"--port_copy", "7"}), "port_copy"), "int 7");
	EXPECT_EQ(get_typed_property(load("", {"--url", "7"}), "url"), "string 7");
	EXPECT_NE(get_load_error("", {"--port_copy", "abc"}), "");
}

TEST_F(Typing_loading, path_values_from_other_sources) {
	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(
		write_config("master.yaml", "out_dir: /opt/out\nin_dir: /opt/in\nname: app\n")
	));
	config_definitions.add(std::make_unique<Config_definition>(
		Config_type::LOCAL,
		write_config("local.yaml", "out_dir: /srv/results\n")
	));
	const char* argv[] = {"program_name", "--in_dir", "/srv/my input", "--name", "./x"};
	config_definitions.add(std::make_unique<Command_line_config_definition>(5, argv));

	const auto config = load_config(config_definitions.config_definitions);

	EXPECT_EQ(get_typed_property(config, "out_dir"), "path /srv/results");
	EXPECT_EQ(get_typed_property(config, "in_dir"), "path /srv/my input");
	// a path given for a string property is a string
	EXPECT_EQ(get_typed_property(config, "name"), "string ./x");
}

TEST_F(Typing_loading, relative_paths_are_relative_to_their_source) {
	const auto master_directory = fs::canonical(directory);
	fs::create_directories(directory / "local");
	const auto local_directory = fs::canonical(directory / "local");

	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(write_config(
		"master.yaml",
		"master_dir: ./master\nlocal_dir: ./master\ncli_dir: ./master\nabsolute: /opt/app\nfiles: [./a.txt]\n"
		"data_dir: ./data\nfrom_variable: ${data_dir}/out\nlocal_from_variable: ./x\n"
	)));
	config_definitions.add(std::make_unique<Config_definition>(
		Config_type::LOCAL,
		// a bare path, and a path with a variable from the master config
		write_config("local/local.yaml", "local_dir: results\nfiles: [b.txt, ../c.txt]\nlocal_from_variable: ${data_dir}/y\n")
	));
	const char* argv[] = {"program_name", "--cli_dir", "cli_results", "--absolute", "/srv/app"};
	config_definitions.add(std::make_unique<Command_line_config_definition>(5, argv));

	const auto config = load_config(config_definitions.config_definitions);

	EXPECT_EQ(config.get<fs::path>("master_dir"), master_directory / "master");
	EXPECT_EQ(config.get<fs::path>("local_dir"), local_directory / "results");
	EXPECT_EQ(config.get<fs::path>("cli_dir"), fs::current_path() / "cli_results");
	EXPECT_EQ(config.get<fs::path>("absolute"), fs::path("/srv/app"));
	EXPECT_EQ(
		config.get_array<fs::path>("files"),
		(std::vector<fs::path>{local_directory / "b.txt", master_directory / "c.txt"})
	);

	// the variable holds a path that was made absolute in the master config
	EXPECT_EQ(config.get<fs::path>("from_variable"), master_directory / "data" / "out");
	EXPECT_EQ(config.get<fs::path>("local_from_variable"), master_directory / "data" / "y");
}

TEST_F(Typing_loading, relative_path_after_variable_substitution_throws) {
	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(
		write_config("master.yaml", "name: app\nout_dir: !path ${name}/out\n")
	));

	try {
		load_config(config_definitions.config_definitions);
		FAIL() << "exception expected";
	} catch(const std::runtime_error& e) {
		const std::string message = e.what();
		EXPECT_NE(message.find("'out_dir'"), std::string::npos) << message;
		EXPECT_NE(message.find("app/out"), std::string::npos) << message;
	}
}

TEST_F(Typing_loading, later_master_config_is_converted) {
	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(write_config("master.yaml", master_yaml)));
	config_definitions.add(std::make_unique<Config_definition>(write_config("master_2.yaml", "ratio: 3\nnew: 1\n")));

	const auto config = load_config(config_definitions.config_definitions);

	EXPECT_EQ(get_typed_property(config, "ratio"), "float 3.0");
	EXPECT_EQ(get_typed_property(config, "new"), "int 1");

	// the types are determined by the first master config
	config_definitions.add(std::make_unique<Config_definition>(write_config("master_3.yaml", "port: abc\n")));
	EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error);
}

TEST_F(Typing_loading, master_array_with_different_types_throws) {
	Config_definitions config_definitions;
	config_definitions.add(std::make_unique<Config_definition>(write_config("master.yaml", "array: [1, two]\n")));

	EXPECT_THROW(load_config(config_definitions.config_definitions), std::runtime_error);
}

TEST_F(Typing_loading, builder_config_has_resolved_variables) {
	std::vector<std::unique_ptr<Config_definition>> config_definitions;
	config_definitions.emplace_back(std::make_unique<Config_definition>(write_config("master.yaml", master_yaml)));

	const auto config = load_config_for_builder(config_definitions);

	// the types of the generated config class members are determined from the resolved values
	EXPECT_EQ(get_typed_property(config, "port_copy"), "int 8080");
	EXPECT_EQ(get_typed_property(config, "url"), "string http://localhost:8080");
}

} // namespace fc
