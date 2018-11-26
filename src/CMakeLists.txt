if (LZWS_DICTIONARY STREQUAL "trie-on-linked-list")
  message(STATUS "Dictionary - trie on linked list")
  set (LZWS_DICTIONARY_TRIE_ON_LINKED_LIST true)
elseif (LZWS_DICTIONARY STREQUAL "trie-on-sparse-array")
  message(STATUS "Dictionary - trie on sparse array")
  set (LZWS_DICTIONARY_TRIE_ON_SPARSE_ARRAY true)
else ()
  message (FATAL_ERROR "Invalid dictionary implementation: ${LZWS_DICTIONARY}")
endif ()

configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

add_subdirectory ("compressor")

set (HEADERS ${HEADERS} "common.h" "config.h" "constants.h" "file.h" "macro.h" "utils.h")
set (SOURCES ${SOURCES} "common.c" "constants.c" "file.c" "utils.c")

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
endif()
