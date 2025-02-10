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
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	#	EXCLUDE_FROM_ALL
	DOWNLOAD_EXTRACT_TIMESTAMP ON
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
	usage_test/src/main.cpp
)

target_compile_definitions(test_run PRIVATE FUTURE_CONFIG_STATIC_DEFINE) # static flag for the export header

target_include_directories(test_run PRIVATE ${FCONFIG_SRC_DIR})

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

# SMOKE TESTS
#set(FCONFIG_TEST_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/test_build-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")
#
#add_test(NAME configurations_test COMMAND {CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/configuration_test.cmake)

if (FCONFIG_TEST_VCPKG_INSTALL)
	set(FCONFIG_PORT_DIR "${PROJECT_SOURCE_DIR}/port/future-config")

	if(FCONFIG_TEST_PLATFORM_NAME MATCHES "WSL-GCC_11")
		set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} --overlay-triplets=${FCONFIG_CPP_DIR}/test/vcpkg_gcc11")
	endif()

	add_test(NAME vcpkg_remove COMMAND vcpkg remove future-config)
	add_test(NAME vcpkg_install_test COMMAND vcpkg install future-config --overlay-ports=${FCONFIG_PORT_DIR} --binarysource=clear)

	# setup the relationship between the test and the fixture
	set_tests_properties(vcpkg_install_test PROPERTIES FIXTURES_REQUIRED vcpkg)
	set_tests_properties(vcpkg_remove PROPERTIES FIXTURES_SETUP vcpkg)
endif ()