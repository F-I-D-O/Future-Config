#include <sstream>
#include "common.h"

const std::unordered_map<Scalar_type::Value, std::string> Scalar_type::cpp_source_type_string_map = {
	{STRING, "std::string"},
	{INT, "int"},
	{FLOAT, "double"},
	{BOOL, "bool"}
};

std::string join(const std::vector <std::string>& v, const std::string& delimiter) {
	std::ostringstream ss;
	auto begin = v.begin();
	auto end = v.end();

	if(begin != end)
	{
		ss << *begin++; // see 3.
	}

	while(begin != end) // see 3.
	{
		ss << delimiter;
		ss << *begin++;
	}

	return ss.str();
}

Scalar_type get_scalar_type_from_yaml_node(const YAML::Node& node) {
	if (node.IsScalar()) {
		return get_scalar_type_from_string(node.as<std::string>());
	}
	throw std::runtime_error("Unsupported YAML node type");
}

Scalar_type get_scalar_type_from_string(const std::string& string) {
	if (string == "true" || string == "false") {
		return Scalar_type::BOOL;
	} else if (string.find('.') != std::string::npos) {
		return Scalar_type::FLOAT;
	} else if (string.find_first_not_of("0123456789") == std::string::npos) {
		return Scalar_type::INT;
	}
	return Scalar_type::STRING;
}