#include "gtest/gtest.h"

#include <cstdint>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

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


// types of the values given as text

TEST(Typing, parse_scalar) {
	EXPECT_EQ(get_typed_scalar("5"), "int 5");
	EXPECT_EQ(get_typed_scalar("-5"), "int -5");
	EXPECT_EQ(get_typed_scalar("1.5"), "float 1.5");
	EXPECT_EQ(get_typed_scalar("true"), "bool true");
	EXPECT_EQ(get_typed_scalar("hello"), "string hello");
	EXPECT_EQ(get_typed_scalar(""), "string ");
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
	EXPECT_EQ(get_typed_scalar("~"), "string ~");
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

} // namespace fc
