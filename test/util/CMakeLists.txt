include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB UNITTEST_SOURCE_FILES "*_unittest.cpp")

add_executable(util_unit_test ${UNITTEST_SOURCE_FILES})
target_include_directories(util_unit_test PRIVATE ${GTEST_INSTALL_PATH}/include)
target_link_directories(util_unit_test PRIVATE ${GTEST_INSTALL_PATH}/lib)
target_link_libraries(util_unit_test mrpc_util gtest gtest_main pthread)
