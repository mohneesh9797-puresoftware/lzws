add_subdirectory ("dictionary")

set (HEADERS ${HEADERS}
  "${CMAKE_CURRENT_SOURCE_DIR}/common.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_alignment.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_code.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_header.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_remainder.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/state.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_symbol.h"
)
set (HEADERS ${HEADERS} PARENT_SCOPE)

set (SOURCES ${SOURCES}
  "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_alignment.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_code.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_header.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_remainder.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/state.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_symbol.c"
)
set (SOURCES ${SOURCES} PARENT_SCOPE)
