function (generate_bit_masks_table)
  if (DEFINED CMAKE_BIT_MASKS_TABLE)
    return ()
  endif ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/generator_bit_masks_table")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/generators/bit_masks_table")
  set (NAME "cmake_generator_bit_masks_table")

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
    message (FATAL_ERROR "Bit masks table - compilation failed")
  endif ()

  execute_process (
    COMMAND "${BINARY_DIR}/main"
    RESULT_VARIABLE RUN_RESULT
    OUTPUT_VARIABLE RUN_OUTPUT
  )
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  set (CMAKE_BIT_MASKS_TABLE ${RUN_OUTPUT} PARENT_SCOPE)

  if (RUN_RESULT EQUAL 0)
    message (STATUS "Bit masks table - generated")
  else ()
    message (FATAL_ERROR "Bit masks table - failed to generate")
  endif ()

  mark_as_advanced (CMAKE_BIT_MASKS_TABLE)
endfunction ()
