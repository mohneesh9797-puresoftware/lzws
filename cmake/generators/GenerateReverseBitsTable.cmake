function (generate_reverse_bits_table)
  if (DEFINED CMAKE_REVERSE_BITS_TABLE)
    return ()
  endif ()

  include (GetVerboseFlags)
  cmake_get_verbose_flags ()

  include (CheckRunnable)
  cmake_check_runnable ()

  ;
endfunction ()
