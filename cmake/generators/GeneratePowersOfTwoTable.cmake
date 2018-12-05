function (generate_powers_of_two_table)
  if (DEFINED CMAKE_POWERS_OF_TWO_TABLE)
    return ()
  endif ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/generator_powers_of_two_table")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/generators/powers_of_two_table")
  set (NAME "cmake_generator_powers_of_two_table")

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
    message (FATAL_ERROR "Powers of two table - compilation failed")
  endif ()

  execute_process(
    COMMAND "${BINARY_DIR}/main"
    RESULT_VARIABLE RUN_RESULT
    OUTPUT_VARIABLE RUN_OUTPUT
  )
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (RUN_RESULT EQUAL 0)
    set (CMAKE_POWERS_OF_TWO_TABLE ${RUN_OUTPUT} CACHE STRING "powers of two table")
    message (STATUS "Powers of two table - generated")
  else ()
    message (FATAL_ERROR "Powers of two table - failed to generate")
  endif ()
endfunction ()
