get_filename_component(MINI_PPC_INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR} PATH)

set(PROTOC ${MINI_PPC_INSTALL_DIR}/3party/protobuf/bin/protoc)
set(PROTOC_CPP_PLUGIN ${MINI_PPC_INSTALL_DIR}/bin/protoc-gen-mrpc_cpp)

include_directories(${MINI_PPC_INSTALL_DIR}/include)
include_directories(${MINI_PPC_INSTALL_DIR}/3party/jsoncpp/include)

add_library(mrpc STATIC IMPORTED)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set_target_properties(mrpc PROPERTIES IMPORTED_LOCATION ${MINI_PPC_INSTALL_DIR}/lib/libmrpc_d.a)
else()
    set_target_properties(mrpc PROPERTIES IMPORTED_LOCATION ${MINI_PPC_INSTALL_DIR}/lib/libmrpc.a)
endif()

add_library(uv STATIC IMPORTED)
set_target_properties(uv PROPERTIES IMPORTED_LOCATION ${MINI_PPC_INSTALL_DIR}/3party/libuv/lib/libuv.a)

add_library(jsoncpp STATIC IMPORTED)
set_target_properties(jsoncpp PROPERTIES IMPORTED_LOCATION ${MINI_PPC_INSTALL_DIR}/3party/jsoncpp/lib/libjsoncpp.a)

target_link_libraries(mrpc INTERFACE uv jsoncpp)

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
        COMMAND ${PROTOC} --plugin=${PROTOC_CPP_PLUGIN} --mrpc_cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=${MINI_PPC_INSTALL_DIR}/include --proto_path=${CMAKE_CURRENT_SOURCE_DIR} ${FILE_PATH}
        DEPENDS ${FILE_PATH}
        )
    add_custom_target(${TARGET}-gen-files DEPENDS ${MRPC_GEN_HEADER_FILE} ${MRPC_GEN_SOURCE_FILE})
    add_library(${TARGET} OBJECT ${MRPC_GEN_SOURCE_FILE})
endfunction()
