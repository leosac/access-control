set(DOORMAN_BIN doorman)

set(DOORMAN_SRCS
    init.cpp
    DoormanModule.cpp
    DoormanInstance.cpp
    DoormanDoor.cpp
)

add_library(${DOORMAN_BIN} SHARED ${DOORMAN_SRCS})

set_target_properties(${DOORMAN_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

target_include_directories(${DOORMAN_BIN}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/modules
    ${CMAKE_SOURCE_DIR}/deps/zmqpp/src
    ${CMAKE_SOURCE_DIR}/deps/flagset
    ${CMAKE_SOURCE_DIR}/deps/date/include
    ${CMAKE_BINARY_DIR}/odb_gen
    ${CMAKE_BINARY_DIR}
    ${ZEROMQ_INCLUDE_DIR}
    ${ODB_INCLUDE_DIRS}
    ${ODB_COMPILE_OUTPUT_DIR}
    )


install(TARGETS ${DOORMAN_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
