set (HEADERS "common.h" "config.h" "file.h")
set (SOURCES "file.c")

add_subdirectory ("compressor")

function (compile)
  configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

  if (LZWS_DICTIONARY_TRIE_ON_LINKED_LIST)
    add_subdirectory ("compressor/dictionary/trie_on_linked_list")
  elseif (LZWS_DICTIONARY_TRIE_ON_SPARSE_ARRAY)
    add_subdirectory ("compressor/dictionary/trie_on_sparse_array")
  endif ()

  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  set (LIB_NAME "${LZWS_TARGET}-${LZWS_DICTIONARY_IMPLEMENTATION}")
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
    set (SOURCES "cli.c")

    set (CLI_NAME "${LIB_NAME}-cli")
    set (CLI_NAME_STATIC "${CLI_NAME}-static")

    if (LZWS_SHARED)
      add_executable (${CLI_NAME} ${SOURCES})
      target_link_libraries (${CLI_NAME} ${LIB_NAME})
      install (TARGETS ${CLI_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif ()

    if (LZWS_STATIC)
      add_executable (${CLI_NAME_STATIC} ${SOURCES})
      target_link_libraries (${CLI_NAME_STATIC} ${LIB_NAME_STATIC})
      install (TARGETS ${CLI_NAME_STATIC} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif ()
  endif()
endfunction ()

foreach (LZWS_DICTIONARY_IMPLEMENTATION ${LZWS_DICTIONARY_IMPLEMENTATIONS})
  if (LZWS_DICTIONARY_IMPLEMENTATION STREQUAL "trie-on-linked-list")
    message(STATUS "Dictionary - trie on linked list")
    set (LZWS_DICTIONARY_TRIE_ON_LINKED_LIST true)
    compile ()
    unset (LZWS_DICTIONARY_TRIE_ON_LINKED_LIST)

  elseif (LZWS_DICTIONARY_IMPLEMENTATION STREQUAL "trie-on-sparse-array")
    message(STATUS "Dictionary - trie on sparse array")
    set (LZWS_DICTIONARY_TRIE_ON_SPARSE_ARRAY true)
    compile ()
    unset (LZWS_DICTIONARY_TRIE_ON_SPARSE_ARRAY)

  else ()
    message (FATAL_ERROR "Invalid dictionary implementation: ${LZWS_DICTIONARY_IMPLEMENTATION}")
  endif ()
endforeach ()
