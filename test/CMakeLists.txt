
include(CodeCoverage)
append_coverage_compiler_flags()

set(TESTS
  test_block
  test_config
  test_dht
  test_files
  test_key
  test_generic
  test_name
  test_object
  test_pin
  test_swarm
)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
  set(TESTS
    ${TESTS}
    test_error
  )
endif()

# we need to turn off optimization for non-skewed coverage reports
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

foreach (T ${TESTS})
  add_executable(${T} ${T}.cc)
  target_link_libraries(${T} ${IPFS_API_LIBNAME})

  add_test(NAME ${T} COMMAND ./${T})
endforeach()

# ctest -R '^test_block'

setup_target_for_coverage_gcovr_xml(
  NAME ctest_coverage_xml
  EXECUTABLE test_block
  DEPENDENCIES ${TESTS}
  EXCLUDE "build/_deps" "include/*"
)

setup_target_for_coverage_gcovr_html(
  NAME ctest_coverage_html
  EXECUTABLE test_files 
  DEPENDENCIES ${TESTS}
  EXCLUDE "build/_deps" "include/*" "build/CMakeFiles/3.16.3/CompilerIdCXX/CMakeCXXCompilerId.cpp"
)