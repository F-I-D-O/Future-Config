# Test script for run_fconfig_builder from setupFunctions.cmake
#
# Required variables (pass via -D):
#   USAGE_TEST_SOURCE_DIR       - path to test/usage_test
#   SETUP_FUNCTIONS_PATH        - path to install/setupFunctions.cmake
#   FCONFIG_BUILDER_EXECUTABLE  - full path to fconfig_builder executable
#   EXPECTED_CONFIG_PATH        - path to expected generated config file
#
# The test:
# 1. Deletes content of usage_test/src/config
# 2. Includes setupFunctions and calls run_fconfig_builder (with FCONFIG_BUILDER_EXECUTABLE override)
# 3. Locates the generated file and compares it to expected output

if(NOT DEFINED USAGE_TEST_SOURCE_DIR)
	message(FATAL_ERROR "USAGE_TEST_SOURCE_DIR not set")
endif()
if(NOT DEFINED SETUP_FUNCTIONS_PATH)
	message(FATAL_ERROR "SETUP_FUNCTIONS_PATH not set")
endif()
if(NOT DEFINED FCONFIG_BUILDER_EXECUTABLE)
	message(FATAL_ERROR "FCONFIG_BUILDER_EXECUTABLE not set")
endif()
if(NOT DEFINED EXPECTED_CONFIG_PATH)
	message(FATAL_ERROR "EXPECTED_CONFIG_PATH not set")
endif()

set(USAGE_TEST_CONFIG_DIR "${USAGE_TEST_SOURCE_DIR}/src/config")
set(USAGE_TEST_SRC_DIR "${USAGE_TEST_SOURCE_DIR}/src")
set(USAGE_TEST_MAIN_CONFIG "${USAGE_TEST_SOURCE_DIR}/data/config.yaml")

# Verify prerequisites exist
if(NOT EXISTS "${USAGE_TEST_SOURCE_DIR}")
	message(FATAL_ERROR "Usage test directory does not exist: ${USAGE_TEST_SOURCE_DIR}")
endif()
if(NOT EXISTS "${SETUP_FUNCTIONS_PATH}")
	message(FATAL_ERROR "setupFunctions.cmake not found: ${SETUP_FUNCTIONS_PATH}")
endif()
if(NOT EXISTS "${USAGE_TEST_MAIN_CONFIG}")
	message(FATAL_ERROR "Main config file not found: ${USAGE_TEST_MAIN_CONFIG}")
endif()

# Get absolute paths for robustness
get_filename_component(USAGE_TEST_SOURCE_DIR "${USAGE_TEST_SOURCE_DIR}" ABSOLUTE)
get_filename_component(USAGE_TEST_MAIN_CONFIG "${USAGE_TEST_MAIN_CONFIG}" ABSOLUTE)
get_filename_component(USAGE_TEST_SRC_DIR "${USAGE_TEST_SRC_DIR}" ABSOLUTE)
get_filename_component(USAGE_TEST_CONFIG_DIR "${USAGE_TEST_CONFIG_DIR}" ABSOLUTE)

# Step 1: Delete content of src/config folder
message(STATUS "Step 1: Deleting contents of ${USAGE_TEST_CONFIG_DIR}")
if(EXISTS "${USAGE_TEST_CONFIG_DIR}")
	file(REMOVE_RECURSE "${USAGE_TEST_CONFIG_DIR}")
endif()
file(MAKE_DIRECTORY "${USAGE_TEST_CONFIG_DIR}")

# Step 2: Include setupFunctions and call run_fconfig_builder
get_filename_component(FCONFIG_BUILDER_EXECUTABLE "${FCONFIG_BUILDER_EXECUTABLE}" ABSOLUTE)
message(STATUS "Step 2: Calling run_fconfig_builder")
include("${SETUP_FUNCTIONS_PATH}")

# Call run_fconfig_builder with explicit parameters (FCONFIG_BUILDER_EXECUTABLE bypasses search)
run_fconfig_builder(
	ROOT_CONFIG_CLASS_NAME "future-config-usage-test"
	SOURCE_DIR "${USAGE_TEST_SRC_DIR}"
	FCONFIG_BUILDER_EXECUTABLE "${FCONFIG_BUILDER_EXECUTABLE}"
	MAIN_CONFIG_PATH "${USAGE_TEST_MAIN_CONFIG}"
)

# Step 3: Locate the generated file and compare to expected
# The generated file is named: <root_config_class_name with hyphens>_config.h
# For "future-config-usage-test" -> future-config-usage-test_config.h
set(EXPECTED_GENERATED_FILE "${USAGE_TEST_CONFIG_DIR}/future-config-usage-test_config.h")

if(NOT EXISTS "${EXPECTED_GENERATED_FILE}")
	message(FATAL_ERROR "Generated config file not found: ${EXPECTED_GENERATED_FILE}")
endif()
message(STATUS "Generated file found: ${EXPECTED_GENERATED_FILE}")

# Compare with expected content
file(READ "${EXPECTED_GENERATED_FILE}" GENERATED_CONTENT)
file(READ "${EXPECTED_CONFIG_PATH}" EXPECTED_CONTENT)

# Normalize line endings for cross-platform comparison
string(REPLACE "\r\n" "\n" GENERATED_CONTENT "${GENERATED_CONTENT}")
string(REPLACE "\r" "\n" GENERATED_CONTENT "${GENERATED_CONTENT}")
string(REPLACE "\r\n" "\n" EXPECTED_CONTENT "${EXPECTED_CONTENT}")
string(REPLACE "\r" "\n" EXPECTED_CONTENT "${EXPECTED_CONTENT}")

# Trim trailing whitespace/newlines for robust comparison
string(STRIP "${GENERATED_CONTENT}" GENERATED_CONTENT)
string(STRIP "${EXPECTED_CONTENT}" EXPECTED_CONTENT)

if(NOT "${GENERATED_CONTENT}" STREQUAL "${EXPECTED_CONTENT}")
	message(FATAL_ERROR
		"Generated file does not match expected content.\n"
		"--- Expected (${EXPECTED_CONFIG_PATH}) ---\n${EXPECTED_CONTENT}\n"
		"--- Actual (${EXPECTED_GENERATED_FILE}) ---\n${GENERATED_CONTENT}\n"
		"--- End ---"
	)
endif()

message(STATUS "RunFconfigBuilderTest: All checks passed.")
