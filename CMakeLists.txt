cmake_minimum_required(VERSION 3.9)
project ("lzw-ab" "C")

set (LZW_AB_VERSION "0.1")

if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set (CMAKE_INSTALL_LIBDIR "lib" CACHE PATH "Output directory for libraries")
endif ()
if (NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  set (CMAKE_INSTALL_INCLUDEDIR "include" CACHE PATH "Output directory for include files")
endif ()
if (NOT DEFINED CMAKE_INSTALL_BINDIR)
  set (CMAKE_INSTALL_INCLUDEDIR "bin" CACHE PATH "Output directory for binaries")
endif ()

set (LZW_AB_SHARED true CACHE BOOL "build shared libs")
set (LZW_AB_STATIC true CACHE BOOL "build static libs")

if (NOT DEFINED LZW_AB_TARGET)
  set (LZW_AB_TARGET ${PROJECT_NAME})
endif ()

if (NOT LZW_AB_STATIC AND NOT LZW_AB_SHARED)
  message (FATAL_ERROR "LZW AB can not be compiled. Please enable building of shared or static libraries.")
endif ()

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/checks")

include (GetVerboseFlags)
include (GetPipeFlags)
include (CheckC99)
include (CheckLTO)
include (CheckRunnable)

cmake_get_verbose_flags ()
cmake_get_pipe_flags ()
cmake_check_c99 ()
cmake_check_lto ()
cmake_check_runnable ()

if (NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set (CMAKE_BUILD_TYPE DEBUG)
endif ()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TRALLOC_PIPE_C_FLAGS} ${TRALLOC_VERBOSE_C_FLAGS} ${TRALLOC_C99_C_FLAGS}")

set (CMAKE_C_FLAGS_DEBUG "-O0 -g")

set (CMAKE_C_FLAGS_RELEASE "-O2 ${TRALLOC_LTO_C_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS}    ${TRALLOC_LTO_LD_FLAGS}")
set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS} ${TRALLOC_LTO_LD_FLAGS}")

set (CMAKE_C_FLAGS_RELEASE_EMBED "-Os ${TRALLOC_LTO_C_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS_RELEASE_EMBED    "${CMAKE_EXE_LINKER_FLAGS}    ${TRALLOC_LTO_LD_FLAGS}")
set (CMAKE_SHARED_LINKER_FLAGS_RELEASE_EMBED "${CMAKE_SHARED_LINKER_FLAGS} ${TRALLOC_LTO_LD_FLAGS}")

include_directories ("src")
add_subdirectory ("src")

include (CPackConfig.cmake)
include (CPack)
