set (SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/codes.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/combinations.c"
)

include (DefineRelativeFilePaths)
cmake_define_relative_file_paths ("${SOURCES}")

if (LZWS_SHARED)
  set (TEST_LIB_TARGET "${LZWS_TARGET}-test")
  add_library (${TEST_LIB_TARGET} SHARED ${SOURCES})
  target_link_libraries (${TEST_LIB_TARGET} ${LIB_TARGET})
endif ()

if (LZWS_STATIC)
  set (STATIC_TEST_LIB_TARGET "${LZWS_TARGET}-static-test")
  add_library (${STATIC_TEST_LIB_TARGET} STATIC ${SOURCES})
  target_link_libraries (${STATIC_TEST_LIB_TARGET} ${STATIC_LIB_TARGET})
endif ()

add_subdirectory ("basic")
add_subdirectory ("invalid")
