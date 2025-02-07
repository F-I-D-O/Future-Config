cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH USAGE_TEST_DIR)

set(CTEST_SOURCE_DIRECTORY ${FCONFIG_CPP_DIR})
set(CTEST_BINARY_DIRECTORY "${FCONFIG_CPP_DIR}/ctest-build-${FCONFIG_TEST_PLATFORM_NAME}")
set(CTEST_CONFIGURE_COMMAND "\"${CMAKE_COMMAND}\" \"${CTEST_SOURCE_DIRECTORY}\" -B \"${CTEST_BINARY_DIRECTORY}\" --toolchain \"${FCONFIG_TOOLCHAIN}\" -DCMAKE_BUILD_TYPE=Release")
set(CTEST_BUILD_COMMAND "\"${CMAKE_COMMAND}\" --build \"${CTEST_BINARY_DIRECTORY}\" --config Release")
set(CTEST_BUILD_NAME ${FCONFIG_TEST_PLATFORM_NAME})
set(CTEST_SITE "Future-Config usage test")

if(FCONFIG_TEST_CXX_COMPILER)
	set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} -DCMAKE_CXX_COMPILER=${FCONFIG_TEST_CXX_COMPILER}")
endif()

message("Deleting previous test build directory")
file(REMOVE_RECURSE "${CTEST_BINARY_DIRECTORY}")

message("Starting CDash test configuration")
ctest_start(Experimental)

message("Configuring test build")
ctest_configure()

message("Building")
ctest_build()

message("Running tests")
ctest_test()

message("Submitting results")
ctest_submit()