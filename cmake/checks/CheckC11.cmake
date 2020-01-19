function (cmake_check_c11)
  if (DEFINED CMAKE_HAVE_C11)
    return ()
  endif ()

  include (GetVerboseFlags)
  cmake_get_verbose_flags ()

  include (CheckRunnable)
  cmake_check_runnable ()

  set (CMAKE_HAVE_C11 false)
  set (CMAKE_C11_C_FLAGS "")

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/check_C11")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/C11")
  set (NAME "cmake_check_c11")

  set (MESSAGE_PREFIX "Status of C11 support")

  # -- -gnu11 --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -std=gnu11"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
      "-DCMAKE_TRY_RUN=${CMAKE_CAN_RUN_EXE}"
    OUTPUT_VARIABLE CHECK_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_HAVE_C11 true)
    set (CMAKE_C11_C_FLAGS "-std=gnu11")
    message (STATUS "${MESSAGE_PREFIX} - gnu11")
  endif ()

  # -- -c11 --

  if (NOT CHECK_RESULT)
    try_compile (
      CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
      CMAKE_FLAGS
        "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -std=c11"
        "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
        "-DCMAKE_TRY_RUN=${CMAKE_CAN_RUN_EXE}"
      OUTPUT_VARIABLE CHECK_OUTPUT
    )
    if (CMAKE_VERBOSE_MAKEFILE)
      message (STATUS ${CHECK_OUTPUT})
    endif ()
    FILE (REMOVE_RECURSE ${BINARY_DIR})

    if (CHECK_RESULT)
      set (CMAKE_HAVE_C11 true)
      set (CMAKE_C11_C_FLAGS "-std=c11")
      message (STATUS "${MESSAGE_PREFIX} - c11")
    endif ()
  endif ()

  # -- vanilla --

  if (NOT CHECK_RESULT)
    try_compile (
      CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
      CMAKE_FLAGS
        "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS}"
        "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
        "-DCMAKE_TRY_RUN=${CMAKE_CAN_RUN_EXE}"
      OUTPUT_VARIABLE CHECK_OUTPUT
    )
    if (CMAKE_VERBOSE_MAKEFILE)
      message (STATUS ${CHECK_OUTPUT})
    endif ()
    FILE (REMOVE_RECURSE ${BINARY_DIR})

    if (CHECK_RESULT)
      set (CMAKE_HAVE_C11 true)
      set (CMAKE_C11_C_FLAGS "")
      message (STATUS "${MESSAGE_PREFIX} - vanilla")
    endif ()
  endif ()

  # -- no support --

  if (NOT CHECK_RESULT)
    set (CMAKE_HAVE_C11 false)
    set (CMAKE_C11_C_FLAGS "")
    message (STATUS "${MESSAGE_PREFIX} - no")
  endif ()

  set (CMAKE_HAVE_C11 ${CMAKE_HAVE_C11} CACHE STRING "Status of C11")
  set (CMAKE_C11_C_FLAGS ${CMAKE_C11_C_FLAGS} CACHE STRING "C11 C flags")

  mark_as_advanced (CMAKE_HAVE_C11 CMAKE_C11_C_FLAGS)
endfunction ()
