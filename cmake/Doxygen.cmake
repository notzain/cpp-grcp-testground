function(enable_doxygen)
  find_package(Doxygen)

  if (DOXYGEN_FOUND)
      # set input and output files
      set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
      set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

      # request to configure the file
      configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
      message("Doxygen build started")

      # Note: do not put "ALL" - this builds docs together with application EVERY TIME!
      add_custom_target( docs
          COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
          COMMENT "Generating API documentation with Doxygen"
          VERBATIM )
  else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif (DOXYGEN_FOUND)

  find_program(SPHINX_EXECUTABLE
             NAMES sphinx-build
             DOC "Path to sphinx-build executable")

include(FindPackageHandleStandardArgs)

#Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(Sphinx
                                  "Failed to find sphinx-build executable"
                                  SPHINX_EXECUTABLE)
endfunction()