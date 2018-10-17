set (HEADERS "common.h" "compressor.h" "decompressor.h")
set (SOURCES "common.c" "compressor.c" "decompressor.c")

install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set (LIB_NAME ${LZW_AB_TARGET})
set (LIB_STATIC_NAME "${LZW_AB_TARGET}-static")

if (LZW_AB_SHARED)
  add_library (${LIB_NAME} SHARED ${SOURCES})
  install (TARGETS ${LIB_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (LZW_AB_STATIC)
  add_library (${LIB_STATIC_NAME} STATIC ${SOURCES})
  set_target_properties (${LIB_STATIC_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})
  install (TARGETS ${LIB_STATIC_NAME} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

# set (SOURCES "main.c")
#
# set (CLI_NAME "${LZW_AB_TARGET}-cli")
# set (CLI_STATIC_NAME "${LZW_AB_TARGET}-cli-static")
#
# if (LZW_AB_CLI AND LZW_AB_SHARED)
#   add_executable (${CLI_NAME} ${SOURCES})
#   target_link_libraries (${CLI_NAME} ${LIB_NAME})
#   install (TARGETS ${CLI_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# endif ()
#
# if (LZW_AB_CLI AND LZW_AB_STATIC)
#   add_executable (${CLI_STATIC_NAME} ${SOURCES})
#   target_link_libraries (${CLI_STATIC_NAME} ${LIB_STATIC_NAME})
#   install (TARGETS ${CLI_STATIC_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# endif ()
