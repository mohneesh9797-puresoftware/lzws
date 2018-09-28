set (CMAKE_SYSTEM_NAME "Linux")
set (CMAKE_C_COMPILER "gcc-7.3.0")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -Werror -fsanitize=thread -fno-omit-frame-pointer")

set (CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -pie")
set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pie")

set (CMAKE_HAVE_LTO false CACHE STRING "Status of LTO support")
