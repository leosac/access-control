set(WEBSOCK_API_BIN websock-api)

set(WEBSOCK_API_SRCS
        init.cpp
        WebSockAPI.cpp
        WSServer.cpp
        api/api.cpp
        api/APIAuth.cpp
        )

add_library(${WEBSOCK_API_BIN} SHARED
        ${WEBSOCK_API_SRCS}
        )

set_target_properties(${WEBSOCK_API_BIN} PROPERTIES
        COMPILE_FLAGS "-std=c++11 -Wno-unknown-pragmas"
        )

target_link_libraries(${WEBSOCK_API_BIN}
        ${Boost_LIBRARIES}
        )

target_include_directories(${WEBSOCK_API_BIN}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/deps/websocketpp
        ${CMAKE_SOURCE_DIR}/deps/json/src
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
        DATABASE_MYSQL
        )

install(TARGETS ${WEBSOCK_API_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
