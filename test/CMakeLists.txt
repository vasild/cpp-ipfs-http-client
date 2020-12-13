
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

foreach (T ${TESTS})
  add_executable(${T} ${T}.cc)
  target_link_libraries(${T} ${IPFS_API_LIBNAME})

  add_test(NAME ${T} COMMAND ./${T})
endforeach()

# Instead of running  'make test' in the ctest_coverage_*, only run our testcases
add_custom_target(our_tests
  COMMAND ctest -R '^test_'
)

setup_target_for_coverage_gcovr_xml(
  NAME ctest_coverage_xml
  EXECUTABLE make our_tests
  DEPENDENCIES ${TESTS}
  EXCLUDE "build/_deps" "include/*" "build/CMakeFiles/*"
)

setup_target_for_coverage_gcovr_html(
  NAME ctest_coverage_html
  EXECUTABLE make our_tests
  DEPENDENCIES ${TESTS}
  EXCLUDE "build/_deps" "include/*" "build/CMakeFiles/*"
)