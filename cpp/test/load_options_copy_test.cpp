//
// Created by david on 2026-02-09.
//

#pragma message("NOTICE: Compiling copy contract test: Load_options. If the compilation of this unit fails, it almost certainly means that the contract was broken and Load_options class is not copy constructible.")

#include <type_traits>
#include "gtest/gtest.h"

#include "future-config/configuration.h"

namespace fc {
static_assert(std::is_copy_constructible_v<Load_options>);

TEST(load_options_copy_test, test) {
	Load_options l1{.config_definitions = Config_definitions()};
	Load_options l2 = l1;
}

}