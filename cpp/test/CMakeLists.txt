# GOOGLE TEST
include(GoogleTest) # cmake module
include(CTest) # for functional tests

# set the policy CMP0135 to use correct timestamp for archives downloaded by FetchContent
#cmake_policy(SET CMP0135 NEW)

# do not install gtest
set(BUILD_GMOCK OFF)

set(INSTALL_GTEST OFF)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
	googletest
	DOWNLOAD_EXTRACT_TIMESTAMP ON
	URL https://github.com/google/googletest/archive/6910c9d9165801d8827d628cb72eb7ea9dd538c5.zip
	#	EXCLUDE_FROM_ALL
)
# For Windows: Prevent overriding the parent project's compiler/linker settings

FetchContent_MakeAvailable(googletest)
# disable building google test targets by default
#set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)
#set_target_properties(gtest_main PROPERTIES EXCLUDE_FROM_ALL TRUE)


# MY TEST TARGET
set(FCONFIG_SRC_DIR "${PROJECT_SOURCE_DIR}/src")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../")
message(STATUS "CMAKE_CURRENT_BINARY_DIR : ${CMAKE_CURRENT_BINARY_DIR}")

add_executable(test_run
	#	EXCLUDE_FROM_ALL
	${FCONFIG_SRC_DIR}/Builder.cpp
	${FCONFIG_SRC_DIR}/command_line_loading.cpp
	${FCONFIG_SRC_DIR}/Command_line_parser.cpp
	${FCONFIG_SRC_DIR}/common.cpp
	${FCONFIG_SRC_DIR}/Config_object.cpp
	${FCONFIG_SRC_DIR}/loading.cpp
	${FCONFIG_SRC_DIR}/Merger.cpp
	${FCONFIG_SRC_DIR}/Parser.cpp
	${FCONFIG_SRC_DIR}/Resolver.cpp
	${FCONFIG_SRC_DIR}/resources.cpp
	${FCONFIG_SRC_DIR}/variable_processing.cpp
	builder_runner_tests.cpp
	bulider_tests.cpp
	common.cpp
	merger_test.cpp
	resolver_tests.cpp
	config_object_test.cpp
	compile_time_tests.cpp
	compile_time_tests.h
	load_options_copy_test.cpp
	configuration_tests.cpp
)

add_dependencies(test_run fconfig_builder)

target_compile_definitions(test_run PRIVATE FUTURE_CONFIG_STATIC_DEFINE) # static flag for the export header

target_include_directories(test_run PRIVATE ${FCONFIG_SRC_DIR} ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(test_run PRIVATE ${YAML_CPP_TARGET})
target_link_libraries(test_run PRIVATE pantor::inja)
target_link_libraries(test_run PRIVATE GTest::gtest_main)
target_link_libraries(test_run PRIVATE spdlog::spdlog)

# test data
add_custom_command(
	TARGET test_run
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/test $<TARGET_FILE_DIR:test_run>/data/test
)

# config template for the builder tool
add_custom_command(
	TARGET test_run
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/config.jinja $<TARGET_FILE_DIR:test_run>/data/config.jinja
)

# enable automatic discovery of tests
gtest_discover_tests(test_run)

# fconfig_builder tests
set(FCONFIG_EXPECTED_OUTPUT [===[]===])
add_test(
	NAME fconfig_builder_missing_config_file
	COMMAND
		${CMAKE_COMMAND}
		-D TEST_EXECUTABLE=$<TARGET_FILE:fconfig_builder>
		-D TEST_ARGS=--name\;test\;--source_dir\;src
		-D "EXPECTED_OUTPUT=PARSE ERROR:[ \t\r\n]*Required argument missing: main.*"
		-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunFailingTest.cmake
)

# Test run_fconfig_builder from setupFunctions.cmake using the usage_test project
add_test(
	NAME run_fconfig_builder_success
	COMMAND
		${CMAKE_COMMAND}
		-D USAGE_TEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/usage_test
		-D SETUP_FUNCTIONS_PATH=${PROJECT_SOURCE_DIR}/install/setupFunctions.cmake
		-D FCONFIG_BUILDER_EXECUTABLE=$<TARGET_FILE:fconfig_builder>
		-D EXPECTED_CONFIG_PATH=${CMAKE_CURRENT_SOURCE_DIR}/cmake/expected_usage_test_config.h
		-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunFconfigBuilderTest.cmake
)
set_tests_properties(run_fconfig_builder_success PROPERTIES REQUIRED_FILES $<TARGET_FILE:fconfig_builder>)

# Test generate_master_config_headers from setupFunctions.cmake (plain CMake, no project)
# Test 1: one main config file
add_test(
	NAME generate_master_config_headers_one_config
	COMMAND
		${CMAKE_COMMAND}
		-D SETUP_FUNCTIONS_PATH=${PROJECT_SOURCE_DIR}/install/setupFunctions.cmake
		-D TEST_OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/GenerateMasterConfigHeadersTest_one
		-D EXPECTED_CONFIG_PATHS_PATH=${CMAKE_CURRENT_SOURCE_DIR}/cmake/expected_config_paths.h
		-D MASTER_CONFIG_PATHS=data/config.yaml
		-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunGenerateMasterConfigHeadersTest.cmake
)

# Test 2: two main config files
add_test(
	NAME generate_master_config_headers_two_configs
	COMMAND
		${CMAKE_COMMAND}
		-D SETUP_FUNCTIONS_PATH=${PROJECT_SOURCE_DIR}/install/setupFunctions.cmake
		-D TEST_OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/GenerateMasterConfigHeadersTest_two
		-D EXPECTED_CONFIG_PATHS_PATH=${CMAKE_CURRENT_SOURCE_DIR}/cmake/expected_config_paths_two.h
		-D "MASTER_CONFIG_PATHS=data/config.yaml;data/other.yaml"
		-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/RunGenerateMasterConfigHeadersTest.cmake
)

#add_test(
#	NAME fconfig_builder_missing_config_file
#	COMMAND $<TARGET_FILE:fconfig_builder> --name "test" --source_dir "src"
#)
#set_property(TEST fconfig_builder_missing_config_file PROPERTY WILL_FAIL TRUE)
#set_property(TEST fconfig_builder_missing_config_file PROPERTY PASS_REGULAR_EXPRESSION "MISSED_CONFIG_FILE")