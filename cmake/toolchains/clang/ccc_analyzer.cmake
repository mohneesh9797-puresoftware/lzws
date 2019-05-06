set (CMAKE_SYSTEM_NAME "Linux")

set (CMAKE_C_COMPILER "ccc-analyzer")
set (CMAKE_C_COMPILER_AR "llvm-ar")
set (CMAKE_C_COMPILER_RANLIB "llvm-ranlib")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror" CACHE STRING "cmake C flags")

set (CMAKE_HAVE_IPO false CACHE BOOL "status of IPO support")
