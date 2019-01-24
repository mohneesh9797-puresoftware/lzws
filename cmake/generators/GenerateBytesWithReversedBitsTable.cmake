function (generate_bytes_with_reversed_bits_table)
  if (DEFINED CMAKE_BYTES_WITH_REVERSED_BITS_TABLE)
    return ()
  endif ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/generator_bytes_with_reversed_bits_table")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/generators/bytes_with_reversed_bits_table")
  set (NAME "cmake_generator_bytes_with_reversed_bits_table")

  try_compile (
    COMPILE_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE COMPILE_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${COMPILE_OUTPUT})
  endif ()

  if (NOT COMPILE_RESULT)
    FILE (REMOVE_RECURSE ${BINARY_DIR})
    message (FATAL_ERROR "Bytes with reversed bits table - compilation failed")
  endif ()

  execute_process (
    COMMAND "${BINARY_DIR}/main"
    RESULT_VARIABLE RUN_RESULT
    OUTPUT_VARIABLE RUN_OUTPUT
  )
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  set (CMAKE_BYTES_WITH_REVERSED_BITS_TABLE ${RUN_OUTPUT} PARENT_SCOPE)

  if (RUN_RESULT EQUAL 0)
    message (STATUS "Bytes with reversed bits table - generated")
  else ()
    message (FATAL_ERROR "Bytes with reversed bits table - failed to generate")
  endif ()

  mark_as_advanced (CMAKE_BYTES_WITH_REVERSED_BITS_TABLE)
endfunction ()
