function(add_protobuf_file TARGET FILE_PATH)
    get_filename_component(FILE_DIR ${FILE_PATH} PATH)
    get_filename_component(FILE_NAME ${FILE_PATH} NAME_WLE)

    if(NOT FILE_DIR)
        set(PROTOBUF_GEN_HEADER_FILE ${FILE_NAME}.pb.h)
        set(PROTOBUF_GEN_SOURCE_FILE ${FILE_NAME}.pb.cc)
    else()
        set(PROTOBUF_GEN_HEADER_FILE ${FILE_DIR}/${FILE_NAME}.pb.h)
        set(PROTOBUF_GEN_SOURCE_FILE ${FILE_DIR}/${FILE_NAME}.pb.cc)
    endif()

    add_custom_command(
        OUTPUT ${PROTOBUF_GEN_HEADER_FILE} ${PROTOBUF_GEN_SOURCE_FILE}
        COMMAND ${PROTOC} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR} ${FILE_PATH}
        DEPENDS ${FILE_PATH}
        )
    add_custom_target(${TARGET}-gen-files DEPENDS ${PROTOBUF_GEN_HEADER_FILE} ${PROTOBUF_GEN_SOURCE_FILE})
    add_library(${TARGET} OBJECT ${PROTOBUF_GEN_SOURCE_FILE})
    target_include_directories(${TARGET} PRIVATE ${PROTOBUF_INSTALL_INCLUDEDIR})
    target_compile_options(${TARGET} PRIVATE -Wno-all -Wno-extra -Wno-error)
endfunction()

function(add_mrpc_file TARGET FILE_PATH)
    get_filename_component(FILE_DIR ${FILE_PATH} PATH)
    get_filename_component(FILE_NAME ${FILE_PATH} NAME_WLE)

    if(NOT FILE_DIR)
        set(MRPC_GEN_HEADER_FILE ${FILE_NAME}.mrpc.h)
        set(MRPC_GEN_SOURCE_FILE ${FILE_NAME}.mrpc.cpp)
    else()
        set(MRPC_GEN_HEADER_FILE ${FILE_DIR}/${FILE_NAME}.mrpc.h)
        set(MRPC_GEN_SOURCE_FILE ${FILE_DIR}/${FILE_NAME}.mrpc.cpp)
    endif()

    add_custom_command(
        OUTPUT ${MRPC_GEN_HEADER_FILE} ${MRPC_GEN_SOURCE_FILE}
        COMMAND ${PROTOC} --plugin=${CMAKE_BINARY_DIR}/protoc_plugin/protoc-gen-mrpc_cpp --mrpc_cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=${CMAKE_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR} ${FILE_PATH}
        DEPENDS protoc-gen-mrpc_cpp ${FILE_PATH}
        )
    add_custom_target(${TARGET}-gen-files DEPENDS ${MRPC_GEN_HEADER_FILE} ${MRPC_GEN_SOURCE_FILE})
    add_library(${TARGET} OBJECT ${MRPC_GEN_SOURCE_FILE})
endfunction()
