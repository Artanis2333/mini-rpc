configure_file(config.h.in config.h)

add_protobuf_file(protobuf-options options.proto)
add_mrpc_file(mrpc-options options.proto)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/message SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/service SOURCE_FILES)

add_library(mrpc STATIC ${SOURCE_FILES})
add_dependencies(mrpc mrpc-options-gen-files)
target_link_libraries(mrpc mrpc-options)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN "*.proto")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mrpc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/options.mrpc.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mrpc)
install(TARGETS mrpc DESTINATION ${CMAKE_INSTALL_LIBDIR})
