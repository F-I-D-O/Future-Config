# include(GNUInstallDirs) # for standard install directories

function(get_default_master_config_path PATH_VAR_NAME)
#	cmake_parse_arguments(
#		PARSE_ARGV
#		1
#		GET_DEFAULT_MASTER_CONFIG_PATH
#		""
#		"PATH_VAR_NAME"
#		""
#	)

	set(${PATH_VAR_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/data/config.yaml" PARENT_SCOPE)
endfunction()

function(run_fconfig_builder)
	cmake_parse_arguments(
		PARSE_ARGV 0
		RUN_FCONFIG_BUILDER
		""
		"ROOT_CONFIG_CLASS_NAME;SOURCE_DIR;FCONFIG_BUILDER_EXECUTABLE"
		"MAIN_CONFIG_PATH"
	)

	# by default, we set the root config class name to the name of the project
	if(NOT DEFINED RUN_FCONFIG_BUILDER_ROOT_CONFIG_CLASS_NAME OR NOT RUN_FCONFIG_BUILDER_ROOT_CONFIG_CLASS_NAME)
		get_filename_component(RUN_FCONFIG_BUILDER_ROOT_CONFIG_CLASS_NAME "${PROJECT_NAME}" NAME)
	endif()

	# check the required arguments
	if(NOT DEFINED RUN_FCONFIG_BUILDER_SOURCE_DIR)
		message(FATAL_ERROR "You must specify the source directory where the generated config classes will be placed")
	endif()
	if(NOT DEFINED RUN_FCONFIG_BUILDER_MAIN_CONFIG_PATH)
		message(FATAL_ERROR "You must specify the main config file path")
	endif()

	# BUILDER TOOL EXECUTABLE: use provided path or search for it
	if(DEFINED RUN_FCONFIG_BUILDER_FCONFIG_BUILDER_EXECUTABLE AND RUN_FCONFIG_BUILDER_FCONFIG_BUILDER_EXECUTABLE)
		set(FCONFIG_BUILDER_EXECUTABLE "${RUN_FCONFIG_BUILDER_FCONFIG_BUILDER_EXECUTABLE}")
		if(NOT EXISTS "${FCONFIG_BUILDER_EXECUTABLE}")
			message(FATAL_ERROR "FCONFIG_BUILDER_EXECUTABLE does not exist: ${FCONFIG_BUILDER_EXECUTABLE}")
		endif()
		cmake_path(GET FCONFIG_BUILDER_EXECUTABLE PARENT_PATH FCONFIG_BUILDER_DIR)
		message(STATUS "Using fconfig_builder executable: ${FCONFIG_BUILDER_EXECUTABLE}")
	else()
		# BUILDER TOOL EXECUTABLE SEARCH
		cmake_path(GET CMAKE_INSTALL_PREFIX PARENT_PATH CMAKE_INSTALL_PARENT_DIR)
		set(FCONFIG_BUILDER_VCPKG_PATH "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/future-config")
		set(FCONFIG_BUILDER_SYSTEM_PATH "${CMAKE_INSTALL_PARENT_DIR}/future-config/bin")
		message(STATUS "looking for fconfig_builder executable at the following locations:
- ${FCONFIG_BUILDER_VCPKG_PATH}
- ${FCONFIG_BUILDER_SYSTEM_PATH}")
		find_program(FCONFIG_BUILDER_EXECUTABLE
			fconfig_builder
			REQUIRED
			HINTS
				"${FCONFIG_BUILDER_VCPKG_PATH}"	# check if fconfig_builder is installed with vcpkg
				"${FCONFIG_BUILDER_SYSTEM_PATH}"	 # check if fconfig_builder is installed in the system
		)
		message(STATUS "Future-Config Builder executable found at: ${FCONFIG_BUILDER_EXECUTABLE}")
		cmake_path(GET FCONFIG_BUILDER_EXECUTABLE PARENT_PATH FCONFIG_BUILDER_DIR)
	endif()

	# Build arguments list, adding --main for each config path
	set(FCONFIG_BUILDER_ARGS
		--name ${RUN_FCONFIG_BUILDER_ROOT_CONFIG_CLASS_NAME}
		--source_dir "${RUN_FCONFIG_BUILDER_SOURCE_DIR}"
	)
	# Add --main argument for each main config path (supports both single value and list)
	foreach(MAIN_CONFIG_PATH_ITEM IN LISTS RUN_FCONFIG_BUILDER_MAIN_CONFIG_PATH)
		list(APPEND FCONFIG_BUILDER_ARGS --main "${MAIN_CONFIG_PATH_ITEM}")
	endforeach()

	# change directory and run the fconfig_builder
	message(STATUS "Running fconfig_builder with arguments: ${FCONFIG_BUILDER_ARGS}")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E
		chdir ${FCONFIG_BUILDER_DIR}
		${FCONFIG_BUILDER_EXECUTABLE}
		${FCONFIG_BUILDER_ARGS}
		RESULT_VARIABLE FCONFIG_BUILDER_RESULT
		OUTPUT_VARIABLE FCONFIG_BUILDER_OUTPUT
		ERROR_VARIABLE FCONFIG_BUILDER_OUTPUT
	)
	if(FCONFIG_BUILDER_RESULT)
		message(
			FATAL_ERROR
			"fconfig_builder failed with code ${FCONFIG_BUILDER_RESULT}.
The path to the fconfig_builder executable is: ${FCONFIG_BUILDER_EXECUTABLE}
The arguments were: ${FCONFIG_BUILDER_ARGS}
The output was:\n${FCONFIG_BUILDER_OUTPUT}"
		)
	else()
		message(STATUS "Configuration classes generated successfully")
	endif()
endfunction()

function(copy_master_config)
	cmake_parse_arguments(
		PARSE_ARGV 0
		COPY_FCONFIG_MASTER_CONFIG
		""
		""
		"CONFIG_PATHS;TARGET_NAMES"
	)

	if(NOT DEFINED COPY_FCONFIG_MASTER_CONFIG_TARGET_NAMES)
		message(FATAL_ERROR "You must specify the targets that need the master config file")
	endif()

	foreach(CONFIG_PATH IN LISTS COPY_FCONFIG_MASTER_CONFIG_CONFIG_PATHS)
		get_filename_component(CONFIG_FILE_NAME "${CONFIG_PATH}" NAME)

		# by default, we copy the config file to the <target output directory>/data/<source_config_file_name>
		set(CONFIG_INSTALL_PATH "data/${CONFIG_FILE_NAME}")

		foreach(TARGET_NAME IN LISTS COPY_FCONFIG_MASTER_CONFIG_TARGET_NAMES)

			# set up a post build copy command for each target to copy the master config file to the target output directory
			add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${CONFIG_PATH}
				"$<TARGET_FILE_DIR:${TARGET_NAME}>/${CONFIG_INSTALL_PATH}"
				COMMENT "Copying ${CONFIG_PATH} to $<TARGET_FILE_DIR:${TARGET_NAME}>/${CONFIG_INSTALL_PATH}"
			)
		endforeach()
	endforeach()
endfunction()

# create a variable for the config file that can be used at runtime to autolocate the config file
function(generate_master_config_headers)
	cmake_parse_arguments(
		PARSE_ARGV 0
		GENERATE_MASTER_CONFIG_HEADERS
		""
		"SOURCE_DIR"
		"MASTER_CONFIG_PATHS"
	)

	if(NOT DEFINED GENERATE_MASTER_CONFIG_HEADERS_SOURCE_DIR)
		message(FATAL_ERROR "You must specify SOURCE_DIR for generate_master_config_headers")
	endif()
	if(NOT DEFINED GENERATE_MASTER_CONFIG_HEADERS_MASTER_CONFIG_PATHS OR NOT GENERATE_MASTER_CONFIG_HEADERS_MASTER_CONFIG_PATHS)
		message(FATAL_ERROR "You must specify MASTER_CONFIG_PATHS for generate_master_config_headers")
	endif()

	# join items as a comma-separated list
	string(JOIN "\",\"" FCONFIG_MASTER_CONFIG_PATHS_JOINED ${GENERATE_MASTER_CONFIG_HEADERS_MASTER_CONFIG_PATHS})

	# Template is next to this file in install/
	set(FCONFIG_CONFIG_PATHS_HEADER_TEMPLATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/config_paths_template.h.in")
	if(NOT EXISTS "${FCONFIG_CONFIG_PATHS_HEADER_TEMPLATE}")
		message(FATAL_ERROR "config_paths_template.h.in not found at ${FCONFIG_CONFIG_PATHS_HEADER_TEMPLATE}")
	endif()
	configure_file(
		"${FCONFIG_CONFIG_PATHS_HEADER_TEMPLATE}"
		"${GENERATE_MASTER_CONFIG_HEADERS_SOURCE_DIR}/config/config_paths.h"
		@ONLY
	)
endfunction()

#[===============================================================================================================[.rst:
setup_fconfig
-------------
Main setup function. For most use cases, this is the only function you need to call in your CMakeLists.txt

Synopsis
^^^^^^^^

.. code-block:: cmake

	setup_fconfig(
		TARGET_NAMES <target1> [<target2> ...]
		[SOURCE_DIR <path>]
		[ROOT_CONFIG_CLASS_NAME <name>]
		[MAIN_CONFIG_PATHS <path1> [<path2> ...]]
		[FCONFIG_BUILDER_EXECUTABLE <path>]
	)

Arguments
^^^^^^^^^

``TARGET_NAMES <target1> [<target2> ...]``
  The list of targets for which the master config file should be applied

``SOURCE_DIR <path>`` 
  Root source directory. By default, it is set to ``<CMakeLists.txt directory>/src``. The generated config classes will
  be placed in `SOURCE_DIR/config`. This directory must be in include directories of the targets using future-config.

``ROOT_CONFIG_CLASS_NAME <name>``
  The name of the root config class. By default, it is set to `<project name>_config`.

``MAIN_CONFIG_PATHS <path1>[ <path2> ...]``
  The paths to the main config files. If omitted, a single main config file is considered:
  ``<CMakeLists.txt directory>/data/config.yaml``.

``FCONFIG_BUILDER_EXECUTABLE <path>``
  The path to the fconfig_builder executable. By default, a search is performed to find the executable.

#]===============================================================================================================]
function(setup_fconfig)
	cmake_parse_arguments(
		PARSE_ARGV 0
		SETUP_FCONFIG
		""
		"SOURCE_DIR;ROOT_CONFIG_CLASS_NAME;CONFIG_INSTALL_PATH;FCONFIG_BUILDER_EXECUTABLE"
		"MAIN_CONFIG_PATHS;TARGET_NAMES"
	)

	# by default, we look for the main config file in the root/data directory
	if(NOT DEFINED SETUP_FCONFIG_MAIN_CONFIG_PATHS)
		get_default_master_config_path(SETUP_FCONFIG_MAIN_CONFIG_PATHS)
	endif()

	# by default, we set the <CmakeLists.txt directory>/src as the source directory
	if(NOT DEFINED SETUP_FCONFIG_SOURCE_DIR)
		set(SETUP_FCONFIG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
	endif()

	set(RUN_FCONFIG_BUILDER_ARGS
		ROOT_CONFIG_CLASS_NAME "${SETUP_FCONFIG_ROOT_CONFIG_CLASS_NAME}"
		SOURCE_DIR "${SETUP_FCONFIG_SOURCE_DIR}"
		MAIN_CONFIG_PATH "${SETUP_FCONFIG_MAIN_CONFIG_PATHS}"
	)
	if(DEFINED SETUP_FCONFIG_FCONFIG_BUILDER_EXECUTABLE AND SETUP_FCONFIG_FCONFIG_BUILDER_EXECUTABLE)
		list(APPEND RUN_FCONFIG_BUILDER_ARGS FCONFIG_BUILDER_EXECUTABLE "${SETUP_FCONFIG_FCONFIG_BUILDER_EXECUTABLE}")
	endif()
	run_fconfig_builder(${RUN_FCONFIG_BUILDER_ARGS})

	copy_master_config(
		CONFIG_PATHS ${SETUP_FCONFIG_MAIN_CONFIG_PATHS}
		TARGET_NAMES ${SETUP_FCONFIG_TARGET_NAMES}
	)

	# Compute MASTER_CONFIG_PATHS for the header relative to the runtime data resource root.
	set(_MASTER_CONFIG_PATHS_FOR_HEADER "")
	foreach(_CONFIG_PATH IN LISTS SETUP_FCONFIG_MAIN_CONFIG_PATHS)
		get_filename_component(_CONFIG_FILE_NAME "${_CONFIG_PATH}" NAME)
		list(APPEND _MASTER_CONFIG_PATHS_FOR_HEADER "${_CONFIG_FILE_NAME}")
	endforeach()
	generate_master_config_headers(
		SOURCE_DIR "${SETUP_FCONFIG_SOURCE_DIR}"
		MASTER_CONFIG_PATHS ${_MASTER_CONFIG_PATHS_FOR_HEADER}
	)

endfunction()
