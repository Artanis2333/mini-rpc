set(TARGET mrpc)

add_protobuf_file(protobuf-options options.proto)
add_mrpc_file(mrpc-options options.proto)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/message SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/service SOURCE_FILES)

add_library(${TARGET} STATIC ${SOURCE_FILES})
add_dependencies(${TARGET} mrpc-options-gen-files)
