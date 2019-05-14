if (LZWS_COMPRESSOR_DICTIONARY STREQUAL "linked-list")
  message (STATUS "Dictionary - linked list")
  set (LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST true)
elseif (LZWS_COMPRESSOR_DICTIONARY STREQUAL "sparse-array")
  message (STATUS "Dictionary - sparse array")
  set (LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY true)
else ()
  message (FATAL_ERROR "Invalid compressor dictionary implementation: ${LZWS_COMPRESSOR_DICTIONARY}")
endif ()

configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

include (GenerateBytesWithReversedBitsTable)
generate_bytes_with_reversed_bits_table ()

include (GeneratePowersOfTwoTable)
generate_powers_of_two_table ()

include (GenerateMasksForLastBitsTable)
generate_masks_for_last_bits_table ()

configure_file ("generated_constants.c.in" "${CMAKE_CURRENT_SOURCE_DIR}/generated_constants.c")

add_subdirectory ("compressor")
add_subdirectory ("decompressor")

set (HEADERS ${HEADERS}
  "${CMAKE_CURRENT_SOURCE_DIR}/buffer.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/common.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/constants.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/file.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/log.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/macro.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/string.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils.h"
)
set (SOURCES ${SOURCES}
  "${CMAKE_CURRENT_SOURCE_DIR}/buffer.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/common.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/file.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/generated_constants.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/log.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/string.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils.c"
)

include (DefineRelativeFilePaths)
cmake_define_relative_file_paths (${SOURCES})

include_directories (${CMAKE_GMP_INCLUDE_PATH})

foreach (HEADER ${HEADERS})
  file (RELATIVE_PATH HEADER_RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${HEADER})
  get_filename_component (HEADER_RELATIVE_DIRECTORY_PATH ${HEADER_RELATIVE_PATH} DIRECTORY)
  install (FILES ${HEADER} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${HEADER_RELATIVE_DIRECTORY_PATH}")
endforeach ()

if (LZWS_SHARED)
  set (LIB_TARGET ${LZWS_TARGET})

  add_library (${LIB_TARGET} SHARED ${SOURCES})
  target_link_libraries (${LIB_TARGET} ${CMAKE_GMP_SHARED_LIBRARY_PATH})

  install (TARGETS ${LIB_TARGET} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (LZWS_STATIC)
  set (STATIC_LIB_TARGET "${LZWS_TARGET}-static")
  set (STATIC_LIB_NAME ${LZWS_TARGET})

  add_library (${STATIC_LIB_TARGET} STATIC ${SOURCES})
  target_link_libraries (${STATIC_LIB_TARGET} ${CMAKE_GMP_STATIC_LIBRARY_PATH})

  set_target_properties (${STATIC_LIB_TARGET} PROPERTIES OUTPUT_NAME ${STATIC_LIB_NAME})
  install (TARGETS ${STATIC_LIB_TARGET} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (LZWS_CLI)
  add_subdirectory ("cli")
endif ()

if (LZWS_EXAMPLES)
  add_subdirectory ("examples")
endif ()

if (LZWS_TESTS)
  add_subdirectory ("tests")
endif ()
