set (CMAKE_C_COMPILER "gcc")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -fsanitize=leak -fno-omit-frame-pointer" CACHE STRING "cmake C flags")

set (CMAKE_HAVE_IPO false CACHE BOOL "status of IPO support")
