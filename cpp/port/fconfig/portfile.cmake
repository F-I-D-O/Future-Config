
#include(GNUInstallDirs) # for standard install directories

#vcpkg_from_github(
#	OUT_SOURCE_PATH SOURCE_PATH
#	REPO F-I-D-O/Future-Config
#	REF master
#	SHA512 ab8cc92b3f0b2344c8e22f6d9522f64dd00c556f1cd5a8a139a1981d227bc371550de695c0a1ccd93e13526c9a74e69a0c8927faf5436f965147a822354a6fa7
#)
set(SOURCE_PATH "D:/Workspaces/Fido/Future-Config")
#set(SOURCE_PATH "C:/Workspaces/ninja/Future-Config")
set(PACKAGE_NAME "fconfig")

vcpkg_cmake_configure(
	SOURCE_PATH ${SOURCE_PATH}/cpp
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup()

# Copy the builder tool dependencies
vcpkg_copy_tools(
	TOOL_NAMES fconfig_builder
	AUTO_CLEAN
)

# move the jinja template for the builder tool from bin directory to tools directory
set(BIN_DATA_DIR "${CURRENT_PACKAGES_DIR}/bin/data")
set(PORT_TOOL_DATA_DIR "${CURRENT_PACKAGES_DIR}/tools/${PORT}/data")
file(MAKE_DIRECTORY "${PORT_TOOL_DATA_DIR}")
file(RENAME "${BIN_DATA_DIR}/config.jinja" "${PORT_TOOL_DATA_DIR}/config.jinja")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin")

file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)

