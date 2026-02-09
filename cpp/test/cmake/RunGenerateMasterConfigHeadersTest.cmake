# Test script for generate_master_config_headers from setupFunctions.cmake
#
# Plain CMake test - no project configuration required.
# Tests generate_master_config_headers with one main config file.
#
# Required variables (pass via -D):
#   SETUP_FUNCTIONS_PATH       - path to install/setupFunctions.cmake
#   TEST_OUTPUT_DIR            - directory for generated output
#   EXPECTED_CONFIG_PATHS_PATH - path to expected config_paths.h content
#   MASTER_CONFIG_PATHS        - config paths (semicolon-separated, e.g. "data/config.yaml;data/other.yaml")
#
# The test:
# 1. Creates output directory structure
# 2. Includes setupFunctions and calls generate_master_config_headers with MASTER_CONFIG_PATHS
# 3. Compares generated config_paths.h against expected file

if(NOT DEFINED SETUP_FUNCTIONS_PATH)
	message(FATAL_ERROR "SETUP_FUNCTIONS_PATH not set")
endif()
if(NOT DEFINED TEST_OUTPUT_DIR)
	message(FATAL_ERROR "TEST_OUTPUT_DIR not set")
endif()
if(NOT DEFINED EXPECTED_CONFIG_PATHS_PATH)
	message(FATAL_ERROR "EXPECTED_CONFIG_PATHS_PATH not set")
endif()
if(NOT DEFINED MASTER_CONFIG_PATHS OR NOT MASTER_CONFIG_PATHS)
	message(FATAL_ERROR "MASTER_CONFIG_PATHS not set")
endif()

if(NOT EXISTS "${SETUP_FUNCTIONS_PATH}")
	message(FATAL_ERROR "setupFunctions.cmake not found: ${SETUP_FUNCTIONS_PATH}")
endif()
if(NOT EXISTS "${EXPECTED_CONFIG_PATHS_PATH}")
	message(FATAL_ERROR "Expected config paths file not found: ${EXPECTED_CONFIG_PATHS_PATH}")
endif()

get_filename_component(TEST_OUTPUT_DIR "${TEST_OUTPUT_DIR}" ABSOLUTE)
set(SOURCE_DIR "${TEST_OUTPUT_DIR}/src")
set(CONFIG_DIR "${SOURCE_DIR}/config")

# Step 1: Prepare output directory
message(STATUS "Step 1: Creating output directory ${CONFIG_DIR}")
file(REMOVE_RECURSE "${CONFIG_DIR}")
file(MAKE_DIRECTORY "${CONFIG_DIR}")

# Step 2: Include setupFunctions and call generate_master_config_headers
message(STATUS "Step 2: Calling generate_master_config_headers with MASTER_CONFIG_PATHS=${MASTER_CONFIG_PATHS}")
include("${SETUP_FUNCTIONS_PATH}")

generate_master_config_headers(
	SOURCE_DIR "${SOURCE_DIR}"
	MASTER_CONFIG_PATHS ${MASTER_CONFIG_PATHS}
)

# Step 3: Compare generated file against expected
set(GENERATED_FILE "${CONFIG_DIR}/config_paths.h")
if(NOT EXISTS "${GENERATED_FILE}")
	message(FATAL_ERROR "Generated config_paths.h not found: ${GENERATED_FILE}")
endif()
message(STATUS "Generated file found: ${GENERATED_FILE}")

file(READ "${GENERATED_FILE}" GENERATED_CONTENT)
file(READ "${EXPECTED_CONFIG_PATHS_PATH}" EXPECTED_CONTENT)

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
		"--- Expected (${EXPECTED_CONFIG_PATHS_PATH}) ---\n${EXPECTED_CONTENT}\n"
		"--- Actual (${GENERATED_FILE}) ---\n${GENERATED_CONTENT}\n"
		"--- End ---"
	)
endif()

message(STATUS "RunGenerateMasterConfigHeadersTest: All checks passed.")
