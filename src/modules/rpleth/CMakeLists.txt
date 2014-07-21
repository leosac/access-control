set(RPLETH_BIN rpleth)

set(RPLETH_SRCS
    init.cpp
    rplethauth.cpp
    rplethpacket.cpp
    rplethprotocol.cpp
    network/unixsocket.cpp
    network/circularbuffer.cpp
)

add_library(${RPLETH_BIN} SHARED ${RPLETH_SRCS})

set_target_properties(${RPLETH_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src")

install(TARGETS ${RPLETH_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
