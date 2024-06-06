//
// Created by Fido on 2024-05-05.
//

#include <ranges>
#include <string>
#include "spdlog/spdlog.h"

#include "Resolver.h"

void Resolver::resolve() {
	add_all_variables_to_queue(config_object);
	process_queue();
}

void Resolver::add_all_variables_to_queue(const Config_object& config_object) {
	for(const auto&[key, value]: config_object) {
		if(std::holds_alternative<Config_object>(value)) {
			add_all_variables_to_queue(std::get<Config_object>(value));
		}
		else if(std::holds_alternative<std::vector<Config_object>>(value)) {
			for(const auto& object: std::get<std::vector<Config_object>>(value)) {
				add_all_variables_to_queue(object);
			}
		}
		else {
			auto scalar = std::get<std::string>(value);
			std::smatch matches;
			const auto found = std::regex_search(scalar, matches, variable_regex);
			if(found) {
				unresolved_variables.emplace(config_object, key);
			}
		}
		// variables in scalar arrays are not supported
	}
}

void Resolver::process_queue() {
	auto last_queue_length = unresolved_variables.size();

	// at least one variable has to be resolved before trying the same variables again
	auto check_counter = last_queue_length;

	while(!unresolved_variables.empty()) {
		auto[parent, key] = unresolved_variables.front();
		unresolved_variables.pop();

		auto config_property = parent[key];
		auto [status, variable_value] = resolve_value(config_property);
		if(status != Resolve_status::FAILED) {
			parent[key] = variable_value;
		}
		if(status != Resolve_status::COMPLETE) {
			unresolved_variables.emplace(parent, key);
		}

		// check for unresolvable references
		if(check_counter == 0){
			if(last_queue_length == unresolved_variables.size()) {
				std::string unresolved_variables_str;
				while(!unresolved_variables.empty()) {
					auto [unresolved_parent, unresolved_key] = unresolved_variables.front();
					unresolved_variables.pop();
					auto value = std::get<std::string>(unresolved_parent[unresolved_key]);
					unresolved_variables_str += std::format("{}: {}\n", unresolved_key, value);
				}
				spdlog::error(
					"None of the remaining variables can be resolved. Remaining variables: {}",
					unresolved_variables_str
				);
				throw std::runtime_error("Unresolvable variables in config");
			}

			last_queue_length = unresolved_variables.size();
			check_counter = last_queue_length;
		}

		check_counter -= 1;
	}
}

std::tuple<Resolve_status,std::string> Resolver::resolve_value(config_property_value& config_property_val) const {
	auto string_value = std::get<std::string>(config_property_val);
	std::smatch matches;
	unsigned short variable_counter = 1;
	while(std::regex_search(string_value, matches, variable_regex)){
		auto match = matches[1];
		auto variable = match.str();
		auto variable_value = get_value(variable);
		std::smatch variable_value_matches;
		bool contains_variable = std::regex_search(variable_value, variable_value_matches, variable_regex);
		if(contains_variable) {
			if(variable_counter == 1) {
				return {Resolve_status::FAILED, string_value};
			}
			else {
				return {Resolve_status::PARTIAL, string_value};
			}
		}
		else {
			string_value = string_value.replace(match.first - 2, match.second + 1, variable_value);
		}
		++variable_counter;
	}
	return {Resolve_status::COMPLETE, string_value};
}

std::string Resolver::get_value(const std::string& var_name) const {
	const auto* current_object = &config_object;
	auto split_view = std::views::split(var_name, '.');
	auto part_count = std::ranges::distance(split_view);
	unsigned short i = 0;
	for(auto key: split_view
		| std::views::transform([](auto&& r) { return std::string(r.begin(), r.end()); })
	){
		if(current_object->contains(key)) {
			if(i < part_count - 1) {
				current_object = &std::get<Config_object>(current_object->operator[](key));
			}
			else {
				return std::get<std::string>(current_object->operator[](key));
			}
		}
		else{
			throw std::runtime_error("Variable not found in config: " + var_name);
		}
		++i;
	}

	throw std::runtime_error("Variable not found in config: " + var_name + " (should not reach this point)");
}
