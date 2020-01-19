function (cmake_check_gmp)
  if (DEFINED CMAKE_GMP_WORKS)
    return ()
  endif ()

  find_package (GMP REQUIRED)

  include (GetVerboseFlags)
  cmake_get_verbose_flags ()

  include (CheckC11)
  cmake_check_c11 ()

  include (CheckRunnable)
  cmake_check_runnable ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/check_GMP")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/GMP")
  set (NAME "cmake_check_gmp")

  set (MESSAGE_PREFIX "Status of GMP support")

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_C11_C_FLAGS} ${CMAKE_WERROR_C_FLAGS}"
      "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}"
      "-DCMAKE_GMP_INCLUDE_PATH=${CMAKE_GMP_INCLUDE_PATH}"
      "-DCMAKE_GMP_SHARED_LIBRARY_PATH=${CMAKE_GMP_SHARED_LIBRARY_PATH}"
      "-DCMAKE_GMP_STATIC_LIBRARY_PATH=${CMAKE_GMP_STATIC_LIBRARY_PATH}"
      "-DCMAKE_TRY_RUN=${CMAKE_CAN_RUN_EXE}"
    OUTPUT_VARIABLE CHECK_OUTPUT
  )
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (CHECK_RESULT)
    set (CMAKE_GMP_WORKS true)
    message (STATUS "${MESSAGE_PREFIX} - working")
  else ()
    set (CMAKE_GMP_WORKS false)
    message (FATAL_ERROR "${MESSAGE_PREFIX} - not working")
  endif ()

  set (CMAKE_GMP_WORKS ${CMAKE_GMP_WORKS} CACHE STRING "Status of GMP")

  mark_as_advanced (CMAKE_GMP_WORKS)
endfunction ()
