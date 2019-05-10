cmake_minimum_required (VERSION 3.9)
project ("cmake_check_gmp" "C")

set (CMAKE_TRY_RUN false CACHE BOOL "try run targets after build")

set (CMAKE_GMP_INCLUDE_PATH        "" CACHE STRING "GMP include path")
set (CMAKE_GMP_SHARED_LIBRARY_PATH "" CACHE STRING "GMP shared library path")
set (CMAKE_GMP_STATIC_LIBRARY_PATH "" CACHE STRING "GMP static library path")

include_directories (${CMAKE_GMP_INCLUDE_PATH})

if (CMAKE_GMP_SHARED_LIBRARY_PATH)
  add_executable ("main" "main.c")
  target_include_directories("main" PUBLIC ${CMAKE_GMP_INCLUDE_PATH})
  target_link_libraries ("main" ${CMAKE_GMP_SHARED_LIBRARY_PATH})

  if (CMAKE_TRY_RUN)
    add_custom_command (
      TARGET "main"
      POST_BUILD
      COMMAND "./main"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
  endif ()
endif ()

if (CMAKE_GMP_STATIC_LIBRARY_PATH)
  add_executable ("main-static" "main.c")
  target_include_directories("main-static" PUBLIC ${CMAKE_GMP_INCLUDE_PATH})
  target_link_libraries ("main-static" ${CMAKE_GMP_STATIC_LIBRARY_PATH})

  if (CMAKE_TRY_RUN)
    add_custom_command (
      TARGET "main-static"
      POST_BUILD
      COMMAND "./main-static"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
  endif ()
endif ()
