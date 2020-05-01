function (cmake_check_runnable)
  if (DEFINED CMAKE_CAN_RUN_EXE)
    return ()
  endif ()

  set (MESSAGE_PREFIX "Status of run exe support")

  set (NAME "cmake_check_runnable")
  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/check_basic")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/basic")

  include (GetVerboseFlags)
  cmake_get_verbose_flags ()

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS}"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
      "-DCMAKE_TRY_RUN=1"
    OUTPUT_VARIABLE CHECK_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_CAN_RUN_EXE true)
    message (STATUS "${MESSAGE_PREFIX} - yes")
  else ()
    set (CMAKE_CAN_RUN_EXE false)
    message (STATUS "${MESSAGE_PREFIX} - no")
  endif ()

  set (CMAKE_CAN_RUN_EXE ${CMAKE_CAN_RUN_EXE} CACHE STRING "Status of run exe")

  mark_as_advanced (CMAKE_CAN_RUN_EXE)
endfunction ()
