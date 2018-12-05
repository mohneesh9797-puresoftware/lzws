cmake_minimum_required(VERSION 3.9)
project ("lzws" "C")

if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed")
endif ()

set (LZWS_VERSION "0.1")

if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set (CMAKE_INSTALL_LIBDIR "lib" CACHE PATH "output directory for libraries")
endif ()
if (NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
  set (CMAKE_INSTALL_INCLUDEDIR "include" CACHE PATH "output directory for include files")
endif ()
if (NOT DEFINED CMAKE_INSTALL_BINDIR)
  set (CMAKE_INSTALL_BINDIR "bin" CACHE PATH "output directory for binaries")
endif ()

if (NOT DEFINED LZWS_TARGET)
  set (LZWS_TARGET ${PROJECT_NAME})
endif ()

set (LZWS_DICTIONARY "trie-on-sparse-array" CACHE STRING "dictionary implementation")

set (LZWS_SHARED true CACHE BOOL "build shared libs")
set (LZWS_STATIC true CACHE BOOL "build static libs")
set (LZWS_CLI    true CACHE BOOL "build cli")
set (LZWS_TESTS  true CACHE BOOL "build tests")

if (NOT LZWS_DICTIONARY)
  message (FATAL_ERROR "Please select dictionary implementation.")
endif ()

if (NOT LZWS_SHARED AND NOT LZWS_STATIC)
  message (FATAL_ERROR "Please enable building of shared or static libraries.")
endif ()

set (
  CMAKE_MODULE_PATH
  "${PROJECT_SOURCE_DIR}/cmake/checks"
  "${PROJECT_SOURCE_DIR}/cmake/finds"
  "${PROJECT_SOURCE_DIR}/cmake/generators"
)

include (GetVerboseFlags)
cmake_get_verbose_flags ()

include (GetPipeFlags)
cmake_get_pipe_flags ()

include (CheckC11)
cmake_check_c11 ()

include (CheckIPO)
cmake_check_ipo ()

include (CheckRunnable)
cmake_check_runnable ()

find_package(GMP REQUIRED)

include (CheckGMP)
cmake_check_gmp ()

if (LZWS_SHARED AND NOT CMAKE_GMP_SHARED_LIBRARY_PATH)
  message (FATAL_ERROR "GMP shared library is not available.")
endif ()
if (LZWS_STATIC AND NOT CMAKE_GMP_STATIC_LIBRARY_PATH)
  message (FATAL_ERROR "GMP static library is not available.")
endif ()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_PIPE_C_FLAGS} ${CMAKE_VERBOSE_C_FLAGS} ${CMAKE_C11_C_FLAGS}")

if (NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set (CMAKE_BUILD_TYPE DEBUG)
endif ()

set (CMAKE_C_FLAGS_DEBUG "-O0 -g")
set (CMAKE_C_FLAGS_RELEASE "-O2")
set (CMAKE_C_FLAGS_RELEASE_EMBED "-Os")

if (
  NOT DEFINED CMAKE_INTERPROCEDURAL_OPTIMIZATION AND
  CMAKE_HAVE_IPO AND (
    CMAKE_BUILD_TYPE MATCHES RELEASE OR
    CMAKE_BUILD_TYPE MATCHES RELEASE_EMBED
  )
)
  set (CMAKE_INTERPROCEDURAL_OPTIMIZATION true CACHE BOOL "status of IPO switch")
endif ()

add_subdirectory ("src")

if (${LZWS_TESTS})
  add_subdirectory ("tests")
endif ()

include (CPackConfig.cmake)
include (CPack)
