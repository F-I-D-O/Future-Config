cmake_minimum_required(VERSION 3.31)

project(setup-fconfig-test)

if(NOT DEFINED SETUP_FUNCTIONS_PATH OR NOT SETUP_FUNCTIONS_PATH)
	message(FATAL_ERROR "SETUP_FUNCTIONS_PATH not set")
endif()
if(NOT DEFINED FCONFIG_BUILDER_EXECUTABLE OR NOT FCONFIG_BUILDER_EXECUTABLE)
	message(FATAL_ERROR "FCONFIG_BUILDER_EXECUTABLE not set")
endif()

include("${SETUP_FUNCTIONS_PATH}")

function(set_stable_runtime_dir TARGET_NAME OUTPUT_DIR)
	set_target_properties(${TARGET_NAME}
		PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}"
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
			RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIR}"
			RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${OUTPUT_DIR}"
	)
endfunction()

add_executable(setup_fconfig_single src/main.cpp)
set_stable_runtime_dir(setup_fconfig_single "${CMAKE_CURRENT_BINARY_DIR}/bin/single")

add_executable(setup_fconfig_multiple src/main.cpp)
set_stable_runtime_dir(setup_fconfig_multiple "${CMAKE_CURRENT_BINARY_DIR}/bin/multiple")

set(SINGLE_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/single/src")
set(MULTIPLE_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/multiple/src")

setup_fconfig(
	ROOT_CONFIG_CLASS_NAME "future-config-usage-test"
	SOURCE_DIR "${SINGLE_SOURCE_DIR}"
	FCONFIG_BUILDER_EXECUTABLE "${FCONFIG_BUILDER_EXECUTABLE}"
	MAIN_CONFIG_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/data/config.yaml"
	TARGET_NAMES setup_fconfig_single
)

setup_fconfig(
	ROOT_CONFIG_CLASS_NAME "future-config-usage-test"
	SOURCE_DIR "${MULTIPLE_SOURCE_DIR}"
	FCONFIG_BUILDER_EXECUTABLE "${FCONFIG_BUILDER_EXECUTABLE}"
	MAIN_CONFIG_PATHS
		"${CMAKE_CURRENT_SOURCE_DIR}/data/config.yaml"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/other.yaml"
	TARGET_NAMES setup_fconfig_multiple
)
