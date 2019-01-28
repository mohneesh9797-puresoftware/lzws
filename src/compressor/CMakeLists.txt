add_subdirectory ("dictionary")
add_subdirectory ("ratio")

set (HEADERS ${HEADERS}
  "${CMAKE_CURRENT_SOURCE_DIR}/common.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/process_code.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_symbol.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/state.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_alignment.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_code.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_header.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_remainder.h"
)
set (HEADERS ${HEADERS} PARENT_SCOPE)

set (SOURCES ${SOURCES}
  "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/process_code.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/read_symbol.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/state.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_alignment.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_code.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_header.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/write_remainder.c"
)
set (SOURCES ${SOURCES} PARENT_SCOPE)
