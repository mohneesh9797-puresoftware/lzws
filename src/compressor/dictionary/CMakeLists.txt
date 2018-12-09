if (LZWS_DICTIONARY_LINKED_LIST)
  add_subdirectory ("linked-list")
elseif (LZWS_DICTIONARY_SPARSE_ARRAY)
  add_subdirectory ("sparse-array")
endif ()

set (HEADERS ${HEADERS}
  "${CMAKE_CURRENT_SOURCE_DIR}/main.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/type.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/wrapper.h"
)
set (HEADERS ${HEADERS} PARENT_SCOPE)

set (SOURCES ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/wrapper.c")
set (SOURCES ${SOURCES} PARENT_SCOPE)
