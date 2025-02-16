cmake_minimum_required(VERSION 3.22)

project(future-config-usage-test)

set(CMAKE_CXX_STANDARD 20)

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

