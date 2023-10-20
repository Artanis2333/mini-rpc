cmake_minimum_required(VERSION 3.4)

project(example)

set(MINI_PPC_INSTALL_PATH /usr/local/mini-rpc)
if(DEFINED ENV{MINI_PPC_INSTALL_PATH})
    set(MINI_PPC_INSTALL_PATH $ENV{MINI_PPC_INSTALL_PATH})
endif()

include(${MINI_PPC_INSTALL_PATH}/cmake/mrpc.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

add_mrpc_file(mrpc-example example.proto)

add_executable(example_json example_json.cpp)
add_dependencies(example_json mrpc-example-gen-files)
target_link_libraries(example_json mrpc-example mrpc)
