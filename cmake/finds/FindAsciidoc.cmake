if (DEFINED CMAKE_ASCIIDOC_FOUND)
  return ()
endif ()

find_program (CMAKE_ASCIIDOC_BINARY asciidoc)
find_program (CMAKE_ASCIIDOC_A2X_BINARY a2x)

message (STATUS "Status of Asciidoc - main binary path is ${CMAKE_ASCIIDOC_BINARY}")
message (STATUS "Status of Asciidoc - a2x binary path is ${CMAKE_ASCIIDOC_A2X_BINARY}")

set (
  CMAKE_ASCIIDOC_FOUND
  (CMAKE_ASCIIDOC_BINARY AND CMAKE_ASCIIDOC_A2X_BINARY)
  CACHE STRING "Asciidoc found"
)

if (NOT CMAKE_ASCIIDOC_FOUND AND ASCIIDOC_FIND_REQUIRED)
  message (FATAL_ERROR "Asciidoc is required")
endif ()

mark_as_advanced (CMAKE_ASCIIDOC_FOUND CMAKE_ASCIIDOC_BINARY CMAKE_ASCIIDOC_A2X_BINARY)
