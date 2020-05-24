cmake_minimum_required (VERSION 3.9)
project ("cmake_generator_bytes_with_reversed_bits_table" "C")

include ("../common/toolchain.cmake")

include_directories ("../common")

add_executable ("main" "main.c")
