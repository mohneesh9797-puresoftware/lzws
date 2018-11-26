function (cmake_check_c11)
  if (DEFINED CMAKE_HAVE_C11)
    return ()
  endif ()

  include (GetVerboseFlags)
  cmake_get_verbose_flags ()

  include (CheckRunnable)
  cmake_check_runnable ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/C11")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/C11")
  set (NAME "cmake_check_c11")

  # -- -gnu11 --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -std=gnu11"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
      "-DCMAKE_TRY_RUN=${CMAKE_CAN_RUN_EXE}"
    OUTPUT_VARIABLE CHECK_OUTPUT_RESULT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_HAVE_C11 true CACHE STRING "status of C11 support")
    set (CMAKE_C11_C_FLAGS "-std=gnu11" CACHE STRING "c11 C flags")
    message (STATUS "Status of C11 support - gnu11")
    return ()
  endif ()

  # -- -c11 --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -std=c11"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
      "-DCMAKE_TRY_RUN=${CMAKE_CAN_RUN_EXE}"
    OUTPUT_VARIABLE CHECK_OUTPUT_RESULT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_HAVE_C11 true CACHE STRING "status of C11 support")
    set (CMAKE_C11_C_FLAGS "-std=c11" CACHE STRING "c11 C flags")
    message (STATUS "Status of C11 support - c11")
    return ()
  endif ()

  # -- vanilla --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS}"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
      "-DCMAKE_TRY_RUN=${CMAKE_CAN_RUN_EXE}"
    OUTPUT_VARIABLE CHECK_OUTPUT_RESULT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_HAVE_C11 true CACHE STRING "status of C11 support")
    set (CMAKE_C11_C_FLAGS "" CACHE STRING "c11 C flags")
    message (STATUS "Status of C11 support - vanilla")
    return ()
  endif ()

  # -- no support --

  set (CMAKE_HAVE_C11 false CACHE STRING "status of C11 support")
  set (CMAKE_C11_C_FLAGS "" CACHE STRING "c11 C flags")
  message (STATUS "Status of C11 support - no")

  mark_as_advanced(CMAKE_HAVE_C11 CMAKE_C11_C_FLAGS)
endfunction ()
