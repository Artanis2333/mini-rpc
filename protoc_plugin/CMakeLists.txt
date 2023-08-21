set(TARGET protoc-gen-mrpc_cpp)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)

add_executable(${TARGET} ${SOURCE_FILES})
add_dependencies(${TARGET} protobuf-options-gen-files)
target_include_directories(${TARGET} PRIVATE ${PROTOBUF_INSTALL_PATH}/include)
target_link_directories(${TARGET} PRIVATE ${PROTOBUF_INSTALL_PATH}/lib)
target_link_libraries(${TARGET} protobuf-options protoc protobuf)
