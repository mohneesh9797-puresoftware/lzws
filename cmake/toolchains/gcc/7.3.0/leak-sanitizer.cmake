set (CMAKE_SYSTEM_NAME "Linux")
set (CMAKE_C_COMPILER "gcc-7.3.0")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -fsanitize=leak -fno-omit-frame-pointer")

set (CMAKE_INTERPROCEDURAL_OPTIMIZATION false CACHE BOOL "status of IPO switch")
