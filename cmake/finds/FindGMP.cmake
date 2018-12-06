if (DEFINED CMAKE_GMP_FOUND)
  return ()
endif ()

find_path (CMAKE_GMP_INCLUDE_PATH NAMES "gmp.h")
if (CMAKE_GMP_INCLUDE_PATH)
  set (PATH_STATUS "found: \"${CMAKE_GMP_INCLUDE_PATH}\"")
else ()
  set (PATH_STATUS "not found")
endif ()
message (STATUS "Status of GMP library - include path is ${PATH_STATUS}")

find_library (CMAKE_GMP_SHARED_LIBRARY_PATH NAMES "libgmp.so")
if (CMAKE_GMP_SHARED_LIBRARY_PATH)
  set (PATH_STATUS "found: \"${CMAKE_GMP_SHARED_LIBRARY_PATH}\"")
else ()
  set (PATH_STATUS "not found")
endif ()
message (STATUS "Status of GMP library - shared library path is ${PATH_STATUS}")

find_library (CMAKE_GMP_STATIC_LIBRARY_PATH NAMES "libgmp.a")
if (CMAKE_GMP_STATIC_LIBRARY_PATH)
  set (PATH_STATUS "found: \"${CMAKE_GMP_STATIC_LIBRARY_PATH}\"")
else ()
  set (PATH_STATUS "not found")
endif ()
message (STATUS "Status of GMP library - static library path is ${PATH_STATUS}")

set (
  CMAKE_GMP_FOUND
  (CMAKE_GMP_INCLUDE_PATH AND (CMAKE_GMP_SHARED_LIBRARY_PATH OR CMAKE_GMP_STATIC_LIBRARY_PATH))
  CACHE STRING "GMP found"
)

if (NOT CMAKE_GMP_FOUND AND GMP_FIND_REQUIRED)
  message (FATAL_ERROR "GMP is required")
endif ()

mark_as_advanced (CMAKE_GMP_FOUND CMAKE_GMP_INCLUDE_PATH CMAKE_GMP_SHARED_LIBRARY_PATH CMAKE_GMP_STATIC_LIBRARY_PATH)
