cmake_minimum_required(VERSION 3.22)

project(future-config-usage-test)

find_package(future-config REQUIRED) # Find the future-config package

add_executable(usage_test_executable
	src/main.cpp
)

target_link_libraries(usage_test_executable PRIVATE future-config) # Link the library

# Copy shared libraries to the build directory (required by the Visual Studio generator)
if(CMAKE_GENERATOR MATCHES "Visual Studio.*")
	add_custom_command(
		TARGET usage_test_executable
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:usage_test_executable> $<TARGET_FILE_DIR:usage_test_executable>
		COMMAND_EXPAND_LISTS
	)
endif()

run_fconfig_builder()