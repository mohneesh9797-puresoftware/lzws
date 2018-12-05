cmake_minimum_required(VERSION 3.9)
project ("cmake_generator_reverse_bits_table" "C")

include("../../toolchains/default.cmake")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

add_executable ("main" "main.c")
