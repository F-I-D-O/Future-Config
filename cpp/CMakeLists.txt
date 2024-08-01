
cmake_minimum_required(VERSION 3.22)

option(VCPKG_APPLOCAL_DEPS ON)

project(fconfig)

# requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# minimum version of GCC (because it incorrectly reports C++20 support)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13.0.0)
	message(FATAL_ERROR "GCC version must be at least 13.0.0 while yours is ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# used modules
include(GNUInstallDirs) # for standard install directories
include(CMakePackageConfigHelpers) # for generating the package configuration file and version file



# path configuration
set(FCONFIG_EXPORT_NAME "${PROJECT_NAME}Targets")
set(FCONFIG_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
set(FCONFIG_CMAKE_FILES_INSTALL_DIR "share/${PROJECT_NAME}")
set(FCONFIG_PACKAGE_CONFIG_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
set(FCONFIG_VERSION_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(FCONFIG_JINJA_TEMPLATE_BUILD_PATH $<TARGET_FILE_DIR:fconfig_builder>/data/config.jinja)

# search for dependencies
find_package(yaml-cpp CONFIG REQUIRED)
find_package(inja CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


# GOOGLE TEST
# do not install gtest
set(INSTALL_GTEST OFF)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	EXCLUDE_FROM_ALL
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gtest_main PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gmock PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gmock_main PROPERTIES EXCLUDE_FROM_ALL TRUE)




add_library(future-config STATIC
	src/Builder.cpp
	src/common.cpp
	src/Config_object.cpp
	src/configuration.cpp
	src/Merger.cpp
	src/Parser.cpp
	src/Resolver.cpp
)


add_executable(fconfig_builder
	src/Builder.cpp
	src/common.cpp
	src/Merger.cpp
	src/Parser.cpp
	src/Resolver.cpp
	src/Config_object.cpp
	src/configuration.cpp
	src/builder_runner.cpp
)

## disable building of the builder tool in debug mode
## for multi-configuration generators
#if(CMAKE_CONFIGURATION_TYPES)
#	set_target_properties(MyTarget PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_DEBUG TRUE)
## for single-configuration generators
#else ()
#	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#		set_target_properties(MyTarget PROPERTIES EXCLUDE_FROM_ALL TRUE)
#	endif()
#endif()

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

# include directories

# for enabling includes relative to src root (instead of relative to the file dir)
target_include_directories(
	future-config
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${FCONFIG_INSTALL_INCLUDE_DIR}>
)

# linking
target_link_libraries(future-config PUBLIC yaml-cpp)

target_link_libraries(test_run PRIVATE yaml-cpp)
target_link_libraries(test_run PRIVATE pantor::inja)
target_link_libraries(test_run PRIVATE GTest::gtest_main)
target_link_libraries(test_run PRIVATE spdlog::spdlog)

target_link_libraries(fconfig_builder PRIVATE yaml-cpp)
target_link_libraries(fconfig_builder PRIVATE pantor::inja)
target_link_libraries(fconfig_builder PRIVATE spdlog::spdlog)

# enable automatic discovery of tests
include(GoogleTest) # cmake module
gtest_discover_tests(test_run)

# Copy shared libraries to the build directory (required by the Visual Studio generator)
if(CMAKE_GENERATOR MATCHES "Visual Studio.*")
	add_custom_command(
		TARGET fconfig_builder
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:fconfig_builder> $<TARGET_FILE_DIR:fconfig_builder>
		COMMAND_EXPAND_LISTS
	)
endif()

# RESOURCE COPYING TO BUILD DIR

# test data
add_custom_command(
	TARGET test_run
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/test $<TARGET_FILE_DIR:test_run>/data/test
)

# config template for the builder tool
add_custom_command(
	TARGET fconfig_builder
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/config.jinja ${FCONFIG_JINJA_TEMPLATE_BUILD_PATH}
)

add_custom_command(
	TARGET test_run
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/config.jinja $<TARGET_FILE_DIR:test_run>/data/config.jinja
)

# INSTALLATION
# targets installation
install(
	TARGETS future-config
	EXPORT "${FCONFIG_EXPORT_NAME}"
)

install(
	TARGETS fconfig_builder
	CONFIGURATIONS Release
)

## builder tool installation
#install(
#	TARGETS fconfig_builder
#	DESTINATION tools/${PROJECT_NAME}
#	CONFIGURATIONS Release
#)


# headers installation
install(
	FILES
		src/common.h
		src/Config_object.h
		src/configuration.h
		src/Merger.h
		src/Parser.h
		src/Resolver.h
	DESTINATION ${FCONFIG_INSTALL_INCLUDE_DIR}
	CONFIGURATIONS Release
)

# builder tool template installation
install(
	FILES ${FCONFIG_JINJA_TEMPLATE_BUILD_PATH}
	DESTINATION ${CMAKE_INSTALL_BINDIR}/data/
	CONFIGURATIONS Release
)

# cmake targets installation
install(
	EXPORT "${FCONFIG_EXPORT_NAME}"
	DESTINATION ${FCONFIG_CMAKE_FILES_INSTALL_DIR}
)

# Generate the package configuration file
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/install/fconfigConfig.cmake.in
	${FCONFIG_PACKAGE_CONFIG_BUILD_PATH}
	INSTALL_DESTINATION ${FCONFIG_CMAKE_FILES_INSTALL_DIR}
)

write_basic_package_version_file(
	${FCONFIG_VERSION_BUILD_PATH}
	VERSION "${fconfig_VERSION_MAJOR}.${fconfig_VERSION_MINOR}.${fconfig_VERSION_PATCH}"
	COMPATIBILITY AnyNewerVersion
)

# Install the package configuration file
install(FILES
	${FCONFIG_PACKAGE_CONFIG_BUILD_PATH}
	${FCONFIG_VERSION_BUILD_PATH}
	DESTINATION ${FCONFIG_CMAKE_FILES_INSTALL_DIR}
)