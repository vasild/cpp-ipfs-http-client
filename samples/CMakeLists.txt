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

# Find "JSON for Modern C++" (nlohmann/json.hpp)
find_path(JSON_FOR_MODERN_CXX_INCLUDE_DIR NAMES nlohmann/json.hpp)
mark_as_advanced(JSON_FOR_MODERN_CXX_INCLUDE_DIR)
if(NOT JSON_FOR_MODERN_CXX_INCLUDE_DIR)
  message(FATAL_ERROR "JSON for Modern C++ (nlohmann/json.hpp) not found. Go fetch it from https://github.com/nlohmann/json/blob/develop/src/json.hpp and tell CMake where it is: 'cmake -DJSON_FOR_MODERN_CXX_INCLUDE_DIR:PATH=/path/to/include' (assuming the file is in /path/to/include/nlohmann/json.hpp)")
endif()
include_directories(${JSON_FOR_MODERN_CXX_INCLUDE_DIR})

include_directories("${CMAKE_SOURCE_DIR}/../include")

link_directories("${CMAKE_SOURCE_DIR}/..")

set(HIVE_API_LIBNAME hive-api++)

add_executable(deploy_message_service deploy_message_service.cc)
target_link_libraries(deploy_message_service ${HIVE_API_LIBNAME} ${CURL_LIBRARIES})

add_executable(send_message send_message.cc)
target_link_libraries(send_message ${HIVE_API_LIBNAME} ${CURL_LIBRARIES})

add_executable(read_message read_message.cc)
target_link_libraries(read_message ${HIVE_API_LIBNAME} ${CURL_LIBRARIES})
