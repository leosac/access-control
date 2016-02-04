set(WEBSOCK_API_BIN websock-api)

set(WEBSOCK_API_SRCS
        init.cpp
        WebSockAPI.cpp
        WSServer.cpp
        api/api.cpp
        )

add_library(${WEBSOCK_API_BIN} SHARED ${WEBSOCK_API_SRCS})

set_target_properties(${WEBSOCK_API_BIN} PROPERTIES
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
        )
target_link_libraries(${WEBSOCK_API_BIN} ${Boost_LIBRARIES})
target_include_directories(${WEBSOCK_API_BIN}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/websocketpp
        ${CMAKE_SOURCE_DIR}/json/src
        )

install(TARGETS ${WEBSOCK_API_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
