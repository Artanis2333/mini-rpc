cmake_minimum_required(VERSION 3.0)

project(mini-rpc VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -Werror)

include(function.cmake)

# Protobuf
set(PROTOBUF_INSTALL_PATH $ENV{PROTOBUF_INSTALL_PATH})
message(STATUS "Protobuf install path: ${PROTOBUF_INSTALL_PATH}")
set(PROTOC ${PROTOBUF_INSTALL_PATH}/bin/protoc)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(protoc_plugin)
add_subdirectory(mrpc)

# Build test
option(BUILD_TEST "Build test cases" OFF)

IF(BUILD_TEST)
add_subdirectory(test)
ENDIF()