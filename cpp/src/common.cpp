#include <sstream>
#include "common.h"

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

std::string get_scalar_type_from_yaml_node(const YAML::Node& node) {
	if (node.IsScalar()) {
		if (node.as<std::string>() == "true" || node.as<std::string>() == "false") {
			return "bool";
		} else if (node.as<std::string>().find('.') != std::string::npos) {
			return "double";
		} else if (node.as<std::string>().find_first_not_of("0123456789") == std::string::npos) {
			return "int";
		}
		return "std::string";
	}
	throw std::runtime_error("Unsupported YAML node type");
}
