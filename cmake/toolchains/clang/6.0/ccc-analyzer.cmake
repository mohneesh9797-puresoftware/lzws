set (CMAKE_SYSTEM_NAME "Linux")
set (CMAKE_C_COMPILER "ccc-analyzer")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

set (CMAKE_INTERPROCEDURAL_OPTIMIZATION false CACHE BOOL "status of IPO switch")
