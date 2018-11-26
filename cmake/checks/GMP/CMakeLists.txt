cmake_minimum_required(VERSION 3.9)
project ("cmake_check_gmp" "C")

set (CMAKE_TRY_RUN false CACHE BOOL "try run targets after build")

include_directories (${CMAKE_GMP_INCLUDE_PATH})

if (CMAKE_GMP_SHARED_LIBRARY_PATH)
  add_executable ("main" "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
  target_link_libraries ("main" ${CMAKE_GMP_SHARED_LIBRARY_PATH})

  if (CMAKE_TRY_RUN)
    add_custom_command(
      TARGET "main"
      POST_BUILD
      COMMAND "./main"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Running main"
    )
  endif ()
endif ()

if (CMAKE_GMP_STATIC_LIBRARY_PATH)
  add_executable ("main-static" "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
  target_link_libraries ("main-static" ${CMAKE_GMP_STATIC_LIBRARY_PATH})

  if (CMAKE_TRY_RUN)
    add_custom_command(
      TARGET "main-static"
      POST_BUILD
      COMMAND "./main-static"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Running main-static"
    )
  endif ()
endif ()
