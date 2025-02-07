
project(future-config-usage-test)

find_package(future-config REQUIRED) # Find the future-config package

add_executable(usage_test_executable
	# Add your source files here
)

target_link_libraries(usage_test_executable PRIVATE future-config) # Link the library

run_fconfig_builder()