# Test script for setup_fconfig from setupFunctions.cmake
#
# Required variables (pass via -D):
#   SETUP_FCONFIG_TEST_SOURCE_DIR   - path to the setup_fconfig test project
#   SETUP_FCONFIG_TEST_BUILD_DIR    - build directory for the test project
#   SETUP_FUNCTIONS_PATH            - path to install/setupFunctions.cmake
#   FCONFIG_BUILDER_EXECUTABLE      - full path to fconfig_builder executable
#   EXPECTED_CONFIG_PATH            - expected generated config file content
#   EXPECTED_CONFIG_PATHS_ONE_PATH  - expected config_paths.h for one config
#   EXPECTED_CONFIG_PATHS_TWO_PATH  - expected config_paths.h for two configs

function(require_variable VARIABLE_NAME)
	if(NOT DEFINED ${VARIABLE_NAME} OR NOT ${VARIABLE_NAME})
		message(FATAL_ERROR "${VARIABLE_NAME} not set")
	endif()
endfunction()

function(compare_file GENERATED_FILE EXPECTED_FILE)
	if(NOT EXISTS "${GENERATED_FILE}")
		message(FATAL_ERROR "Generated file not found: ${GENERATED_FILE}")
	endif()
	if(NOT EXISTS "${EXPECTED_FILE}")
		message(FATAL_ERROR "Expected file not found: ${EXPECTED_FILE}")
	endif()

	file(READ "${GENERATED_FILE}" GENERATED_CONTENT)
	file(READ "${EXPECTED_FILE}" EXPECTED_CONTENT)

	string(REPLACE "\r\n" "\n" GENERATED_CONTENT "${GENERATED_CONTENT}")
	string(REPLACE "\r" "\n" GENERATED_CONTENT "${GENERATED_CONTENT}")
	string(REPLACE "\r\n" "\n" EXPECTED_CONTENT "${EXPECTED_CONTENT}")
	string(REPLACE "\r" "\n" EXPECTED_CONTENT "${EXPECTED_CONTENT}")

	string(STRIP "${GENERATED_CONTENT}" GENERATED_CONTENT)
	string(STRIP "${EXPECTED_CONTENT}" EXPECTED_CONTENT)

	if(NOT "${GENERATED_CONTENT}" STREQUAL "${EXPECTED_CONTENT}")
		message(FATAL_ERROR
			"Generated file does not match expected content.\n"
			"--- Expected (${EXPECTED_FILE}) ---\n${EXPECTED_CONTENT}\n"
			"--- Actual (${GENERATED_FILE}) ---\n${GENERATED_CONTENT}\n"
			"--- End ---"
		)
	endif()
endfunction()

require_variable(SETUP_FCONFIG_TEST_SOURCE_DIR)
require_variable(SETUP_FCONFIG_TEST_BUILD_DIR)
require_variable(SETUP_FUNCTIONS_PATH)
require_variable(FCONFIG_BUILDER_EXECUTABLE)
require_variable(EXPECTED_CONFIG_PATH)
require_variable(EXPECTED_CONFIG_PATHS_ONE_PATH)
require_variable(EXPECTED_CONFIG_PATHS_TWO_PATH)

foreach(REQUIRED_PATH IN ITEMS
	"${SETUP_FCONFIG_TEST_SOURCE_DIR}"
	"${SETUP_FUNCTIONS_PATH}"
	"${FCONFIG_BUILDER_EXECUTABLE}"
	"${EXPECTED_CONFIG_PATH}"
	"${EXPECTED_CONFIG_PATHS_ONE_PATH}"
	"${EXPECTED_CONFIG_PATHS_TWO_PATH}"
)
	if(NOT EXISTS "${REQUIRED_PATH}")
		message(FATAL_ERROR "Required path does not exist: ${REQUIRED_PATH}")
	endif()
endforeach()

get_filename_component(SETUP_FCONFIG_TEST_SOURCE_DIR "${SETUP_FCONFIG_TEST_SOURCE_DIR}" ABSOLUTE)
get_filename_component(SETUP_FCONFIG_TEST_BUILD_DIR "${SETUP_FCONFIG_TEST_BUILD_DIR}" ABSOLUTE)
get_filename_component(SETUP_FUNCTIONS_PATH "${SETUP_FUNCTIONS_PATH}" ABSOLUTE)
get_filename_component(FCONFIG_BUILDER_EXECUTABLE "${FCONFIG_BUILDER_EXECUTABLE}" ABSOLUTE)

message(STATUS "Preparing setup_fconfig test build directory: ${SETUP_FCONFIG_TEST_BUILD_DIR}")
file(REMOVE_RECURSE "${SETUP_FCONFIG_TEST_BUILD_DIR}")

