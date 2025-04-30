include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_mrpc_file(mrpc-message_example message_example.proto)

add_executable(json json.cpp)
add_dependencies(json mrpc-message_example-gen-files)
target_link_libraries(json mrpc-message_example mrpc_message)

add_executable(stl stl.cpp)
add_dependencies(stl mrpc-message_example-gen-files)
target_link_libraries(stl mrpc-message_example mrpc_message)
