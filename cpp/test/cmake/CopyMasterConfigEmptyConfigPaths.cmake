# Calls copy_master_config from setupFunctions.cmake with an empty CONFIG_PATHS list.
#
# This script is expected to FAIL with a fatal error. It is meant to be run through RunFailingTest.cmake, which checks
# the exit code and the error message.
#
# Plain CMake script - no project configuration required. The target does not need to exist, because the CONFIG_PATHS
# check has to fail before any command is attached to the target.
#
# Required variables (pass via -D):
#   SETUP_FUNCTIONS_PATH - path to install/setupFunctions.cmake

if(NOT DEFINED SETUP_FUNCTIONS_PATH OR NOT SETUP_FUNCTIONS_PATH)
	message(FATAL_ERROR "SETUP_FUNCTIONS_PATH not set")
endif()
if(NOT EXISTS "${SETUP_FUNCTIONS_PATH}")
	message(FATAL_ERROR "setupFunctions.cmake not found: ${SETUP_FUNCTIONS_PATH}")
endif()

include("${SETUP_FUNCTIONS_PATH}")

set(EMPTY_CONFIG_PATHS "")
copy_master_config(
	CONFIG_PATHS ${EMPTY_CONFIG_PATHS}
	TARGET_NAMES some_target
)
