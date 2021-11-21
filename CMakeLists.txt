cmake_minimum_required(VERSION 3.11.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Adhere the version number to http://semver.org/
project(cpp-ipfs-http-client VERSION 0.5.0 LANGUAGES CXX)

# Compile in C++11 mode
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(AppleClang|Clang|GNU)$")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Werror")
endif()

# Generate compile_commands.json, to be used by YouCompleteMe.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Set the available options 
option(DOC "Build Doxygen" OFF)
option(COVERAGE "Enable generation of coverage info" OFF)

# Find curl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

include_directories("include")

# When build with coverage, set the correct compiler flags before the targets are defined
if(COVERAGE)
  set(CMAKE_BUILD_TYPE Debug)

  include(CodeCoverage)
  append_coverage_compiler_flags()
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0")
endif()

# Targets
set(IPFS_API_LIBNAME ipfs-http-client)

# To build and install a shared library: "cmake -DBUILD_SHARED_LIBS:BOOL=ON ..."
add_library(${IPFS_API_LIBNAME}
  src/client.cc
  src/http/transport-curl.cc
)

# Be able to find the client.h header file 
#  (eg. when static linked, outside this project folder)
target_include_directories(${IPFS_API_LIBNAME}
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Fetch "JSON for Modern C++"
include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.9.1)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set_target_properties(${IPFS_API_LIBNAME} PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
)
target_link_libraries(${IPFS_API_LIBNAME} ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)
if(NOT DISABLE_INSTALL)
  install(TARGETS ${IPFS_API_LIBNAME} DESTINATION lib)
  install(FILES include/ipfs/client.h DESTINATION include/ipfs)
  install(FILES include/ipfs/http/transport.h DESTINATION include/ipfs/http)
  install(FILES ${json_SOURCE_DIR}/include/nlohmann/json.hpp DESTINATION include/nlohmann)
  install(FILES ${json_SOURCE_DIR}/include/nlohmann/json_fwd.hpp DESTINATION include/nlohmann)
endif()
# Tests, use "CTEST_OUTPUT_ON_FAILURE=1 make test" to see output from failed tests

# https://cmake.org/cmake/help/v3.0/module/CTest.html
include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

if(DOC)
  include(doxygen)
endif()
