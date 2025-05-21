
cmake_minimum_required(VERSION 3.22)

option(FCONFIG_BUILD_SHARED_LIBS "If ON, shared libraries will be used during installation. Otherwise, static libraries are build" OFF)
option(FCONFIG_ENABLE_TESTS "Enables testing and builds tests" ON)
option(FCONFIG_BUILDER_COPY_LIBRARY_DEPENDENCIES_MANUALLY "If ON, the library dependencies of the builder tool will be copied manually to the bin directory" ON)
option(FCONFIG_INSTALL_BUILDER_TOOL_AND_HEADERS_DEBUG "If ON, the builder tool and headers will be installed even in debug mode" ON)

project(future-config)

# requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# used modules
include(GNUInstallDirs) # for standard install directories
include(CMakePackageConfigHelpers) # for generating the package configuration file and version file
#include(GenerateExportHeader) # for generating the export headers
include(CTest) # for testing

# set the MSVC runtime library to /MT or MTd if appropriate
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"
	AND NOT FCONFIG_BUILD_SHARED_LIBS
	AND (NOT DEFINED VCPKG_CRT_LINKAGE OR VCPKG_CRT_LINKAGE STREQUAL "static")
)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	message(STATUS "Setting static runtime library")
endif()

# copy runtime dependencies to the build directory using some vcpkg magic
set(VCPKG_APPLOCAL_DEPS ON)

# minimum version of GCC (because it incorrectly reports C++20 support)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13.0.0)
	set(FCONFIG_USE_FMT ON)
	add_compile_definitions(FCONFIG_USE_FMT)
	#	message(FATAL_ERROR "GCC version must be at least 13.0.0 while yours is ${CMAKE_CXX_COMPILER_VERSION}")
endif()


