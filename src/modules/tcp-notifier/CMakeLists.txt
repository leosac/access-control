set(TCP_NOTIFIER_BIN tcp-notifier)

set(TCP_NOTIFIER_SRCS
    init.cpp
    TcpNotifier.cpp
)

add_library(${TCP_NOTIFIER_BIN} SHARED ${TCP_NOTIFIER_SRCS})

set_target_properties(${TCP_NOTIFIER_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

target_link_libraries(${TCP_NOTIFIER_BIN})

install(TARGETS ${TCP_NOTIFIER_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
