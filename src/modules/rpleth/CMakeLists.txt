set(RPLETH_BIN rpleth)

set(RPLETH_SRCS
    init.cpp
    rplethauth.cpp
    rplethpacket.cpp
    rplethprotocol.cpp
    network/unixsocket.cpp
)

add_library(${RPLETH_BIN} SHARED ${RPLETH_SRCS})

#Add symbol exporting options flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

target_link_libraries(${RPLETH_BIN})

install(TARGETS ${RPLETH_BIN} LIBRARY DESTINATION bin)
