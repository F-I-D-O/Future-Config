//
// Created by Fido on 2024-10-16.
//

// This is needed because GCC incorrectly reports C++20 support

#pragma once

#if __has_include(<format>)
	#include <format>
	namespace format = std;
#else
	#include <fmt/core.h>
	namespace format = fmt;
#endif