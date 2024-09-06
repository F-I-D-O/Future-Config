# GOOGLE TEST
# do not install gtest
set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	EXCLUDE_FROM_ALL
)
# For Windows: Prevent overriding the parent project's compiler/linker settings

FetchContent_MakeAvailable(googletest)
# disable building google test targets by default
set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gtest_main PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gmock PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gmock_main PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(future_config_lib_files
	src/Builder.cpp
	src/common.cpp
	src/Config_object.cpp
	src/configuration.cpp
	src/Merger.cpp
	src/Parser.cpp
	src/Resolver.cpp
)

# enable automatic discovery of tests
include(GoogleTest) # cmake module
gtest_discover_tests(test_run)



# MY TEST TARGET
add_executable(test_run EXCLUDE_FROM_ALL
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