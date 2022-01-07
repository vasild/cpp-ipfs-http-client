
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

set(TESTS
  test_block
  test_config
  test_dht
  test_files
  test_generic
  test_key
  test_name
  test_object
  test_pin
  test_stats
  test_swarm
  test_threading
)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
  set(TESTS
    ${TESTS}
    test_error
  )
endif()

foreach (T ${TESTS})
  add_executable(${T} ${T}.cc)
  target_link_libraries(${T} ${IPFS_API_LIBNAME} Threads::Threads)

  add_test(NAME ${T} COMMAND ./${T})
endforeach()

# Instead of running 'make test', this will only run our testcases. Used the ctest_coverage_* targets as well.
add_custom_target(our_tests
  COMMAND ctest -R '^test_'
)

# Additional targets
if(COVERAGE)
  include(CodeCoverage)

  # Create a new target 'ctest_coverage_xml' that generates an XML coverage file
  setup_target_for_coverage_gcovr_xml(
    NAME ctest_coverage_xml
    EXECUTABLE make our_tests
    DEPENDENCIES ${TESTS}
    EXCLUDE "test/*" "build/_deps" "include/*" "build/CMakeFiles/*"
  )

  # Create a new target 'ctest_coverage_html' that generates a HTML page
  setup_target_for_coverage_gcovr_html(
    NAME ctest_coverage_html
    EXECUTABLE make our_tests
    DEPENDENCIES ${TESTS}
    EXCLUDE "test/*" "build/_deps" "include/*" "build/CMakeFiles/*"
  )
endif()