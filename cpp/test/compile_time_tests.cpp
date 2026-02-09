#include <gtest/gtest.h>

#include "future-config/configuration.h"
#include "compile_time_tests.h"

namespace fc {

static_assert(Input_pointer_range<std::vector<std::unique_ptr<Config_definition_base>>, Config_definition_base>);
static_assert(Input_pointer_range<std::vector<std::unique_ptr<Config_definition>>, Config_definition>);
static_assert(Input_pointer_range<std::vector<std::unique_ptr<Config_definition>>, Config_definition_base>);


}