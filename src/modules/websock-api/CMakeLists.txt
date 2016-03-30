set(WEBSOCK_API_BIN websock-api)

set(WEBSOCK_API_SRCS
        init.cpp
        WebSockAPI.cpp
        WSServer.cpp
        api/api.cpp
        api/APIAuth.cpp
        db/user.cpp
        )

find_package(ODB REQUIRED COMPONENTS sqlite)
include(${ODB_USE_FILE})

set(OdbCMake_SOURCES
        ""
        )

set(OdbCMake_ODB_HEADERS
        db/user.hpp)

## ODB database support code generation
odb_compile(OdbCMake_SOURCES FILES
        ${OdbCMake_ODB_HEADERS} DB
        sqlite
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        )

add_library(${WEBSOCK_API_BIN} SHARED ${WEBSOCK_API_SRCS} ${OdbCMake_SOURCES}
        ${OdbCMake_ODB_HEADERS})

set_target_properties(${WEBSOCK_API_BIN} PROPERTIES
        COMPILE_FLAGS "-std=c++11 -Wno-unknown-pragmas"
        )

target_link_libraries(${WEBSOCK_API_BIN}
        ${Boost_LIBRARIES}
        ${ODB_LIBRARIES}
        )

target_include_directories(${WEBSOCK_API_BIN}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/websocketpp
        ${CMAKE_SOURCE_DIR}/json/src
        ${ODB_INCLUDE_DIRS}
        ${ODB_COMPILE_OUTPUT_DIR}

        # ODB generated code use a simple #include, so it needs to be able
        # to find our header file.
        ${CMAKE_CURRENT_SOURCE_DIR}/db

        # To access the odb_gen of Leosac's core.
        ${CMAKE_BINARY_DIR}
        # And for Generated ODB code to access our headeres
        ${CMAKE_SOURCE_DIR}/src/tools/db
        )

target_compile_definitions(${WEBSOCK_API_BIN}
        PRIVATE
        DATABASE_SQLITE
        )

install(TARGETS ${WEBSOCK_API_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
