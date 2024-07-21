
#include(GNUInstallDirs) # for standard install directories

#vcpkg_from_github(
#	OUT_SOURCE_PATH SOURCE_PATH
#	REPO F-I-D-O/Future-Config
#	REF master
#	SHA512 ab8cc92b3f0b2344c8e22f6d9522f64dd00c556f1cd5a8a139a1981d227bc371550de695c0a1ccd93e13526c9a74e69a0c8927faf5436f965147a822354a6fa7
#)
#set(SOURCE_PATH "D:/Workspaces/Fido/Future-Config")
set(SOURCE_PATH "C:/Workspaces/ninja/Future-Config")
set(PACKAGE_NAME "fconfig")

vcpkg_cmake_configure(
	SOURCE_PATH ${SOURCE_PATH}/cpp
)

vcpkg_cmake_install(
#	ADD_BIN_TO_PATH
)
#message(STATUS "setting CONFIG_PATH to ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}")
vcpkg_cmake_config_fixup(
#	PACKAGE_NAME ${PACKAGE_NAME}
#	CONFIG_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}
)

