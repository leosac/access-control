set(LEDMONITOR_BIN ledmonitor)

set(LEDMONITOR_SRCS
    ledmonitormodule.cpp
    init.cpp
)

add_library(${LEDMONITOR_BIN} SHARED ${LEDMONITOR_SRCS})

set_target_properties(${LEDMONITOR_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../..")
