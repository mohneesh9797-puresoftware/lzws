set (CMAKE_SYSTEM_NAME "Linux")

set (CMAKE_C_COMPILER "gcc")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -Werror -fsanitize=thread -fno-omit-frame-pointer")

set (CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -pie")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pie")

set (CMAKE_INTERPROCEDURAL_OPTIMIZATION false CACHE BOOL "status of IPO switch")
