set (CMAKE_SYSTEM_NAME "Linux")
set (CMAKE_C_COMPILER "clang-6.0")
set (_CMAKE_TOOLCHAIN_PREFIX "llvm-")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -fsanitize=address -fno-omit-frame-pointer")

set (CMAKE_INTERPROCEDURAL_OPTIMIZATION false CACHE BOOL "status of IPO switch")
