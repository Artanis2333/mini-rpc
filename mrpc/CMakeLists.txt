include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(config.h.in config.h)

add_protobuf_file(protobuf-options options.proto)
add_protobuf_file(protobuf-common message/common.proto)

add_mrpc_file(mrpc-options options.proto)
add_mrpc_file(mrpc-error_code error_code.proto)
add_mrpc_file(mrpc-common message/common.proto)
add_mrpc_file(mrpc-application_config service/application_config.proto)
add_mrpc_file(mrpc-protocol service/protocol.proto)
add_mrpc_file(mrpc-log util/log.proto)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/message MESSAGE_SOURCE_FILES)

add_library(mrpc_message STATIC ${MESSAGE_SOURCE_FILES})
target_include_directories(mrpc_message PRIVATE ${MINI_PPC_INSTALL_PATH}/3party/nlohmann/include)
add_dependencies(mrpc_message mrpc-common-gen-files)
target_link_libraries(mrpc_message PUBLIC mrpc-common)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/util UTIL_SOURCE_FILES)

add_library(mrpc_util STATIC ${UTIL_SOURCE_FILES})
add_dependencies(mrpc_util mrpc-log-gen-files)
target_link_libraries(mrpc_util PUBLIC mrpc-log)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/message SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/service SOURCE_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/util SOURCE_FILES)

add_library(mrpc STATIC ${SOURCE_FILES})
add_dependencies(mrpc mrpc-options-gen-files mrpc-error_code-gen-files mrpc-common-gen-files mrpc-application_config-gen-files mrpc-protocol-gen-files mrpc-log-gen-files)
target_include_directories(mrpc PRIVATE ${MINI_PPC_INSTALL_PATH}/3party/libuv/include)
target_include_directories(mrpc PRIVATE ${MINI_PPC_INSTALL_PATH}/3party/nlohmann/include)
target_link_directories(mrpc INTERFACE ${MINI_PPC_INSTALL_PATH}/3party/libuv/lib)
target_link_libraries(mrpc PUBLIC mrpc-options mrpc-error_code mrpc-common mrpc-application_config mrpc-protocol mrpc-log uv pthread dl)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN "*.proto")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mrpc)
install(TARGETS mrpc_message DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS mrpc_util DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS mrpc DESTINATION ${CMAKE_INSTALL_LIBDIR})

file(GLOB_RECURSE MRPC_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/*.mrpc.h")
foreach(header ${MRPC_HEADERS})
    file(RELATIVE_PATH relative_path "${CMAKE_CURRENT_BINARY_DIR}" "${header}")
    get_filename_component(target_path "${relative_path}" DIRECTORY)
    install(FILES "${header}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mrpc/${target_path}")
endforeach()
