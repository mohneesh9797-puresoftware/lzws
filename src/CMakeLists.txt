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

set (HEADERS ${HEADERS} "common.h" "config.h" "constants.h" "file.h" "log.h" "utils.h")
set (SOURCES ${SOURCES} "common.c" "file.c" "generated_constants.c" "log.c" "utils.c")

install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set (LIB_NAME ${LZWS_TARGET})
set (LIB_NAME_STATIC "${LIB_NAME}-static")

if (LZWS_SHARED)
  add_library (${LIB_NAME} SHARED ${SOURCES})
  target_link_libraries (${LIB_NAME} ${CMAKE_GMP_SHARED_LIBRARY_PATH})
  install (TARGETS ${LIB_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (LZWS_STATIC)
  add_library (${LIB_NAME_STATIC} STATIC ${SOURCES})
  target_link_libraries (${LIB_NAME_STATIC} ${CMAKE_GMP_STATIC_LIBRARY_PATH})
  set_target_properties (${LIB_NAME_STATIC} PROPERTIES OUTPUT_NAME ${LIB_NAME})
  install (TARGETS ${LIB_NAME_STATIC} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (LZWS_CLI)
  add_subdirectory ("cli")
endif ()
