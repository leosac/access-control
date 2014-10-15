set(DOORMAN_BIN doorman)

set(DOORMAN_SRCS
    init.cpp
    DoormanModule.cpp
    DoormanInstance.cpp
)

add_library(${DOORMAN_BIN} SHARED ${DOORMAN_SRCS})

set_target_properties(${DOORMAN_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${DOORMAN_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
