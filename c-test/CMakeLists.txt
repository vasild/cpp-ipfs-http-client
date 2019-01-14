cmake_minimum_required(VERSION 3.1.0)

# Compile in C++11 mode
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compiler warnings
if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS} -Wall -Wpedantic -Wextra -Wno-discarded-qualifiers")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_C_FLAGS "-Wno-unused-command-line-argument -Wno-incompatible-pointer-types-discards-qualifiers")
endif()

# Generate compile_commands.json, to be used by YouCompleteMe.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find curl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

include_directories("${CMAKE_SOURCE_DIR}/../include")

link_directories("${CMAKE_SOURCE_DIR}/..")

set(HIVE_API_LIBNAME hive-api)
set(HIVE_CAPI_LIBNAME hive-capi)
set(TESTS
  hive_random_host
  hive_generate_conf
  hive_refresh_conf
)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
  set(TESTS
    ${TESTS}
    error
  )
endif()

foreach (T ${TESTS})
  add_executable(${T} ${T}.c)
  target_link_libraries(${T} ${HIVE_CAPI_LIBNAME} ${HIVE_API_LIBNAME} ${CURL_LIBRARIES} "-lstdc++")


  add_test(NAME ${T} COMMAND ./${T})
endforeach()
