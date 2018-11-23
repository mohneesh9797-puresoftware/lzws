set (SOURCES "main.c")

set (CLI_NAME "${LZWS_TARGET}-cli")
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
