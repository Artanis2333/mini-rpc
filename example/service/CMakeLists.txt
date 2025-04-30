include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_mrpc_file(mrpc-service_example service_example.proto)

add_executable(server server.cpp)
add_dependencies(server mrpc-service_example-gen-files)
target_link_libraries(server mrpc-service_example mrpc)

add_executable(sync_client sync_client.cpp)
add_dependencies(sync_client mrpc-service_example-gen-files)
target_link_libraries(sync_client mrpc-service_example mrpc)

add_executable(async_client async_client.cpp)
add_dependencies(async_client mrpc-service_example-gen-files)
target_link_libraries(async_client mrpc-service_example mrpc)
