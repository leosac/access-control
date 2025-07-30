set(AUTH-DB_BIN auth-db)

set(AUTH-DB_SRCS
        init.cpp
        AuthDBModule.cpp
        AuthDBInstance.cpp
        )

add_library(${AUTH-DB_BIN} SHARED
        ${AUTH-DB_SRCS}
        )

set_target_properties(${AUTH-DB_BIN} PROPERTIES
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
        )

find_library(LEOSAC_DB_LIB leosac_db PATHS /home/pi/leosac/build NO_DEFAULT_PATH)
find_library(LEOSAC_LIB_LIB leosac_lib PATHS /home/pi/leosac/build NO_DEFAULT_PATH)

target_link_libraries(${AUTH-DB_BIN}
        ${Boost_LIBRARIES}
        ${LEOSAC_DB_LIB}
        ${LEOSAC_LIB_LIB}
        )

target_include_directories(${AUTH-DB_BIN}
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

install(TARGETS ${AUTH-DB_BIN}
        LIBRARY DESTINATION lib/leosac/modules
        )