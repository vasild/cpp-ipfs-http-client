cmake_minimum_required(VERSION 3.1.0)

# Adhere the version number to http://semver.org/
project(cpp-hive-api VERSION 0.1.0 LANGUAGES CXX)

# Compile in C++11 mode
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wpedantic -Wextra -Werror -Wno-unused-parameter")
endif()

# Generate compile_commands.json, to be used by YouCompleteMe.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find curl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

# Find "JSON for Modern C++" (nlohmann/json.hpp)
include_directories("include/hive")

include_directories("include")

# Targets
# cpp-hive-api

set(HIVE_API_LIBNAME hive-api++)

# To build and install a shared library: "cmake -DBUILD_SHARED_LIBS:BOOL=ON ..."
add_library(${HIVE_API_LIBNAME}
  src/node.cc
  src/cluster.cc
  src/message.cc
  src/http/transport-curl.cc
)
set_target_properties(${HIVE_API_LIBNAME} PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
)
target_link_libraries(${HIVE_API_LIBNAME} ${CURL_LIBRARIES})

install(TARGETS ${HIVE_API_LIBNAME} DESTINATION lib)
install(FILES include/hive/node.h DESTINATION include/hive)
install(FILES include/hive/message.h DESTINATION include/hive)
install(FILES include/hive/http/transport.h DESTINATION include/hive/http)
install(FILES include/hive/nlohmann/json.hpp DESTINATION include/hive/nlohmann)

# c-hive-api

set(HIVE_CAPI_LIBNAME hive-api)

# To build and install a shared library: "cmake -DBUILD_SHARED_LIBS:BOOL=ON ..."
add_library(${HIVE_CAPI_LIBNAME}
  src/c-api/c-api.cc
  src/c-api/dstorec.cc
)
set_target_properties(${HIVE_CAPI_LIBNAME} PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
)
target_link_libraries(${HIVE_CAPI_LIBNAME} ${HIVE_API_LIBNAME})

install(TARGETS ${HIVE_CAPI_LIBNAME} DESTINATION lib)
install(FILES include/hive/c-api.h DESTINATION include/hive)

# Tests, use "CTEST_OUTPUT_ON_FAILURE=1 make test" to see output from failed tests

# https://cmake.org/cmake/help/v3.0/module/CTest.html
include(CTest)

if(BUILD_TESTING)
  add_subdirectory(cpp-test)
endif()

# Samples
add_subdirectory(samples)
