# Google Test
set(GTEST_INSTALL_PATH $ENV{GTEST_INSTALL_PATH})
message(STATUS "Google Test install path: ${GTEST_INSTALL_PATH}")

include_directories(${GTEST_INSTALL_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

link_directories(${GTEST_INSTALL_PATH}/lib)

add_subdirectory(message)
