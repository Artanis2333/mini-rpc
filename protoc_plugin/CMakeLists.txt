aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)

add_executable(protoc-gen-mrpc_cpp ${SOURCE_FILES})
add_dependencies(protoc-gen-mrpc_cpp protobuf-options-gen-files)
target_include_directories(protoc-gen-mrpc_cpp PRIVATE ${PROTOBUF_INSTALL_INCLUDEDIR})
target_link_directories(protoc-gen-mrpc_cpp PRIVATE ${PROTOBUF_INSTALL_LIBDIR})
target_link_libraries(protoc-gen-mrpc_cpp protobuf-options protoc protobuf)

install(TARGETS protoc-gen-mrpc_cpp DESTINATION ${CMAKE_INSTALL_BINDIR})
