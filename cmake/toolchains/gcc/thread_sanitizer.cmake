set (CMAKE_SYSTEM_NAME "Linux")

set (CMAKE_C_COMPILER "gcc")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -Werror -fsanitize=thread -fno-omit-frame-pointer" CACHE STRING "cmake C flags")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie" CACHE STRING "cmake exe linker flags")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pie" CACHE STRING "cmake shared linker flags")

set (CMAKE_HAVE_IPO false CACHE BOOL "status of IPO support")
