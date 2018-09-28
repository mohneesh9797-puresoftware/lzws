function (cmake_get_verbose_flags)
  if (DEFINED CMAKE_GET_VERBOSE_FLAGS_PROCESSED)
    return ()
  endif ()

  set (CMAKE_VERBOSE_C_FLAGS "")

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/basic")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/basic")
  set (NAME "cmake_check_basic")

  # -- Werror --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} -Werror"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_COMPILE_RESULT
  )
  if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
    message (STATUS ${CHECK_COMPILE_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (${CHECK_RESULT})
    set (CMAKE_WERROR_C_FLAGS "-Werror" CACHE STRING "Werror C flags")
    message (STATUS "Check for C compiler -Werror support - yes")
  else ()
    set (CMAKE_WERROR_C_FLAGS "" CACHE STRING "Werror C flags")
    message (STATUS "Check for C compiler -Werror support - no")
  endif ()

  # -- pedantic --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -pedantic"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_COMPILE_RESULT
  )
  if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
    message (STATUS ${CHECK_COMPILE_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (${CHECK_RESULT})
    set (CMAKE_VERBOSE_C_FLAGS "${CMAKE_VERBOSE_C_FLAGS} -pedantic")
    message (STATUS "Check for C compiler -pedantic support - yes")
  else ()
    message (STATUS "Check for C compiler -pedantic support - no")
  endif ()

  # -- Wall --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -Wall"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_COMPILE_RESULT
  )
  if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
    message (STATUS ${CHECK_COMPILE_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (${CHECK_RESULT})
    set (CMAKE_VERBOSE_C_FLAGS "${CMAKE_VERBOSE_C_FLAGS} -Wall")
    message (STATUS "Check for C compiler -Wall support - yes")
  else ()
    message (STATUS "Check for C compiler -Wall support - no")
  endif ()

  # -- Wextra --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -Wextra"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_CONFIG_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_COMPILE_RESULT
  )
  if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
    message (STATUS ${CHECK_COMPILE_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (${CHECK_RESULT})
    set (CMAKE_VERBOSE_C_FLAGS "${CMAKE_VERBOSE_C_FLAGS} -Wextra")
    message (STATUS "Check for C compiler -Wextra support - yes")
  else ()
    message (STATUS "Check for C compiler -Wextra support - no")
  endif ()

  set (CMAKE_VERBOSE_C_FLAGS ${CMAKE_VERBOSE_C_FLAGS} CACHE STRING "verbose C flags")

  set (CMAKE_GET_VERBOSE_FLAGS_PROCESSED true CACHE STRING "Get verbose flags processed")
endfunction ()
