set(FCONFIG_USAGE_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CTEST_SOURCE_DIRECTORY ${FCONFIG_USAGE_TEST_DIR})
set(CTEST_BINARY_DIRECTORY "${FCONFIG_USAGE_TEST_DIR}/ctest-build-${FCONFIG_TEST_PLATFORM_NAME}")
set(CTEST_CONFIGURE_COMMAND "\"${CMAKE_COMMAND}\" \"${CTEST_SOURCE_DIRECTORY}\" -B \"${CTEST_BINARY_DIRECTORY}\" --toolchain \"${FCONFIG_TOOLCHAIN}\" -D CMAKE_BUILD_TYPE=${FCONFIG_CTEST_CONFIGURATION} -D VCPKG_TARGET_TRIPLET=${FCONFIG_VCPKG_TRIPLET} -D FCONFIG_TEST_BUILD_SHARED=${FCONFIG_TEST_BUILD_SHARED}")
# append the x64 flag only if the platform is windows
if(FCONFIG_TEST_PLATFORM_NAME MATCHES "Windows.*")
	string(APPEND CTEST_CONFIGURE_COMMAND " -A x64")
endif()

set(CTEST_BUILD_COMMAND "\"${CMAKE_COMMAND}\" --build \"${CTEST_BINARY_DIRECTORY}\" --config ${FCONFIG_CTEST_CONFIGURATION}")
set(CTEST_BUILD_NAME ${FCONFIG_TEST_PLATFORM_NAME})
set(CTEST_SITE "Future-Config usage test")
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH FCONFIG_TEST_DIR)
cmake_path(GET FCONFIG_TEST_DIR PARENT_PATH FCONFIG_CPP_DIR)
set(FCONFIG_PORT_DIR "${FCONFIG_CPP_DIR}/port")

if(FCONFIG_TEST_CXX_COMPILER)
	set(CTEST_CONFIGURE_COMMAND "${CTEST_CONFIGURE_COMMAND} -DCMAKE_CXX_COMPILER=${FCONFIG_TEST_CXX_COMPILER}")
endif()

message("Removing future-config from vcpkg")
# vcpkg remove future-config
execute_process(COMMAND vcpkg remove future-config --triplet ${FCONFIG_VCPKG_TRIPLET})

message("Deleting previous test build directory")
file(REMOVE_RECURSE "${CTEST_BINARY_DIRECTORY}")

if(FCONFIG_VCPKG_INSTALL)
	# vcpkg install future-config
	execute_process(COMMAND vcpkg install future-config --triplet ${FCONFIG_VCPKG_TRIPLET} --overlay-ports=${FCONFIG_PORT_DIR} --binarysource=clear)
else()
	#install future-config using cmake
	set(FCONFIG_BUILD_DIR_FOR_INSTALL "${CTEST_BINARY_DIRECTORY}/future-config-install")

	# 1. create a build directory
	file(MAKE_DIRECTORY "${FCONFIG_BUILD_DIR_FOR_INSTALL}")

	# 2. configure
	set(FCONFIG_CMAKE_INSTALL_CONFIGURE_COMMAND
		${CMAKE_COMMAND}
		${FCONFIG_CPP_DIR}
		-B ${FCONFIG_BUILD_DIR_FOR_INSTALL}
		--toolchain ${FCONFIG_TOOLCHAIN}
		-D CMAKE_BUILD_TYPE=${FCONFIG_CTEST_CONFIGURATION}
		-D VCPKG_TARGET_TRIPLET=${FCONFIG_VCPKG_TRIPLET}
		-D FCONFIG_BUILD_SHARED_LIBS=${FCONFIG_TEST_BUILD_SHARED}
		-D FCONFIG_ENABLE_TESTS=OFF
	)

	# append the x64 flag only if the platform is windows
	if(FCONFIG_VCPKG_TRIPLET MATCHES "x64-windows*")
		list(APPEND FCONFIG_CMAKE_INSTALL_CONFIGURE_COMMAND -A x64)
	endif()

	string(JOIN " " FCONFIG_CMAKE_INSTALL_CONFIGURE_COMMAND_STR ${FCONFIG_CMAKE_INSTALL_CONFIGURE_COMMAND})
	message("Configuring future-config using: ${FCONFIG_CMAKE_INSTALL_CONFIGURE_COMMAND_STR}")
	execute_process(COMMAND ${FCONFIG_CMAKE_INSTALL_CONFIGURE_COMMAND} COMMAND_ERROR_IS_FATAL ANY)
#	execute_process(COMMAND "${CMAKE_COMMAND}" "${CTEST_SOURCE_DIRECTORY}../../../" -B "${FCONFIG_BUILD_DIR_FOR_INSTALL}" --toolchain "${FCONFIG_TOOLCHAIN}" -D CMAKE_BUILD_TYPE=Release -D VCPKG_TARGET_TRIPLET=${FCONFIG_VCPKG_TRIPLET} -D FCONFIG_BUILD_SHARED_LIBS=${FCONFIG_TEST_BUILD_SHARED} -A x64)

	# 3. install
	set(FCONFIG_CMAKE_INSTALL_INSTALL_COMMAND
		${CMAKE_COMMAND}
		--build ${FCONFIG_BUILD_DIR_FOR_INSTALL}
		--config ${FCONFIG_CTEST_CONFIGURATION}
		--target install
	)
	string(JOIN " " FCONFIG_CMAKE_INSTALL_INSTALL_COMMAND_STR ${FCONFIG_CMAKE_INSTALL_INSTALL_COMMAND})
	message("Installing future-config using: ${FCONFIG_CMAKE_INSTALL_INSTALL_COMMAND_STR}")
	execute_process(COMMAND ${FCONFIG_CMAKE_INSTALL_INSTALL_COMMAND} COMMAND_ERROR_IS_FATAL ANY)
endif()

message("Deleting the generated configuration files from previous tests")
file(REMOVE_RECURSE "${CTEST_SOURCE_DIRECTORY}/src/config")

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