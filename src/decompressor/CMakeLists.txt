add_subdirectory ("alignment")
add_subdirectory ("dictionary")

set (HEADERS ${HEADERS}
  "${CMAKE_CURRENT_SOURCE_DIR}/code.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/common.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/header.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/process_code.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/remainder.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/state.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/symbol.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils.h"
)
set (HEADERS ${HEADERS} PARENT_SCOPE)

set (SOURCES ${SOURCES}
  "${CMAKE_CURRENT_SOURCE_DIR}/code.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/header.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/process_code.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/remainder.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/state.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/symbol.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils.c"
)
set (SOURCES ${SOURCES} PARENT_SCOPE)
