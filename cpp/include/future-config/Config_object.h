//
// Created by david on 2024-05-08.
//

#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <filesystem>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

#include "future-config/format.h"
#include "future-config/future-config_export.h"


namespace fc {


class Config_object;

/**
 * Scalar config value. The type of the value is determined when the config is loaded.
 */
using Config_scalar = std::variant<
	std::string,
	std::int64_t,
	double,
	bool,
	std::filesystem::path
>;

using config_object_property_value = std::unique_ptr<Config_object>;


using config_property_value = std::variant<
	Config_scalar,
	std::vector<Config_scalar>,
	config_object_property_value
>;

constexpr unsigned short scalar_index = 0;
constexpr unsigned short array_index = 1;
constexpr unsigned short object_index = 2;

/**
 * @return name of the type of the scalar value: string, int, float, bool, or path
 */
FUTURE_CONFIG_EXPORT std::string get_type_name(const Config_scalar& scalar);

/**
 * @return text representation of the scalar value, as it would be written in a YAML file
 */
FUTURE_CONFIG_EXPORT std::string to_string(const Config_scalar& scalar);


/**
 * @brief Class template for getting the scalar values as various C++ types. The values are not converted from text,
 * the requested type has to match the type of the value.
 */
template<typename T>
struct scalar_converter {
	static T convert(const Config_scalar& scalar, const std::string& key) = delete;
};

namespace detail {

template<typename T>
const T& get_scalar_alternative(const Config_scalar& scalar, const std::string& key, const std::string& requested_type) {
	if(!std::holds_alternative<T>(scalar)) {
		throw std::runtime_error(format::format(
			"Property {} is of type {}, it cannot be read as {}",
			key,
			get_type_name(scalar),
			requested_type
		));
	}
	return std::get<T>(scalar);
}

}

template<>
struct scalar_converter<std::string> {
	static std::string convert(const Config_scalar& scalar, const std::string& key) {
		return detail::get_scalar_alternative<std::string>(scalar, key, "string");
	}
};

template<>
struct scalar_converter<bool> {
	static bool convert(const Config_scalar& scalar, const std::string& key) {
		return detail::get_scalar_alternative<bool>(scalar, key, "bool");
	}
};

template<>
struct scalar_converter<std::filesystem::path> {
	static std::filesystem::path convert(const Config_scalar& scalar, const std::string& key) {
		return detail::get_scalar_alternative<std::filesystem::path>(scalar, key, "path");
	}
};

template<std::floating_point T>
struct scalar_converter<T> {
	static T convert(const Config_scalar& scalar, const std::string& key) {
		// integer values can be read as floating point numbers
		if(std::holds_alternative<std::int64_t>(scalar)) {
			return static_cast<T>(std::get<std::int64_t>(scalar));
		}
		return static_cast<T>(detail::get_scalar_alternative<double>(scalar, key, "float"));
	}
};

template<std::integral T>
struct scalar_converter<T> {
	static T convert(const Config_scalar& scalar, const std::string& key) {
		const auto value = detail::get_scalar_alternative<std::int64_t>(scalar, key, "int");
		if(!std::in_range<T>(value)) {
			throw std::runtime_error(format::format(
				"Value {} of property {} is out of range of the requested integer type",
				value,
				key
			));
		}
		return static_cast<T>(value);
	}
};


class FUTURE_CONFIG_EXPORT Config_object {
	using properties_map = std::unordered_map<std::string, config_property_value>;


	properties_map properties;

public:
	Config_object() = default; // Define as inline default constructor


	Config_object(const Config_object& other);

	Config_object(Config_object&& other) noexcept = default;

	Config_object& operator=(const Config_object& other);

	Config_object& operator=(Config_object&& other) noexcept = default;


	auto begin() {
		return properties.begin();
	}

	auto end() {
		return properties.end();
	}

	[[nodiscard]] auto begin() const {
		return properties.begin();
	}

	[[nodiscard]] auto end() const {
		return properties.end();
	}

	[[nodiscard]] bool contains(const std::string& key) const {
		return properties.contains(key);
	}

	config_property_value& operator[](const std::string& key) {
		return properties.at(key);
	}

	const config_property_value& operator[](const std::string& key) const {
		return properties.at(key);
	}

	/**
	 * @brief emplace wrapper. We need it because the current implementation of config_object_property_value is
	 * a unique_ptr to Config_object which is not copyable.
	 * @param key
	 * @param value
	 */
	void emplace(const std::string& key, config_property_value&& value) {
		properties.insert_or_assign(key, std::move(value));
	}

	[[nodiscard]] unsigned short size() const {
		return static_cast<unsigned short>(properties.size());
	}

	/**
	 * Gets a scalar property. The requested type has to match the type of the value: std::string for strings, bool for
	 * booleans, std::filesystem::path for paths, any integer type for integers (the value has to be in the range of the
	 * type), and any floating point type for floating point numbers and integers.
	 * @tparam T requested type
	 * @param key key of the property
	 * @throws std::runtime_error if the property is not a scalar or if it is of a different type
	 */
	template<typename T>
	T get(const std::string& key) const {
		const auto& property = properties.at(key);
		if(std::holds_alternative<Config_scalar>(property)) {
			return scalar_converter<T>::convert(std::get<Config_scalar>(property), key);
		}
		throw std::runtime_error(format::format("Property {} is not a scalar", key));
	}

	/**
	 * Gets an array property. The same rules as for the scalar properties apply for each item of the array.
	 * @tparam T requested type of the array items
	 * @param key key of the property
	 * @throws std::runtime_error if the property is not an array of scalars or if some item is of a different type
	 */
	template<typename T>
	std::vector<T> get_array(const std::string& key) const {
		const auto& property = properties.at(key);
		if(std::holds_alternative<std::vector<Config_scalar>>(property)) {
			std::vector<T> result;
			std::ranges::transform(
				std::get<std::vector<Config_scalar>>(property),
				std::back_inserter(result),
				[&key](const Config_scalar& scalar) {
					return scalar_converter<T>::convert(scalar, key);
				}
			);
			return result;
		}
		else {
			throw std::runtime_error(format::format("Property {} is not an array of scalars", key));
		}
	}


};

template<>
FUTURE_CONFIG_EXPORT Config_object& Config_object::get<Config_object&>(const std::string& key) const;

static_assert(std::ranges::input_range<Config_object>);


}
