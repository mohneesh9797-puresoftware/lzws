set (SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/code.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/header.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/max_code_bit_length.c"
)

include (DefineRelativeFilePaths)
cmake_define_relative_file_paths ("${SOURCES}")

if (LZWS_SHARED)
  set (TEST_NAME "test-invalid")
  set (TEST_TARGET "${LZWS_TARGET}-${TEST_NAME}")

  add_executable (${TEST_TARGET} ${SOURCES})
  target_link_libraries (${TEST_TARGET} ${TEST_LIB_TARGET})

  if (CMAKE_CAN_RUN_EXE)
    add_test (${TEST_NAME} ${TEST_TARGET})
  endif ()
endif ()

if (LZWS_STATIC)
  set (STATIC_TEST_NAME "static-test-invalid")
  set (STATIC_TEST_TARGET "${LZWS_TARGET}-${STATIC_TEST_NAME}")

  add_executable (${STATIC_TEST_TARGET} ${SOURCES})
  target_link_libraries (${STATIC_TEST_TARGET} ${STATIC_TEST_LIB_TARGET})

  if (CMAKE_CAN_RUN_EXE)
    add_test (${STATIC_TEST_NAME} ${STATIC_TEST_TARGET})
  endif ()
endif ()
