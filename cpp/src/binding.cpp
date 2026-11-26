#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "future-config/format.h"
#include "future-config/loading.h"

namespace fc {

namespace {

// Splits the dot-separated bind path to keys. An empty bind path (root) results in an empty vector.
std::vector<std::string> split_bind_path(const std::string& bind_path) {
	std::vector<std::string> keys;
	if(bind_path.empty()) {
		return keys;
	}

	std::string::size_type start = 0;
	while(true) {
		const auto end = bind_path.find('.', start);
		if(end == std::string::npos) {
			keys.push_back(bind_path.substr(start));
			break;
		}
		keys.push_back(bind_path.substr(start, end - start));
		start = end + 1;
	}
	return keys;
}

} // namespace

const Config_object& get_bound_object(
	const Config_object& config,
	const std::string& bind_path,
	const std::string& source_description
) {
	const Config_object* current_object = &config;
	for(const auto& key: split_bind_path(bind_path)) {
		if(!current_object->contains(key)) {
			throw std::runtime_error(format::format(
				"Cannot bind {} to '{}': there is no '{}' key in the configuration.",
				source_description,
				bind_path,
				key
			));
		}
		const auto& property = (*current_object)[key];
		if(!std::holds_alternative<config_object_property_value>(property)) {
			throw std::runtime_error(format::format(
				"Cannot bind {} to '{}': '{}' is not an object.",
				source_description,
				bind_path,
				key
			));
		}
		current_object = std::get<config_object_property_value>(property).get();
	}
	return *current_object;
}

Config_object bind_config(Config_object config, const std::string& bind_path) {
	const auto keys = split_bind_path(bind_path);

	// wrap the config to the objects from the bind path, starting from the innermost one
	for(auto key_it = keys.rbegin(); key_it != keys.rend(); ++key_it) {
		Config_object parent;
		parent.emplace(*key_it, std::make_unique<Config_object>(std::move(config)));
		config = std::move(parent);
	}
	return config;
}

} // namespace fc
