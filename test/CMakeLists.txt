find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR} ${CMAKE_})

add_executable(mytest test.cpp)
target_link_libraries(mytest ${GTEST_BOTH_LIBRARIES} leosac_lib)
set_target_properties(mytest PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/src)

add_test(NAME mytest COMMAND ${LEOSAC_BINARY_DIR}/mytest)
