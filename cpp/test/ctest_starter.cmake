cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH FCONFIG_CPP_DIR)

# Configuration type for configuration and build steps
if(NOT FCONFIG_CTEST_CONFIGURATION)
  SET(FCONFIG_CTEST_CONFIGURATION Release)
endif()

set(CTEST_SOURCE_DIRECTORY ${FCONFIG_CPP_DIR})
set(CTEST_BINARY_DIRECTORY "${FCONFIG_CPP_DIR}/ctest-build-${FCONFIG_TEST_PLATFORM_NAME}")
set(CTEST_CONFIGURE_COMMAND "\"${CMAKE_COMMAND}\" \"${CTEST_SOURCE_DIRECTORY}\" -B \"${CTEST_BINARY_DIRECTORY}\" --toolchain \"${FCONFIG_TOOLCHAIN}\" -DCMAKE_BUILD_TYPE=${FCONFIG_CTEST_CONFIGURATION}")
set(CTEST_BUILD_COMMAND "\"${CMAKE_COMMAND}\" --build \"${CTEST_BINARY_DIRECTORY}\" --config ${FCONFIG_CTEST_CONFIGURATION}")
set(CTEST_BUILD_NAME ${FCONFIG_TEST_PLATFORM_NAME})
set(CTEST_SITE "Future-Config local test")

if(FCONFIG_TEST_CXX_COMPILER)
	set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} -DCMAKE_CXX_COMPILER=${FCONFIG_TEST_CXX_COMPILER}")
endif()

if(FCONFIG_TEST_BUILD_SHARED)
	set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} -D FCONFIG_BUILD_SHARED_LIBS=ON")
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