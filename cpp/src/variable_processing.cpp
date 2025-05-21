//
// Created by Gemini on 2024-05-23.
//

#include "future-config/loading.h" // For process_variables declaration
#include "Resolver.h"                  // For fc::Resolver
#include "future-config/Config_object.h" // For fc::Config_object

namespace fc {

void process_variables(Config_object& config) {
    Resolver(config).resolve();
}

} // namespace fc 