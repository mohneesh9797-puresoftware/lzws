function (cmake_get_verbose_flags)
  if (DEFINED CMAKE_GET_VERBOSE_FLAGS_PROCESSED)
    return ()
  endif ()

  set (CMAKE_VERBOSE_C_FLAGS "")

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/check_basic")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/basic")
  set (NAME "cmake_get_verbose_flags")

  # -- Werror --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} -Werror"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_WERROR_C_FLAGS "-Werror" CACHE STRING "Werror C flags")
    message (STATUS "Status of -Werror support - yes")
  else ()
    set (CMAKE_WERROR_C_FLAGS "" CACHE STRING "Werror C flags")
    message (STATUS "Status of -Werror support - no")
  endif ()

  # -- pedantic --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -pedantic"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_VERBOSE_C_FLAGS "${CMAKE_VERBOSE_C_FLAGS} -pedantic")
    message (STATUS "Status of -pedantic support - yes")
  else ()
    message (STATUS "Status of -pedantic support - no")
  endif ()

  # -- Wall --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -Wall"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_VERBOSE_C_FLAGS "${CMAKE_VERBOSE_C_FLAGS} -Wall")
    message (STATUS "Status of -Wall support - yes")
  else ()
    message (STATUS "Status of -Wall support - no")
  endif ()

  # -- Wextra --

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -Wextra"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
    OUTPUT_VARIABLE CHECK_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_VERBOSE_C_FLAGS "${CMAKE_VERBOSE_C_FLAGS} -Wextra")
    message (STATUS "Status of -Wextra support - yes")
  else ()
    message (STATUS "Status of -Wextra support - no")
  endif ()

  set (CMAKE_VERBOSE_C_FLAGS ${CMAKE_VERBOSE_C_FLAGS} CACHE STRING "verbose C flags")
  set (CMAKE_GET_VERBOSE_FLAGS_PROCESSED true CACHE STRING "get verbose flags processed")

  mark_as_advanced (CMAKE_VERBOSE_C_FLAGS CMAKE_GET_VERBOSE_FLAGS_PROCESSED)
endfunction ()
