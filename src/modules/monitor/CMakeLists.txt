set(MONITOR_BIN monitor)

set(MONITOR_SRCS
    init.cpp
    MonitorModule.cpp
)

add_library(${MONITOR_BIN} SHARED ${MONITOR_SRCS})

set_target_properties(${MONITOR_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )
#target_include_directories(${PERSISTENTMONITOR_BIN} PUBLIC ${CMAKE_SOURCE_DIR}/src "${CMAKE_SOURCE_DIR}/zmqpp/src")

install(TARGETS ${MONITOR_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
