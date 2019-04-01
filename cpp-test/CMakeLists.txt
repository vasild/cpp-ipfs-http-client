cmake_minimum_required(VERSION 3.1.0)

# Compile in C++11 mode
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Werror -Wno-unused-parameter")
endif()

# Generate compile_commands.json, to be used by YouCompleteMe.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find curl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

include_directories("${CMAKE_SOURCE_DIR}/../include")

link_directories("${CMAKE_SOURCE_DIR}/..")

set(HIVE_API_LIBNAME hive-api++)

set(TESTS
  cluster_address_list
  cluster
  cluster_id
  cluster_peers
  cluster_peer_rm
  cluster_peer_add
  cluster_version
  file_add
  file_chcid
  file_cp
  file_dups
  file_flush
  file_get
  file_ls
  file_mkdir
  file_mv
  file_read
  file_rm
  file_stat
  file_write
  block
  config
  dht
  file
  generic
  object
  pin
  swarm
  uid_info
  dstore_test
)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
  set(TESTS
    ${TESTS}
    error
  )
endif()

foreach (T ${TESTS})
  add_executable(${T} ${T}.cc)
  target_link_libraries(${T} ${HIVE_API_LIBNAME} ${CURL_LIBRARIES})

  add_test(NAME ${T} COMMAND ./${T})
endforeach()
