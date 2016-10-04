cmake_minimum_required(VERSION 3.1.0)

project(cpp-ipfs-api VERSION 0.0.1 LANGUAGES CXX)

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

include_directories("include")

# Targets

set(IPFS_API_LIBNAME ipfs-api)

add_library(${IPFS_API_LIBNAME} src/ipfs-api.cc)
target_link_libraries(${IPFS_API_LIBNAME} ${CURL_LIBRARIES})

install(TARGETS ${IPFS_API_LIBNAME} DESTINATION lib)
install(FILES include/ipfs-api.h DESTINATION include)

# Tests

# https://cmake.org/cmake/help/v3.0/module/CTest.html
include(CTest)

if(BUILD_TESTING)
  add_subdirectory(test)
  add_test(NAME simpleTest COMMAND test/simple)
endif()
