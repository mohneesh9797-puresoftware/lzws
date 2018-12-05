# We need to set default values in order to override values from any other toolchain.

set (CMAKE_C_COMPILER "cc")
set (CMAKE_C_COMPILER_AR "ar")
set (CMAKE_C_COMPILER_RANLIB "ranlib")

set (CMAKE_C_FLAGS "")
set (CMAKE_EXE_LINKER_FLAGS "")
set (CMAKE_SHARED_LINKER_FLAGS "")

set (CMAKE_INTERPROCEDURAL_OPTIMIZATION false)
