
cmake_minimum_required(VERSION 3.22)

option(VCPKG_APPLOCAL_DEPS ON)
option(FCONFIG_BUILD_SHARED_LIBS "If ON, shared libraries will be used during installation. Otherwise, static libraries are build" OFF)
option(FCONFIG_ENABLE_TESTS "Enables testing and builds tests" ON)

project(future-config)

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
#include(GenerateExportHeader) # for generating the export headers



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

#message(STATUS "yaml-cpp version: ${yaml-cpp_VERSION}")
if(${yaml-cpp_VERSION} VERSION_LESS 0.8.0)
	set(YAML_CPP_TARGET yaml-cpp)
else()
	set(YAML_CPP_TARGET yaml-cpp::yaml-cpp)
endif()

add_library(future-config SHARED ${future_config_lib_files})
#generate_export_header(future-config)

add_library(future-config-static STATIC ${future_config_lib_files})
target_compile_definitions(future-config-static PUBLIC FUTURE_CONFIG_STATIC_DEFINE) # static flag for the export header
#set_target_properties(
#	future-config-static
#	PROPERTIES COMPILE_FLAGS -DFUTURE_CONFIG_STATIC_DEFINE
#)

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
set_target_properties(
	fconfig_builder
	PROPERTIES COMPILE_FLAGS -DFUTURE_CONFIG_STATIC_DEFINE
)

# include directories

# for enabling includes relative to src root (instead of relative to the file dir)
target_include_directories(
	future-config
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${FCONFIG_INSTALL_INCLUDE_DIR}>
)
target_include_directories(
	future-config-static
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${FCONFIG_INSTALL_INCLUDE_DIR}>
)

# linking
target_link_libraries(future-config PRIVATE ${YAML_CPP_TARGET})
target_link_libraries(future-config-static PRIVATE ${YAML_CPP_TARGET})
target_link_libraries(future-config PRIVATE spdlog::spdlog)
target_link_libraries(future-config-static PRIVATE spdlog::spdlog)

target_link_libraries(fconfig_builder PRIVATE ${YAML_CPP_TARGET})
target_link_libraries(fconfig_builder PRIVATE pantor::inja)
target_link_libraries(fconfig_builder PRIVATE spdlog::spdlog)

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

# config template for the builder tool
add_custom_command(
	TARGET fconfig_builder
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/config.jinja ${FCONFIG_JINJA_TEMPLATE_BUILD_PATH}
)


# INSTALLATION
# targets installation
if(FCONFIG_BUILD_SHARED_LIBS)
	set_target_properties(future-config-static PROPERTIES EXCLUDE_FROM_ALL TRUE)
	install(TARGETS future-config EXPORT "${FCONFIG_EXPORT_NAME}")
else()
	set_target_properties(future-config PROPERTIES EXCLUDE_FROM_ALL TRUE)
	set_target_properties(future-config-static PROPERTIES EXPORT_NAME "future-config")
	install(TARGETS future-config-static EXPORT "${FCONFIG_EXPORT_NAME}")
endif()

install(
	TARGETS fconfig_builder
	CONFIGURATIONS Release
)

# headers installation
install(
	FILES
		src/common.h
		src/Config_object.h
		src/configuration.h
		src/future-config_export.h
		src/Merger.h
		src/Parser.h
		src/Resolver.h
	DESTINATION ${FCONFIG_INSTALL_INCLUDE_DIR}
	CONFIGURATIONS Release
)
## export header installation
#install(
#	FILES ${PROJECT_BINARY_DIR}/future-config_export.h
#	DESTINATION ${FCONFIG_INSTALL_INCLUDE_DIR}
#	CONFIGURATIONS Release
#)

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
	NAMESPACE future-config::
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

# Install the fcconfig_builder run script
set(FCONFIG_BUILDER_RUN_SCRIPT_NAME "builderRunner.cmake")
install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/install/${FCONFIG_BUILDER_RUN_SCRIPT_NAME}
	DESTINATION ${FCONFIG_CMAKE_FILES_INSTALL_DIR}
	CONFIGURATIONS Release
)

if(FCONFIG_ENABLE_TESTS)
	add_subdirectory(test)
endif()