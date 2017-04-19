set(WS2_BIN ws2)

set(WS2_SRCS
        init.cpp
        WS2.cpp
        ConnectionMetadata.cpp
        MyWSServer.cpp
        Parser.cpp
        ExceptionConverter.cpp
        Exceptions.cpp
        WS2Service.cpp
        HandlerManager.cpp
        api/Common.cpp
        )

add_library(${WS2_BIN} SHARED
        ${WS2_SRCS}
        )

set_target_properties(${WS2_BIN} PROPERTIES
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
        )

target_link_libraries(${WS2_BIN}
        ${Boost_LIBRARIES}
        leosac_db
        leosac_lib
        boost_coroutine
        )

target_include_directories(${WS2_BIN}
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

target_compile_definitions(${WS2_BIN}
        PRIVATE
        DATABASE_MYSQL
        BOOST_COROUTINE_NO_DEPRECATION_WARNING
        BOOST_COROUTINES_NO_DEPRECATION_WARNING
        ${MODULE_COMPILE_DEFINITIONS}
        )

install(TARGETS ${WS2_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
