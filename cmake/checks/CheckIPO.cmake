function (cmake_check_ipo)
  if (DEFINED CMAKE_HAVE_IPO)
    return ()
  endif ()

  include (CheckIPOSupported)

  check_ipo_supported (RESULT CHECK_RESULT OUTPUT CHECK_OUTPUT)

  set (MESSAGE_PREFIX "Status of IPO support")

  if (CHECK_RESULT)
    set (CMAKE_HAVE_IPO true)
    message (STATUS "${MESSAGE_PREFIX} - yes")
  else ()
    set (CMAKE_HAVE_IPO false)
    message (STATUS "${MESSAGE_PREFIX} - no")
  endif ()

  set (CMAKE_HAVE_IPO ${CMAKE_HAVE_IPO} CACHE STRING "Status of IPO")

  mark_as_advanced (CMAKE_HAVE_IPO)
endfunction ()
