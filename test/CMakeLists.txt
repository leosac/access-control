find_package(GTest REQUIRED)

list(APPEND TEST_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/src ${GTEST_INCLUDE_DIRS})

add_executable(mytest test.cpp)
target_link_libraries(mytest leosac_lib ${GTEST_BOTH_LIBRARIES})
set_target_properties(mytest PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${TEST_INCLUDE_DIRECTORIES}")

add_test(NAME mytest COMMAND ${LEOSAC_BINARY_DIR}/mytest)