set(CONFIGURE_COMMAND
	"${CMAKE_COMMAND}"
	-S "${SETUP_FCONFIG_TEST_SOURCE_DIR}"
	-B "${SETUP_FCONFIG_TEST_BUILD_DIR}"
	-D "SETUP_FUNCTIONS_PATH=${SETUP_FUNCTIONS_PATH}"
	-D "FCONFIG_BUILDER_EXECUTABLE=${FCONFIG_BUILDER_EXECUTABLE}"
)
if(DEFINED TEST_GENERATOR AND TEST_GENERATOR)
	list(APPEND CONFIGURE_COMMAND -G "${TEST_GENERATOR}")
endif()
if(DEFINED TEST_GENERATOR_PLATFORM AND TEST_GENERATOR_PLATFORM)
	list(APPEND CONFIGURE_COMMAND -A "${TEST_GENERATOR_PLATFORM}")
endif()
if(DEFINED TEST_GENERATOR_TOOLSET AND TEST_GENERATOR_TOOLSET)
	list(APPEND CONFIGURE_COMMAND -T "${TEST_GENERATOR_TOOLSET}")
endif()
if(DEFINED TEST_CXX_COMPILER AND TEST_CXX_COMPILER)
	list(APPEND CONFIGURE_COMMAND -D "CMAKE_CXX_COMPILER=${TEST_CXX_COMPILER}")
endif()

message(STATUS "Configuring setup_fconfig test project")
execute_process(
	COMMAND ${CONFIGURE_COMMAND}
	RESULT_VARIABLE CONFIGURE_RESULT
	OUTPUT_VARIABLE CONFIGURE_OUTPUT
	ERROR_VARIABLE CONFIGURE_OUTPUT
)
if(CONFIGURE_RESULT)
	message(FATAL_ERROR
		"setup_fconfig test project configure failed with code ${CONFIGURE_RESULT}.\n"
		"Command: ${CONFIGURE_COMMAND}\n"
		"Output:\n${CONFIGURE_OUTPUT}"
	)
endif()

set(SINGLE_GENERATED_DIR "${SETUP_FCONFIG_TEST_BUILD_DIR}/generated/single/src/config")
set(MULTIPLE_GENERATED_DIR "${SETUP_FCONFIG_TEST_BUILD_DIR}/generated/multiple/src/config")

compare_file(
	"${SINGLE_GENERATED_DIR}/future-config-usage-test_config.h"
	"${EXPECTED_CONFIG_PATH}"
)
compare_file(
	"${SINGLE_GENERATED_DIR}/config_paths.h"
	"${EXPECTED_CONFIG_PATHS_ONE_PATH}"
)
compare_file(
	"${MULTIPLE_GENERATED_DIR}/config_paths.h"
	"${EXPECTED_CONFIG_PATHS_TWO_PATH}"
)

set(BUILD_COMMAND
	"${CMAKE_COMMAND}"
	--build "${SETUP_FCONFIG_TEST_BUILD_DIR}"
)
if(DEFINED TEST_CONFIGURATION AND TEST_CONFIGURATION)
	list(APPEND BUILD_COMMAND --config "${TEST_CONFIGURATION}")
endif()

message(STATUS "Building setup_fconfig test project")
execute_process(
	COMMAND ${BUILD_COMMAND}
	RESULT_VARIABLE BUILD_RESULT
	OUTPUT_VARIABLE BUILD_OUTPUT
	ERROR_VARIABLE BUILD_OUTPUT
)
if(BUILD_RESULT)
	message(FATAL_ERROR
		"setup_fconfig test project build failed with code ${BUILD_RESULT}.\n"
		"Command: ${BUILD_COMMAND}\n"
		"Output:\n${BUILD_OUTPUT}"
	)
endif()

set(SINGLE_TARGET_DATA_DIR "${SETUP_FCONFIG_TEST_BUILD_DIR}/bin/single/data")
set(MULTIPLE_TARGET_DATA_DIR "${SETUP_FCONFIG_TEST_BUILD_DIR}/bin/multiple/data")

if(NOT EXISTS "${SINGLE_TARGET_DATA_DIR}/config.yaml")
	message(FATAL_ERROR "Single-config target did not receive config.yaml")
endif()
if(EXISTS "${SINGLE_TARGET_DATA_DIR}/other.yaml")
	message(FATAL_ERROR "Single-config target unexpectedly received other.yaml")
endif()
if(NOT EXISTS "${MULTIPLE_TARGET_DATA_DIR}/config.yaml")
	message(FATAL_ERROR "Multiple-config target did not receive config.yaml")
endif()
if(NOT EXISTS "${MULTIPLE_TARGET_DATA_DIR}/other.yaml")
	message(FATAL_ERROR "Multiple-config target did not receive other.yaml")
endif()

message(STATUS "RunSetupFconfigTest: All checks passed.")
