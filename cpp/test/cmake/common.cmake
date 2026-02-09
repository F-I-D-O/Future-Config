function(add_failing_test_with_output)
  cmake_parse_arguments(ARG
    ""
    "NAME;EXECUTABLE;EXPECTED_OUTPUT"
    ""
    ${ARGN}
  )

  if(NOT ARG_NAME OR NOT ARG_EXECUTABLE OR NOT ARG_EXPECTED_OUTPUT)
    message(FATAL_ERROR
      "Usage:\n"
      "  add_failing_test_with_output(\n"
      "    NAME <name>\n"
      "    EXECUTABLE <exe>\n"
      "    EXPECTED_OUTPUT <regex>\n"
      "  )"
    )
  endif()

  add_test(
    NAME ${ARG_NAME}
    COMMAND
      ${CMAKE_COMMAND}
      -DTEST_EXECUTABLE=$<TARGET_FILE:${ARG_EXECUTABLE}>
      -DEXPECTED_OUTPUT=${ARG_EXPECTED_OUTPUT}
      -P ${CMAKE_CURRENT_LIST_DIR}/RunFailingTest.cmake
  )
endfunction()

if(NOT DEFINED TEST_EXECUTABLE)
  message(FATAL_ERROR "TEST_EXECUTABLE not set")
endif()

if(NOT DEFINED EXPECTED_OUTPUT)
  message(FATAL_ERROR "EXPECTED_OUTPUT not set")
endif()

execute_process(
  COMMAND "${TEST_EXECUTABLE}"
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

# Must contain expected output
string(REGEX MATCH "${EXPECTED_OUTPUT}" match "${all_output}")

if(NOT match)
  message(FATAL_ERROR
"Program failed as expected, but output did NOT match:
EXPECTED OUTPUT: '${EXPECTED_OUTPUT}'
ACTUAL OUTPUT: '${all_output}"
  )
endif()

# Success
message(STATUS "Expected failure and message observed.")

