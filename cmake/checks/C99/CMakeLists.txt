cmake_minimum_required(VERSION 3.9)
project ("cmake_check_c99" "C")

add_executable (
  "main"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
)
