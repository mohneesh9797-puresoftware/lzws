function (cmake_define_relative_file_paths SOURCES)
  foreach (SOURCE IN LISTS SOURCES)
    file (
      RELATIVE_PATH RELATIVE_SOURCE_PATH
      ${PROJECT_SOURCE_DIR} ${SOURCE}
    )

    set_source_files_properties (
      ${SOURCE} PROPERTIES
      COMPILE_DEFINITIONS __RELATIVE_FILE_PATH__="${RELATIVE_SOURCE_PATH}"
    )
  endforeach ()
endfunction ()
