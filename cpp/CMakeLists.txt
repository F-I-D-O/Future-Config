cmake_minimum_required(VERSION 3.22)

option(VCPKG_APPLOCAL_DEPS ON)

project(FutureConfig)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# search for dependencies
find_package(yaml-cpp CONFIG REQUIRED)
find_package(inja CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


# GOOGLE TEST
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(src) # for including files relative to src dir for the compilation of all targets


add_library(future-config STATIC
	src/Builder.cpp
	src/common.cpp
	src/Config_object.cpp
	src/configuration.cpp
	src/Merger.cpp
	src/Parser.cpp
	src/Resolver.cpp
)

add_executable(fconfig_builder
	src/Builder.cpp
	src/common.cpp
	src/Merger.cpp
	src/Parser.cpp
	src/Resolver.cpp
	src/Config_object.cpp
	src/configuration.cpp
	src/builder_runner.cpp
)

add_executable(test_run
	src/Builder.cpp
	src/common.cpp
	src/Merger.cpp
	src/Parser.cpp
	src/Resolver.cpp
	test/bulider_tests.cpp
	test/merger_test.cpp
	test/resolver_tests.cpp
	src/Config_object.cpp
	test/common.cpp
)

# include directories for linking
target_include_directories(future-config PUBLIC src)

# linking
target_link_libraries(future-config PUBLIC yaml-cpp)

target_link_libraries(test_run PRIVATE yaml-cpp)
target_link_libraries(test_run PRIVATE pantor::inja)
target_link_libraries(test_run PRIVATE GTest::gtest_main)
target_link_libraries(test_run PRIVATE spdlog::spdlog)

target_link_libraries(fconfig_builder PRIVATE yaml-cpp)
target_link_libraries(fconfig_builder PRIVATE pantor::inja)
target_link_libraries(fconfig_builder PRIVATE spdlog::spdlog)

# enable automatic discovery of tests
include(GoogleTest) # cmake module
gtest_discover_tests(test_run)



# RESOURCE COPYING TO BUILD DIR

# test data
add_custom_command(
	TARGET test_run
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/test $<TARGET_FILE_DIR:test_run>/data/test
)

# config template
add_custom_command(
	TARGET fconfig_builder
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/config.jinja $<TARGET_FILE_DIR:test_run>/data/config.jinja
)

add_custom_command(
	TARGET test_run
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/config.jinja $<TARGET_FILE_DIR:test_run>/data/config.jinja
)