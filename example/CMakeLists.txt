cmake_minimum_required(VERSION 3.15)

project(example)

set(MINI_PPC_INSTALL_PATH /usr/local/mini-rpc)
if(DEFINED ENV{MINI_PPC_INSTALL_PATH})
    set(MINI_PPC_INSTALL_PATH $ENV{MINI_PPC_INSTALL_PATH})
endif()

set(CMAKE_BUILD_TYPE Debug)

include(${MINI_PPC_INSTALL_PATH}/cmake/mrpc.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(message)
add_subdirectory(service)
