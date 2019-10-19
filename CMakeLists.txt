cmake_minimum_required(VERSION 3.1.0)

# Adhere the version number to http://semver.org/
project(cpp-ipfs-api VERSION 0.1.1 LANGUAGES CXX)

# Compile in C++11 mode
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Werror")
endif()

# Generate compile_commands.json, to be used by YouCompleteMe.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find curl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

# Find "JSON for Modern C++" (nlohmann/json.hpp)
find_path(JSON_FOR_MODERN_CXX_INCLUDE_DIR NAMES nlohmann/json.hpp)
mark_as_advanced(JSON_FOR_MODERN_CXX_INCLUDE_DIR)
if(NOT JSON_FOR_MODERN_CXX_INCLUDE_DIR)
  message(FATAL_ERROR "JSON for Modern C++ (nlohmann/json.hpp) not found. Go fetch it from https://github.com/nlohmann/json/blob/develop/src/json.hpp and tell CMake where it is: 'cmake -DJSON_FOR_MODERN_CXX_INCLUDE_DIR:PATH=/path/to/include' (assuming the file is in /path/to/include/nlohmann/json.hpp)")
endif()
include_directories(${JSON_FOR_MODERN_CXX_INCLUDE_DIR})

include_directories("include")

# Targets

set(IPFS_API_LIBNAME ipfs-api)

# Build shared-lib by default.
add_library(${IPFS_API_LIBNAME} SHARED
  src/client.cc
  src/http/transport-curl.cc
)
set_target_properties(${IPFS_API_LIBNAME} PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
)
target_link_libraries(${IPFS_API_LIBNAME} ${CURL_LIBRARIES})
if(NOT DISABLE_INSTALL)
  install(TARGETS ${IPFS_API_LIBNAME} DESTINATION lib)
  install(FILES include/ipfs/client.h DESTINATION include/ipfs)
  install(FILES include/ipfs/http/transport.h DESTINATION include/ipfs/http)
endif()
# Tests, use "CTEST_OUTPUT_ON_FAILURE=1 make test" to see output from failed tests

# https://cmake.org/cmake/help/v3.0/module/CTest.html
include(CTest)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
