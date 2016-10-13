set(WEBSOCK_API_BIN websock-api)

set(WEBSOCK_API_SRCS
        init.cpp
        WebSockAPI.cpp
        WSServer.cpp
        Exceptions.cpp
        api/APISession.cpp
        api/MethodHandler.cpp
        api/APIAuth.cpp
        api/LogGet.cpp
        api/PasswordChange.cpp
        api/CRUDResourceHandler.cpp
        api/GroupCRUD.cpp
        api/UserCRUD.cpp
        api/CredentialCRUD.cpp
        api/ScheduleCRUD.cpp
        api/MembershipCRUD.cpp
        api/DoorCRUD.cpp
        api/search/GroupSearch.cpp
        api/search/DoorSearch.cpp
        )

add_library(${WEBSOCK_API_BIN} SHARED
        ${WEBSOCK_API_SRCS}
        )

set_target_properties(${WEBSOCK_API_BIN} PROPERTIES
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
        )

target_link_libraries(${WEBSOCK_API_BIN}
        ${Boost_LIBRARIES}
        leosac_db
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

        # And for Generated ODB code to access our headers
        ${CMAKE_SOURCE_DIR}/src/tools/db
        )

target_compile_definitions(${WEBSOCK_API_BIN}
        PRIVATE
        DATABASE_MYSQL
        )

install(TARGETS ${WEBSOCK_API_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
