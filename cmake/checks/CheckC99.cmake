function (cmake_check_c99)
  if (DEFINED CMAKE_HAVE_C99)
    return ()
  endif ()

  include (GetVerboseFlags)
  cmake_get_verbose_flags ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/C99")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/C99")
  set (NAME "cmake_check_c99")

  # -- -gnu99 --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -std=gnu99"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_COMPILE_RESULT
  )
  if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
    message (STATUS ${CHECK_COMPILE_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (${CHECK_RESULT})
    set (CMAKE_HAVE_C99 true CACHE STRING "Status of C99 support")
    set (CMAKE_C99_C_FLAGS "-std=gnu99" CACHE STRING "c99 C flags")
    message (STATUS "Check for C compiler C99 support - gnu99")
    return ()
  endif ()

  # -- -c99 --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -std=c99"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_COMPILE_RESULT
  )
  if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
    message (STATUS ${CHECK_COMPILE_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (${CHECK_RESULT})
    set (CMAKE_HAVE_C99 true CACHE STRING "Status of C99 support")
    set (CMAKE_C99_C_FLAGS "-std=c99" CACHE STRING "c99 C flags")
    message (STATUS "Check for C compiler C99 support - c99")
    return ()
  endif ()

  # -- vanilla --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS}"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_COMPILE_RESULT
  )
  if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
    message (STATUS ${CHECK_COMPILE_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (${CHECK_RESULT})
    set (CMAKE_HAVE_C99 true CACHE STRING "Status of C99 support")
    set (CMAKE_C99_C_FLAGS "" CACHE STRING "c99 C flags")
    message (STATUS "Check for C compiler C99 support - vanilla")
    return ()
  endif ()

  # -- no support --

  set (CMAKE_HAVE_C99 false CACHE STRING "Status of C99 support")
  set (CMAKE_C99_C_FLAGS "" CACHE STRING "c99 C flags")
  message (STATUS "Check for C compiler C99 support - no")
endfunction ()
