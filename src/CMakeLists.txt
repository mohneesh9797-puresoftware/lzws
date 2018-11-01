configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

set (HEADERS "config.h" "common.h" "compressor.h" "decompressor.h")
set (SOURCES "compressor.c" "decompressor.c")

install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set (LIB_NAME ${LZWS_TARGET})
set (LIB_STATIC_NAME "${LZWS_TARGET}-static")

if (LZWS_SHARED)
  add_library (${LIB_NAME} SHARED ${SOURCES})
  install (TARGETS ${LIB_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (LZWS_STATIC)
  add_library (${LIB_STATIC_NAME} STATIC ${SOURCES})
  set_target_properties (${LIB_STATIC_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})
  install (TARGETS ${LIB_STATIC_NAME} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

# set (SOURCES "main.c")
#
# set (CLI_NAME "${LZWS_TARGET}-cli")
# set (CLI_STATIC_NAME "${LZWS_TARGET}-cli-static")
#
# if (LZWS_CLI AND LZWS_SHARED)
#   add_executable (${CLI_NAME} ${SOURCES})
#   target_link_libraries (${CLI_NAME} ${LIB_NAME})
#   install (TARGETS ${CLI_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# endif ()
#
# if (LZWS_CLI AND LZWS_STATIC)
#   add_executable (${CLI_STATIC_NAME} ${SOURCES})
#   target_link_libraries (${CLI_STATIC_NAME} ${LIB_STATIC_NAME})
#   install (TARGETS ${CLI_STATIC_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# endif ()
