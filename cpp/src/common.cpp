#include <sstream>
#include <ranges>
#include <regex>

#include "future-config/common.h"
#include "future-config/format.h"
    
    
namespace fc {    

    


const std::regex float_regex(R"regex([-+]?[0-9]+\.[0-9]+?)regex");
const std::regex int_regex(R"regex([-+]?[0-9]+)regex");


const std::unordered_map<Scalar_type::Value, std::string> Scalar_type::cpp_source_type_string_map = {
	{STRING, "std::string"},
	{INT, "int"},
	{FLOAT, "double"},
	{BOOL, "bool"}
};

Config_definition::Config_definition(Config_type type, fs::path yaml_file_path):
	Config_definition_base(type), yaml_file_path(std::move(yaml_file_path))
{
	try {
		check_path(yaml_file_path);
	} catch(const std::runtime_error& e) {
		throw std::runtime_error(
			format::format("Configuration file does not exist: {}", yaml_file_path.string(), e.what())
		);
	}
}

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

Scalar_type get_scalar_type_from_string(const std::string& string) {
	if(string == "true" || string == "false") {
		return Scalar_type::BOOL;
	}

	auto float_found = std::regex_match(string, float_regex);
	if(float_found) {
		return Scalar_type::FLOAT;
	}

	auto int_found = std::regex_match(string, int_regex);
	if(int_found) {
		return Scalar_type::INT;
	}
	return Scalar_type::STRING;
}

std::vector<std::unique_ptr<Config_definition>> parse_dependency_config_definitions(
	const std::vector<std::string>& dependency_config_strings
) {
	std::vector<std::unique_ptr<Config_definition>> dependency_config_definitions;
	for(const auto& dependency_config_string: dependency_config_strings) {
		// parse the dependency config definition tuple argument
		auto parts_view = std::ranges::views::split(dependency_config_string, ',');
		std::vector<std::string> parts;
		for(const auto& part: parts_view) {
			// this is needed for GCC11 compatibility. When dropping support, we can use the `part` range directly and
			// emplace to the `parts` vector
			std::string part_str;
			std::ranges::copy(part, std::back_inserter(part_str));

			parts.push_back(part_str);
		}

		dependency_config_definitions.emplace_back(std::make_unique<Dependency_config_definition>(
			fs::path(parts.at(0)),
			parts.at(1),
			fs::path(parts.at(2))
		));
	}
	return dependency_config_definitions;
}



std::filesystem::path check_path(const std::filesystem::path& path) {
	const auto abs_path = std::filesystem::absolute(path);

	if(!std::filesystem::exists(abs_path)) {
		auto message
			= format::format("File does not exists: {} ", path.string());
		if(abs_path != path) {
			message += format::format(" absolute path: {}", abs_path.string());
		}
		throw std::runtime_error(message);
	}

	return std::filesystem::canonical(path);
}
        
}
    