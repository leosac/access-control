find_package(GTest REQUIRED)

list(APPEND LEOSAC_TEST_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/zmqpp/src ${GTEST_INCLUDE_DIRS})
list(APPEND LEOSAC_TEST_LIBRARIES leosac_lib ${GTEST_BOTH_LIBRARIES} )

function(leosacCreateSingleSourceTest NAME)
## module we link against
set(MODULES_LIB WIEGAND led)

    set(TEST_NAME test-${NAME})
    add_executable(${TEST_NAME} ${NAME}.cpp helper/FakeGPIO.cpp)
    set_target_properties(${TEST_NAME} PROPERTIES
        COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}"
        INCLUDE_DIRECTORIES "${LEOSAC_TEST_INCLUDE_DIRECTORIES}")
    target_link_libraries(${TEST_NAME} ${MODULES_LIB} ${LEOSAC_TEST_LIBRARIES})
    add_test(NAME ${TEST_NAME}
        COMMAND $<TARGET_FILE:${TEST_NAME}>)
    unset(TEST_NAME)
endfunction()

leosacCreateSingleSourceTest(version)
leosacCreateSingleSourceTest(automaton)
leosacCreateSingleSourceTest(WiegandReader)
leosacCreateSingleSourceTest(Led)
