function (cmake_check_ipo)
  if (DEFINED CMAKE_HAVE_IPO)
    return ()
  endif ()

  include (CheckIPOSupported)

  check_ipo_supported (RESULT CHECK_RESULT OUTPUT CHECK_OUTPUT_RESULT)
  if (CMAKE_VERBOSE_MAKEFILE)
    message (STATUS ${CHECK_OUTPUT_RESULT})
  endif ()

  if (CHECK_RESULT)
    set (CMAKE_HAVE_IPO true CACHE STRING "status of IPO support")
    message (STATUS "Status of IPO support - yes")
    return ()
  endif ()

  set (CMAKE_HAVE_IPO false CACHE STRING "status of IPO support")
  message (STATUS "Status of IPO support - no")
endfunction ()
