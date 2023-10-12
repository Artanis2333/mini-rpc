configure_file(config.h.in config.h)

add_protobuf_file(protobuf-options options.proto)
add_mrpc_file(mrpc-options options.proto)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/message SOURCE_FILES)
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/service SOURCE_FILES)

add_library(mrpc STATIC ${SOURCE_FILES})
add_dependencies(mrpc mrpc-options-gen-files)
target_link_libraries(mrpc mrpc-options)

install_header_files(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION include/mrpc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/options.mrpc.h DESTINATION include/mrpc)
install(TARGETS mrpc DESTINATION lib)
