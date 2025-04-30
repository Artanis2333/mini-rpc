aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)

add_executable(protoc-gen-mrpc-cpp ${SOURCE_FILES})
add_dependencies(protoc-gen-mrpc-cpp protobuf-options-gen-files protobuf-common-gen-files)
target_include_directories(protoc-gen-mrpc-cpp PRIVATE ${PROTOBUF_INSTALL_INCLUDEDIR})
target_link_directories(protoc-gen-mrpc-cpp PRIVATE ${PROTOBUF_INSTALL_LIBDIR})
target_link_libraries(protoc-gen-mrpc-cpp protobuf-options protoc protobuf)

install(TARGETS protoc-gen-mrpc-cpp DESTINATION ${CMAKE_INSTALL_BINDIR})
