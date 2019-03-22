find_package (Asciidoc REQUIRED)

set (SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/lzws.1.txt")
set (RESULT "${CMAKE_CURRENT_BINARY_DIR}/lzws.1")

set (MAN_TARGET "${LZWS_TARGET}-man")

add_custom_command (
  OUTPUT  ${RESULT}
  COMMAND ${CMAKE_ASCIIDOC_A2X_BINARY}
    --doctype=manpage
    --format=manpage
    --destination-dir="${CMAKE_CURRENT_BINARY_DIR}"
    "${SOURCE}"
  DEPENDS ${SOURCE}
)
add_custom_target (${MAN_TARGET} ALL DEPENDS ${RESULT})

install (FILES ${RESULT} DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
