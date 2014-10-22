set(LOGGER_BIN logger)

set(LOGGER_SRCS
    init.cpp
    Logger.cpp
)

add_library(${LOGGER_BIN} SHARED ${LOGGER_SRCS})

set_target_properties(${LOGGER_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
   )

install(TARGETS ${LOGGER_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
