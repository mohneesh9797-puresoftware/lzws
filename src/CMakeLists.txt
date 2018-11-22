if (LZWS_DICTIONARY STREQUAL "trie-on-sparse-array")
  message(STATUS "Dictionary - trie on sparse array")
  set (LZWS_DICTIONARY_TRIE_ON_SPARSE_ARRAY true)
elseif (LZWS_DICTIONARY STREQUAL "trie-on-linked-list")
  message(STATUS "Dictionary - trie on linked list")
  set (LZWS_DICTIONARY_TRIE_ON_LINKED_LIST true)
else ()
  message (FATAL_ERROR "Invalid dictionary implementation: ${LZWS_DICTIONARY}")
endif ()

configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

set (HEADERS "common.h" "config.h" "file.h")
set (SOURCES "file.c")

add_subdirectory ("compressor")

install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set (LIB_NAME "${LZWS_TARGET}-${LZWS_DICTIONARY}")
set (LIB_NAME_STATIC "${LIB_NAME}-static")

if (LZWS_SHARED)
  add_library (${LIB_NAME} SHARED ${SOURCES})
  install (TARGETS ${LIB_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (LZWS_STATIC)
  add_library (${LIB_NAME_STATIC} STATIC ${SOURCES})
  set_target_properties (${LIB_NAME_STATIC} PROPERTIES OUTPUT_NAME ${LIB_NAME})
  install (TARGETS ${LIB_NAME_STATIC} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (LZWS_CLI)
  add_subdirectory ("cli")
endif()
