include(GNUInstallDirs) # for standard install directories

function(run_fconfig_builder)
	cmake_parse_arguments(
		PARSE_ARGV
		0
		RUN_FCONFIG_BUILDER
		""
		"MAIN_CONFIG_PATH;ROOT_CONFIG_CLASS_NAME;SOURCE_DIR"
		""
	)

	# by default, we look for the main config file in the CMakeLists.txt directory
	if(NOT DEFINED RUN_FCONFIG_BUILDER_MAIN_CONFIG_PATH)
		set(RUN_FCONFIG_BUILDER_MAIN_CONFIG_PATH "${CMAKE_SOURCE_DIR}/data/config.yaml")
	endif()

	# by default, we set the root config class name to the name of the project
	if(NOT DEFINED RUN_FCONFIG_BUILDER_ROOT_CONFIG_CLASS_NAME)
		get_filename_component(RUN_FCONFIG_BUILDER_ROOT_CONFIG_CLASS_NAME "${CMAKE_PROJECT_NAME}" NAME)
	endif()

	# by default, we set the <CmakeLists.txt directory>/src as the source directory
	if(NOT DEFINED RUN_FCONFIG_BUILDER_SOURCE_DIR)
		set(RUN_FCONFIG_BUILDER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
	endif()

	# BUILDER TOOL EXECUTABLE SEARCH
	cmake_path(GET CMAKE_INSTALL_PREFIX PARENT_PATH CMAKE_INSTALL_PARENT_DIR)
	find_program(FCONFIG_BUILDER_EXECUTABLE
		fconfig_builder
		REQUIRED
		HINTS
			"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/future-config" # check if fconfig_builder is installed with vcpkg
			"${CMAKE_INSTALL_PARENT_DIR}/future-config/bin" # check if fconfig_builder is installed in the system
	)
	message(STATUS "Future-Config Builder executable found at: ${FCONFIG_BUILDER_EXECUTABLE}")
	cmake_path(GET FCONFIG_BUILDER_EXECUTABLE PARENT_PATH FCONFIG_BUILDER_DIR)
#	# check if fconfig_builder is installed with vcpkg
#	message(STATUS "FCONFIG_BUILD_DIR: ${CMAKE_INSTALL_PREFIX}/future-config/bin/fconfig_builder")
#	set(FCONFIG_BUILDER_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/future-config")
#	if(EXISTS "${FCONFIG_BUILDER_DIR}")
#		message(STATUS "Found fconfig_builder installed with vcpkg")
#	else()
#		# check if fconfig_builder is installed in the system
#		cmake_path(GET CMAKE_INSTALL_PREFIX PARENT_PATH CMAKE_INSTALL_PARENT_DIR)
#		set(FCONFIG_BUILDER_DIR "${CMAKE_INSTALL_PARENT_DIR}/future-config/bin")
#
#		if(EXISTS "${FCONFIG_BUILDER_DIR}")
#			message(STATUS "Found fconfig_builder installed in the system")
#		else()
#			set(FCONFIG_BUILDER_DIR "${CMAKE_INSTALL_FULL_BINDIR}")
#			set(FCONFIG_BUILDER_EXECUTABLE "${FCONFIG_BUILDER_DIR}/fconfig_builder")
#
#			if(EXISTS "${FCONFIG_BUILDER_EXECUTABLE}")
#				message(STATUS "fconfig_builder found in the local bin directory")
#			else()
#				message(FATAL_ERROR "Could not find fconfig_builder. Local path tried: ${FCONFIG_BUILDER_EXECUTABLE}")
#			endif()
#		endif ()
#	endif()
#	set(FCONFIG_BUILDER_EXECUTABLE "${FCONFIG_BUILDER_DIR}/fconfig_builder")

	set(FCONFIG_BUILDER_ARGS
		--main "${RUN_FCONFIG_BUILDER_MAIN_CONFIG_PATH}"
		--name ${RUN_FCONFIG_BUILDER_ROOT_CONFIG_CLASS_NAME}
		--source_dir "${RUN_FCONFIG_BUILDER_SOURCE_DIR}"
	)
	# change directory and run the fconfig_builder
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
The output was:\n${FCONFIG_BUILDER_OUTPUT}"
		)
	else()
		message(STATUS "Configuration classes generated successfully")
	endif()
endfunction()