set(DOOR_BIN door)

set(DOOR_SRCS
    doorconfig.cpp
    doormodule.cpp
    init.cpp
)

add_library(${DOOR_BIN} SHARED ${DOOR_SRCS})

set_target_properties(${DOOR_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../..")
