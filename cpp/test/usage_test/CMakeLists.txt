cmake_minimum_required(VERSION 3.22)

project(future-config-usage-test)

set(CMAKE_CXX_STANDARD 20)

include(CTest)

# set the MSVC runtime library to /MT or MTd if we link to a static library
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND NOT FCONFIG_TEST_BUILD_SHARED AND NOT VCPKG_TARGET_TRIPLET STREQUAL "x64-windows")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	message(STATUS "Setting static runtime library")
endif()


find_package(future-config CONFIG REQUIRED) # Find the future-config package

add_executable(usage_test_executable
	src/main.cpp
)

#target_include_directories(usage_test_executable PRIVATE

target_link_libraries(usage_test_executable PRIVATE future-config::future-config) # Link the library

# Copy shared libraries to the build directory (required by the Visual Studio generator)
if(CMAKE_GENERATOR MATCHES "Visual Studio.*")
	# only call the copy when linking to a shared library
	get_target_property(DLLS future-config::future-config TARGET_RUNTIME_DLLS)

	if(DLLS)
		add_custom_command(
			TARGET usage_test_executable
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:usage_test_executable> $<TARGET_FILE_DIR:usage_test_executable>
			COMMAND_EXPAND_LISTS
		)
	endif()
endif()

run_fconfig_builder()

# move the default config file to the build directory
add_custom_command(
	TARGET usage_test_executable
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/config.yaml $<TARGET_FILE_DIR:usage_test_executable>/data/config.yaml
)

# move the "custom" config file to the build directory
add_custom_command(
	TARGET usage_test_executable
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/config.yaml $<TARGET_FILE_DIR:usage_test_executable>/config.yaml
)

# add test that executes the built executable
add_test(NAME usage_test COMMAND usage_test_executable $<TARGET_FILE_DIR:usage_test_executable>/config.yaml)
set_property(TEST usage_test PROPERTY PASS_REGULAR_EXPRESSION "2")

