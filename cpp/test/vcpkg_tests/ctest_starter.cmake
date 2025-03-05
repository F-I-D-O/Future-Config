set(CTEST_SOURCE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
set(CTEST_BINARY_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/ctest-build-${FCONFIG_VCPKG_TEST_TRIPLET}")
set(CTEST_CONFIGURE_COMMAND
	"\"${CMAKE_COMMAND}\""
	"\"${CTEST_SOURCE_DIRECTORY}\""
	-B "\"${CTEST_BINARY_DIRECTORY}\""
	-D FCONFIG_VCPKG_TEST_TRIPLET=${FCONFIG_VCPKG_TEST_TRIPLET}
)
if(FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR)
	list(APPEND CTEST_CONFIGURE_COMMAND -D FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR=${FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR})
endif()
list(JOIN CTEST_CONFIGURE_COMMAND " " CTEST_CONFIGURE_COMMAND)
set(CTEST_SITE "Future-Config vcpkg test")
set(CTEST_BUILD_NAME ${FCONFIG_VCPKG_TEST_TRIPLET})


message("Deleting previous test build directory")
file(REMOVE_RECURSE "${CTEST_BINARY_DIRECTORY}")

message("Starting CDash test configuration")
ctest_start(Experimental)

message("Configuring test build")
ctest_configure()

message("Running tests")
ctest_test()

message("Submitting results")
ctest_submit()