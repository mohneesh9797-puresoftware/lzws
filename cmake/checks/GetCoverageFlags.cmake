function (cmake_get_coverage_flags)
  if (DEFINED CMAKE_GET_COVERAGE_FLAGS_PROCESSED)
    return ()
  endif ()

  include (GetVerboseFlags)
  cmake_get_verbose_flags ()

  include (CheckRunnable)
  cmake_check_runnable ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/check_basic")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/basic")
  set (NAME "cmake_get_coverage_flags")

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} --coverage"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
      "-DCMAKE_TRY_RUN=${CMAKE_CAN_RUN_EXE}"
    OUTPUT_VARIABLE CHECK_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_COVERAGE_C_FLAGS "--coverage")
    message (STATUS "Status of --coverage support - yes")
  else ()
    set (CMAKE_COVERAGE_C_FLAGS "")
    message (STATUS "Status of --coverage support - no")
  endif ()

  set (CMAKE_COVERAGE_C_FLAGS ${CMAKE_COVERAGE_C_FLAGS} CACHE STRING "coverage C flags")
  set (CMAKE_GET_COVERAGE_FLAGS_PROCESSED true CACHE STRING "get coverage flags processed")

  mark_as_advanced (CMAKE_COVERAGE_C_FLAGS CMAKE_GET_COVERAGE_FLAGS_PROCESSED)
endfunction ()
