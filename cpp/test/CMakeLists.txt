# GOOGLE TEST
include(GoogleTest) # cmake module

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
	${FCONFIG_SRC_DIR}/common.cpp
	${FCONFIG_SRC_DIR}/Config_object.cpp
	${FCONFIG_SRC_DIR}/Merger.cpp
	${FCONFIG_SRC_DIR}/Parser.cpp
	${FCONFIG_SRC_DIR}/Resolver.cpp
	${FCONFIG_SRC_DIR}/resources.cpp
	bulider_tests.cpp
	common.cpp
	merger_test.cpp
	resolver_tests.cpp
	config_object_test.cpp
)

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