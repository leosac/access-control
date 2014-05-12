set(DOOR_BIN door)

set(DOOR_SRCS
    doormodule.cpp
    init.cpp
)

add_library(${DOOR_BIN} SHARED ${DOOR_SRCS})

set_target_properties(${DOOR_BIN} PROPERTIES COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}")
