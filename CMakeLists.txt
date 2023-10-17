cmake_minimum_required(VERSION 3.0)

project(mini-rpc VERSION 0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -Werror)

include(function.cmake)

# MiniRPC 
set(MINI_PPC_INSTALL_PATH /usr/local/mini-rpc)
if(DEFINED ENV{MINI_PPC_INSTALL_PATH})
    set(MINI_PPC_INSTALL_PATH $ENV{MINI_PPC_INSTALL_PATH})
endif()

# Protobuf
set(PROTOBUF_INSTALL_PATH /usr/local/protobuf)
if(DEFINED ENV{PROTOBUF_INSTALL_PATH})
    set(PROTOBUF_INSTALL_PATH $ENV{PROTOBUF_INSTALL_PATH})
endif()
message(STATUS "PROTOBUF_INSTALL_PATH: ${PROTOBUF_INSTALL_PATH}")
set(PROTOC ${PROTOBUF_INSTALL_PATH}/bin/protoc)
set(PROTOBUF_INSTALL_INCLUDEDIR ${PROTOBUF_INSTALL_PATH}/include)
set(PROTOBUF_INSTALL_LIBDIR ${PROTOBUF_INSTALL_PATH}/lib)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(protoc_plugin)
add_subdirectory(mrpc)

# Build test
option(BUILD_TEST "Build test cases" OFF)

IF(BUILD_TEST)
add_subdirectory(test)
ENDIF()
