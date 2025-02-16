//
// Created by Fido on 2024-10-16.
//

#pragma once

#ifdef FCONFIG_USE_FMT
#include <fmt/core.h>
namespace format = fmt;
#else
#include <format>
namespace format = std;
#endif