# path configuration
set(FCONFIG_EXPORT_NAME "${PROJECT_NAME}Targets")
set(FCONFIG_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
set(FCONFIG_CMAKE_FILES_INSTALL_DIR "share/${PROJECT_NAME}")
set(FCONFIG_PACKAGE_CONFIG_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
set(FCONFIG_VERSION_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(FCONFIG_JINJA_TEMPLATE_BUILD_PATH $<TARGET_FILE_DIR:fconfig_builder>/data/config.jinja)

# DEPENDENCIES
# packages
find_package(yaml-cpp CONFIG REQUIRED)
find_package(inja CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
if(FCONFIG_USE_FMT)
	find_package(fmt CONFIG REQUIRED)
endif()
# headers for header-only libs
find_path(TCLAP_INCLUDE_DIRS "tclap/" REQUIRED) # used for command line parsing in the builder tool

set(future_config_lib_files
	src/Builder.cpp
	src/Command_line_parser.cpp
	src/common.cpp
	src/Config_object.cpp
	src/configuration.cpp
	src/Merger.cpp
	src/Parser.cpp
	src/Resolver.cpp
	src/resources.cpp
)

if (NOT DEFINED CMAKE_DEBUG_POSTFIX)
	set(CMAKE_DEBUG_POSTFIX "d")
endif()

add_library(future-config SHARED ${future_config_lib_files})
#generate_export_header(future-config)
set_target_properties(
	future-config
	PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
)

add_library(future-config-static STATIC ${future_config_lib_files})
target_compile_definitions(future-config-static PUBLIC FUTURE_CONFIG_STATIC_DEFINE) # static flag for the export header
set_target_properties(
	future-config-static
	PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
)

add_executable(fconfig_builder
	src/Builder.cpp
	src/common.cpp
	src/Merger.cpp
	src/Parser.cpp
	src/Resolver.cpp
	src/Config_object.cpp
	src/builder_runner.cpp
	src/resources.cpp
)
set_target_properties(
	fconfig_builder
	PROPERTIES COMPILE_FLAGS -DFUTURE_CONFIG_STATIC_DEFINE
)

# INCLUDE DIRECTORIES

# public API headers: a different include directory is used for the installation
target_include_directories(
	future-config
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(
	future-config-static
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# private headers: only need to be included in during the build
target_include_directories(
	future-config
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_include_directories(
	future-config-static
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# builder tool includes
target_include_directories(
	fconfig_builder
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# header-only libraries include directories
target_include_directories(fconfig_builder PRIVATE ${TCLAP_INCLUDE_DIRS})


# for enabling includes relative to src root (instead of relative to the file dir)

# LINKING
#message(STATUS "yaml-cpp version: ${yaml-cpp_VERSION}")
if(${yaml-cpp_VERSION} VERSION_LESS 0.8.0)
	set(YAML_CPP_TARGET yaml-cpp)
else()
	set(YAML_CPP_TARGET yaml-cpp::yaml-cpp)
endif()
target_link_libraries(future-config PUBLIC ${YAML_CPP_TARGET})
target_link_libraries(future-config-static PUBLIC ${YAML_CPP_TARGET})

# we need to expose the spdlog headers as they are used in the public API
target_link_libraries(future-config PRIVATE spdlog::spdlog)
target_link_libraries(future-config-static PRIVATE spdlog::spdlog)
if(FCONFIG_USE_FMT)
	target_link_libraries(future-config PRIVATE fmt::fmt)
	target_link_libraries(future-config-static PRIVATE fmt::fmt)
endif()

target_link_libraries(fconfig_builder PRIVATE ${YAML_CPP_TARGET})
target_link_libraries(fconfig_builder PRIVATE pantor::inja)
target_link_libraries(fconfig_builder PRIVATE spdlog::spdlog)
if(FCONFIG_USE_FMT)
	target_link_libraries(fconfig_builder PRIVATE fmt::fmt)
endif()

## Copy shared libraries to the build directory (required by the Visual Studio generator)
#if(CMAKE_GENERATOR MATCHES "Visual Studio.*")
#	add_custom_command(
#		TARGET fconfig_builder
#		POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:fconfig_builder> $<TARGET_FILE_DIR:fconfig_builder>
#		COMMAND_EXPAND_LISTS
#	)
#endif()


# RESOURCE COPYING TO BUILD DIR

# config template for the builder tool
add_custom_command(
	TARGET fconfig_builder
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/config.jinja ${FCONFIG_JINJA_TEMPLATE_BUILD_PATH}
)


# INSTALLATION
if(NOT FCONFIG_INSTALL_BUILDER_TOOL_AND_HEADERS_DEBUG)
	set(FCONFIG_BUILDER_TOOL_AND_HEADERS_INSTALL_CONFIGURATIONS CONFIGURATIONS Release)
endif()

# targets installation
if(FCONFIG_BUILD_SHARED_LIBS)
	set_target_properties(future-config-static PROPERTIES EXCLUDE_FROM_ALL TRUE)
	install(TARGETS future-config EXPORT "${FCONFIG_EXPORT_NAME}")
else()
	set_target_properties(future-config PROPERTIES EXCLUDE_FROM_ALL TRUE)
	set_target_properties(future-config-static PROPERTIES EXPORT_NAME "future-config")
	install(TARGETS future-config-static EXPORT "${FCONFIG_EXPORT_NAME}")
endif()

install(TARGETS fconfig_builder ${FCONFIG_BUILDER_TOOL_AND_HEADERS_INSTALL_CONFIGURATIONS})

# headers installation
install(
	FILES
		include/future-config/common.h
		include/future-config/Config_object.h
		include/future-config/configuration.h
		include/future-config/format.h
		include/future-config/future-config_export.h
		include/future-config/resources.h
	DESTINATION ${FCONFIG_INSTALL_INCLUDE_DIR}
	${FCONFIG_BUILDER_TOOL_AND_HEADERS_INSTALL_CONFIGURATIONS}
)

# builder tool template installation
install(
	FILES ${FCONFIG_JINJA_TEMPLATE_BUILD_PATH}
	DESTINATION ${CMAKE_INSTALL_BINDIR}/data/
	${FCONFIG_BUILDER_TOOL_AND_HEADERS_INSTALL_CONFIGURATIONS}
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
	${FCONFIG_BUILDER_TOOL_AND_HEADERS_INSTALL_CONFIGURATIONS}
)

# install the builder tool runtime dependencies if the installation is not handled by vcpkg
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND FCONFIG_BUILDER_COPY_LIBRARY_DEPENDENCIES_MANUALLY)
	message(STATUS "Copying runtime dependencies. CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")
	install(FILES $<TARGET_RUNTIME_DLLS:fconfig_builder> DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(FCONFIG_ENABLE_TESTS)
	add_subdirectory(test)
endif()
