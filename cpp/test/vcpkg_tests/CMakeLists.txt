
cmake_minimum_required(VERSION 3.22)

project(future-config-vcpgk-tests)

enable_testing()

# ensure the triplet is set
if(NOT DEFINED FCONFIG_VCPKG_TEST_TRIPLET)
	message(FATAL_ERROR "FCONFIG_VCPKG_TEST_TRIPLET must be set")
endif()

set(FCONFIG_PORT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../port")
set(FCONFIG_TEST_VCPKG_INSTALL_COMMAND vcpkg install future-config
	--overlay-ports=${FCONFIG_PORT_DIR}
	--binarysource=clear
	--triplet=${FCONFIG_VCPKG_TEST_TRIPLET}
)
set(FCONFIG_TEST_VCPKG_REMOVE_COMMAND vcpkg remove future-config
	--triplet ${FCONFIG_VCPKG_TEST_TRIPLET}
)
if(FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR)
	list(APPEND FCONFIG_TEST_VCPKG_INSTALL_COMMAND "--overlay-triplets=${FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR}")
	list(APPEND FCONFIG_TEST_VCPKG_REMOVE_COMMAND "--overlay-triplets=${FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR}")
endif()

add_test(NAME vcpkg_remove COMMAND ${FCONFIG_TEST_VCPKG_REMOVE_COMMAND})
add_test(NAME vcpkg_install_test COMMAND ${FCONFIG_TEST_VCPKG_INSTALL_COMMAND})

# setup the relationship between the test and the fixture
set_tests_properties(vcpkg_install_test PROPERTIES FIXTURES_REQUIRED vcpkg)
set_tests_properties(vcpkg_remove PROPERTIES FIXTURES_SETUP vcpkg)


