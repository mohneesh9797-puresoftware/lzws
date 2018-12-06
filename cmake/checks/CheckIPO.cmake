function (cmake_check_ipo)
  if (DEFINED CMAKE_HAVE_IPO)
    return ()
  endif ()

  include (CheckIPOSupported)

  check_ipo_supported (RESULT CHECK_RESULT OUTPUT CHECK_OUTPUT)

  if (CHECK_RESULT)
    set (CMAKE_HAVE_IPO true)
    message (STATUS "Status of IPO support - yes")
  else ()
    set (CMAKE_HAVE_IPO false)
    message (STATUS "Status of IPO support - no")
  endif ()

  set (CMAKE_HAVE_IPO ${CMAKE_HAVE_IPO} CACHE STRING "status of IPO support")

  mark_as_advanced (CMAKE_HAVE_IPO)
endfunction ()
