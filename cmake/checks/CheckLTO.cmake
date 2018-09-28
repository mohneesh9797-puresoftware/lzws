function (cmake_check_lto)
  if (DEFINED CMAKE_HAVE_LTO)
    return ()
  endif ()

  include (GetVerboseFlags)
  cmake_get_verbose_flags ()

  set (BINARY_DIR "${PROJECT_BINARY_DIR}/CMakeTmp/basic")
  set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/checks/basic")
  set (NAME "cmake_check_basic")

  try_compile (
    CHECK_RESULT ${BINARY_DIR} ${SOURCE_DIR} ${NAME}
    CMAKE_FLAGS
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_WERROR_C_FLAGS} -flto"
      "-DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS} -fuse-linker-plugin"
    OUTPUT_VARIABLE CHECK_COMPILE_RESULT
  )
  if (${CMAKE_CONFIG_VERBOSE_MAKEFILE})
    message (STATUS ${CHECK_COMPILE_RESULT})
  endif ()
  FILE (REMOVE_RECURSE ${BINARY_DIR})

  if (${CHECK_RESULT})
    set (CMAKE_HAVE_LTO true CACHE STRING "Status of LTO support")
    set (CMAKE_LTO_C_FLAGS "-flto" CACHE STRING "LTO C flags")
    set (CMAKE_LTO_LD_FLAGS "-fuse-linker-plugin" CACHE STRING "LTO LD flags")
    message (STATUS "Check for C compiler LTO support - yes")
    return ()
  endif ()

  set (CMAKE_HAVE_LTO false CACHE STRING "Status of LTO support")
  set (CMAKE_LTO_C_FLAGS "" CACHE STRING "LTO C flags")
  set (CMAKE_LTO_LD_FLAGS "" CACHE STRING "LTO LD flags")
  message (STATUS "Check for C compiler LTO support - no")
endfunction ()
