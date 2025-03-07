include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_protobuf_file(protobuf-pb pb.proto)
add_mrpc_file(mrpc-mine mine.proto)

file(GLOB UNITTEST_SOURCE_FILES "*_unittest.cpp")

add_executable(unit_test ${UNITTEST_SOURCE_FILES})
add_dependencies(unit_test mrpc-mine-gen-files)
target_include_directories(unit_test PRIVATE ${GTEST_INSTALL_PATH}/include)
target_link_directories(unit_test PRIVATE ${GTEST_INSTALL_PATH}/lib)
target_link_libraries(unit_test mrpc-mine mrpc gtest gtest_main)

add_executable(compatibility_test compatibility_test.cpp)
add_dependencies(compatibility_test protobuf-pb-gen-files)
add_dependencies(compatibility_test mrpc-mine-gen-files)
target_include_directories(compatibility_test PRIVATE ${PROTOBUF_INSTALL_INCLUDEDIR})
target_include_directories(compatibility_test PRIVATE ${GTEST_INSTALL_PATH}/include)
target_link_directories(compatibility_test PRIVATE ${PROTOBUF_INSTALL_LIBDIR})
target_link_directories(compatibility_test PRIVATE ${GTEST_INSTALL_PATH}/lib)
target_link_libraries(compatibility_test mrpc-mine mrpc protobuf-pb protobuf gtest gtest_main)
