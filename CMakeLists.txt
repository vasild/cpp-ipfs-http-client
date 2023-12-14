cmake_minimum_required(VERSION 3.12.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Adhere the version number to http://semver.org/
project(cpp-ipfs-http-client
  VERSION 0.7.0
  LANGUAGES CXX)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# Compile in C++20 mode
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(AppleClang|Clang|GNU)$" && NOT COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pedantic -Werror=incompatible-pointer-types")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Werror")
endif()

# Generate compile_commands.json, to be used by YouCompleteMe.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Set the available options 
option(DOC "Build Doxygen" OFF)
option(COVERAGE "Enable generation of coverage info" OFF)
option(BUILD_TESTING "Enable building test cases" ON)

# Find curl
# Look for static import symbols for Windows builds
if(WIN32)
    add_definitions("-DCURL_STATICLIB")
endif()
find_package(CURL REQUIRED)

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

# Add include directories
target_include_directories(${IPFS_API_LIBNAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CURL_INCLUDE_DIRS}
)

# Fetch "JSON for Modern C++"
include(FetchContent)
# Retrieve Nlohmann JSON
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# libcurl requires additional libs only for static Windows builds
if(WIN32)
   set(WINDOWS_CURL_LIBS idn2 unistring iconv charset ssh2 gcrypt gpg-error ws2_32 advapi32 crypt32 wldap32)
endif()

set_target_properties(${IPFS_API_LIBNAME} PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
)
target_link_libraries(${IPFS_API_LIBNAME} ${CURL_LIBRARIES} ${WINDOWS_CURL_LIBS} nlohmann_json::nlohmann_json)
if(NOT DISABLE_INSTALL)
  install(TARGETS ${IPFS_API_LIBNAME} DESTINATION lib)
  install(FILES include/ipfs/client.h DESTINATION include/ipfs)
  install(FILES include/ipfs/http/transport.h DESTINATION include/ipfs/http)
  install(FILES ${json_SOURCE_DIR}/include/nlohmann/json.hpp DESTINATION include/nlohmann)
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
