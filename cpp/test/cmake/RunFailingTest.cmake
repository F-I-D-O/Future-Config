if(NOT DEFINED TEST_EXECUTABLE)
  message(FATAL_ERROR "TEST_EXECUTABLE not set")
endif()

if(NOT DEFINED EXPECTED_OUTPUT)
  message(FATAL_ERROR "EXPECTED_OUTPUT not set")
endif()

string(JOIN " " TEST_ARGS_FOR_PRINT ${TEST_ARGS})
message(STATUS "Running test executable: ${TEST_EXECUTABLE} with arguments: ${TEST_ARGS_FOR_PRINT}")
execute_process(
  COMMAND "${TEST_EXECUTABLE}" ${TEST_ARGS}
  RESULT_VARIABLE exit_code
  OUTPUT_VARIABLE stdout
  ERROR_VARIABLE stderr
)

set(all_output "${stdout}\n${stderr}")

# Must FAIL
if(exit_code EQUAL 0)
  message(FATAL_ERROR
    "Expected failure, but program exited with 0.\n"
    "Output:\n${all_output}"
  )
endif()

# Normalize line endings
string(REPLACE "\r\n" "\n" all_output "${all_output}")

# Trim
string(STRIP "${all_output}" all_output)

# Must contain expected output
string(REGEX MATCH "^${EXPECTED_OUTPUT}$" match "${all_output}")

if(NOT match)
	string(REPLACE " " "·" EXPECTED_OUTPUT_PRINT "${EXPECTED_OUTPUT}")
  message(FATAL_ERROR
"Program failed as expected, but output did NOT match:
EXPECTED OUTPUT: '${EXPECTED_OUTPUT_PRINT}'
ACTUAL OUTPUT: '${all_output}'"
  )
endif()

# Success
message(STATUS "Expected failure and message observed.")
