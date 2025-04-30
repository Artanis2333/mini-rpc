cmake_minimum_required(VERSION 3.15)

project(mini-rpc VERSION 0.3.0)

include(GNUInstallDirs)
include(function.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX _d)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_options(-Wall -Wextra -Werror)
else()
    add_compile_options(-Wall -Wextra)
endif()

# MiniRPC
set(MINI_PPC_INSTALL_PATH /usr/local/mini-rpc)
if(DEFINED ENV{MINI_PPC_INSTALL_PATH})
    set(MINI_PPC_INSTALL_PATH $ENV{MINI_PPC_INSTALL_PATH})
endif()

# Protobuf
set(PROTOBUF_INSTALL_PATH ${MINI_PPC_INSTALL_PATH}/3party/protobuf)
set(PROTOC ${PROTOBUF_INSTALL_PATH}/bin/protoc)
set(PROTOBUF_INSTALL_INCLUDEDIR ${PROTOBUF_INSTALL_PATH}/include)
set(PROTOBUF_INSTALL_LIBDIR ${PROTOBUF_INSTALL_PATH}/lib)

option(BUILD_TEST "Build test cases" OFF)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(protoc_plugin)
add_subdirectory(mrpc)

if(BUILD_TEST)
    add_subdirectory(test)
endif()

install(DIRECTORY cmake DESTINATION . FILES_MATCHING PATTERN "*.cmake")
install(FILES LICENSE DESTINATION .)
