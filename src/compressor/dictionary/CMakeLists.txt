if (LZWS_DICTIONARY_TRIE_ON_LINKED_LIST)
  add_subdirectory ("trie-on-linked-list")
elseif (LZWS_DICTIONARY_TRIE_ON_SPARSE_ARRAY)
  add_subdirectory ("trie-on-sparse-array")
endif ()

set (HEADERS ${HEADERS} "${CMAKE_CURRENT_SOURCE_DIR}/common.h")
set (HEADERS ${HEADERS} PARENT_SCOPE)
