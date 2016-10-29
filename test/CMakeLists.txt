set(TESTS
  config
  generic
  simple
)

foreach (T ${TESTS})
  add_executable(${T} ${T}.cc)
  target_link_libraries(${T} ${IPFS_API_LIBNAME})

  add_test(NAME ${T} COMMAND ./${T})
endforeach()
