set (CMAKE_C_COMPILER "clang")
set (CMAKE_C_COMPILER_AR "llvm-ar")
set (CMAKE_C_COMPILER_RANLIB "llvm-ranlib")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-inline" CACHE STRING "cmake C flags")
