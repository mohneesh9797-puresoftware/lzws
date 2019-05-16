set (SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.c")

include (DefineRelativeFilePaths)
cmake_define_relative_file_paths (${SOURCES})

if (LZWS_SHARED MATCHES ON)
  set (CLI_TARGET "${LZWS_TARGET}-cli")
  set (CLI_NAME ${LZWS_TARGET})

  add_executable (${CLI_TARGET} ${SOURCES})
  target_link_libraries (${CLI_TARGET} ${LIB_TARGET})

  set_target_properties (${CLI_TARGET} PROPERTIES OUTPUT_NAME ${CLI_NAME})
  install (TARGETS ${CLI_TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

if (LZWS_STATIC MATCHES ON)
  set (STATIC_CLI_TARGET "${LZWS_TARGET}-static-cli")
  set (STATIC_CLI_NAME "${LZWS_TARGET}-static")

  add_executable (${STATIC_CLI_TARGET} ${SOURCES})
  target_link_libraries (${STATIC_CLI_TARGET} ${STATIC_LIB_TARGET})

  set_target_properties (${STATIC_CLI_TARGET} PROPERTIES OUTPUT_NAME ${STATIC_CLI_NAME})
  install (TARGETS ${STATIC_CLI_TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()
