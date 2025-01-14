
message("Creating directory for test build: ${FCONFIG_TEST_BUILD_DIR}")
file(MAKE_DIRECTORY "${FCONFIG_TEST_BUILD_DIR}")

message("Configuring test build")
execute_process(COMMAND ${CMAKE_COMMAND}
	.
	-B "{FCONFIG_TEST_BUILD_DIR}"
	--toolchain "${VCPKG_TOOLCHAIN_FILE}"
	-DCMAKE_BUILD_TYPE=Release
)