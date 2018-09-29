cmake_minimum_required(VERSION 3.9)
project ("cmake_check_basic" "C")

set (CMAKE_TRY_RUN false CACHE BOOL "try run targets after build")

add_executable (
  "main"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
)

if (CMAKE_TRY_RUN)
  add_custom_command(
    TARGET "main"
    POST_BUILD
    COMMAND "./main"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running main"
  )
endif ()
