function (generate_reverse_bits_table)
  if (DEFINED CMAKE_REVERSE_BITS_TABLE)
    return ()
  endif ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/generator_reverse_bits_table")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/generators/reverse_bits_table")
  set (NAME "cmake_generator_reverse_bits_table")

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
    message (FATAL_ERROR "Reverse bits table - compilation failed")
  endif ()

  execute_process(
    COMMAND "${BINARY_DIR}/main"
    RESULT_VARIABLE RUN_RESULT
    OUTPUT_VARIABLE RUN_OUTPUT
  )
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (RUN_RESULT EQUAL 0)
    set (CMAKE_REVERSE_BITS_TABLE ${RUN_OUTPUT} CACHE STRING "reverse bits table")
    message (STATUS "Reverse bits table - generated")
  else ()
    message (FATAL_ERROR "Reverse bits table - failed to generate")
  endif ()
endfunction ()
