set(TESTS
  generic
  simple
)

foreach (T ${TESTS})
  add_executable(${T} ${T}.cc)
  target_link_libraries(${T} ${IPFS_API_LIBNAME})
endforeach()
