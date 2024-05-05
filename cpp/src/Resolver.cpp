//
// Created by Fido on 2024-05-05.
//

#include <ranges>
#include <string>
#include "spdlog/spdlog.h"

#include "Resolver.h"

YAML::Node Resolver::resolve() {
	add_all_variables_to_queue(yaml_config);
	process_queue();
	return yaml_config;
}

void Resolver::add_all_variables_to_queue(const YAML::Node& yaml_config_object) {
	for(YAML::const_iterator it = yaml_config_object.begin(); it != yaml_config_object.end(); ++it) {
		auto node = it->second;
		if(node.IsScalar()) {
			auto scalar = node.as<std::string>();
			std::smatch matches;
			const auto found = std::regex_search(scalar, matches, variable_regex);
			if(found) {
				unresolved_variables.emplace(yaml_config_object, it->first.as<std::string>());
			}
		}
		else if(node.IsMap()) {
			add_all_variables_to_queue(node);
		}
		else if(node.IsSequence()) {
			for(const auto& sequence_node: node) {
				add_all_variables_to_queue(sequence_node);
			}
		}
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
					auto value = unresolved_parent[unresolved_key].as<std::string>();
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

std::tuple<Resolve_status,std::string> Resolver::resolve_value(YAML::Node & node) const {
	auto string_value = node.as<std::string>();
	std::smatch matches;
	std::regex_search(string_value, matches, variable_regex);
	for(auto i = 1; i < matches.size(); i++) {
		const auto& match = matches[i];
		auto variable = match.str();
		auto variable_value = get_value(variable);
		std::smatch variable_value_matches;
		bool contains_variable = std::regex_search(variable_value, variable_value_matches, variable_regex);
		if(contains_variable) {
			if(i == 1) {
				return {Resolve_status::FAILED, string_value};
			}
			else {
				return {Resolve_status::PARTIAL, string_value};
			}
		}
		else {
			string_value.replace(match.first, match.second, variable_value);
		}
	}
	return {Resolve_status::COMPLETE, string_value};
}

std::string Resolver::get_value(const std::string& var_name) const {
	auto current_object = yaml_config;
	auto split_view = std::views::split(var_name, '.');
	auto part_count = std::ranges::distance(split_view);
	unsigned short i = 0;
	for(auto key: split_view
		| std::views::transform([](auto&& r) { return std::string(r.begin(), r.end()); })
	){
		if(current_object[key]) {
			if(i < part_count - 1) {
				current_object = current_object[key];
			}
			else {
				return current_object[key].as<std::string>();
			}
		}
		else{
			throw std::runtime_error("Variable not found in config: " + var_name);
		}
		++i;
	}

	throw std::runtime_error("Variable not found in config: " + var_name + " (should not reach this point)");
}
