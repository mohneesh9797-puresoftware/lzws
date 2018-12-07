set (CMAKE_SYSTEM_NAME "Linux")

set (CMAKE_C_COMPILER "clang")
set (CMAKE_C_COMPILER_AR "llvm-ar")
set (CMAKE_C_COMPILER_RANLIB "llvm-ranlib")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -fsanitize=undefined -fno-omit-frame-pointer" CACHE STRING "cmake C flags")

set (CMAKE_HAVE_IPO false CACHE BOOL "status of IPO support")
