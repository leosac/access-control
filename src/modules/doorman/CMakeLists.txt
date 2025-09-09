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

find_library(LEOSAC_DB_LIB leosac_db PATHS ${CMAKE_BINARY_DIR} NO_DEFAULT_PATH)
find_library(LEOSAC_LIB_LIB leosac_lib PATHS ${CMAKE_BINARY_DIR} NO_DEFAULT_PATH)

if(LEOSAC_DB_LIB AND LEOSAC_LIB_LIB)
    target_link_libraries(${DOORMAN_BIN}
            ${LEOSAC_DB_LIB}
            ${LEOSAC_LIB_LIB}
            )
else()
    target_link_libraries(${DOORMAN_BIN}
            leosac_db
            leosac_lib
            )
endif()

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
