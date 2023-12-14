find_package(Doxygen REQUIRED)

# Prepare the doxyfile
configure_file(${CMAKE_SOURCE_DIR}/doxyfile.in doxyfile @ONLY)

# note the option ALL which allows to build the docs together with the application
add_custom_target(doc ALL
  COMMAND ${DOXYGEN_EXECUTABLE} doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM)
