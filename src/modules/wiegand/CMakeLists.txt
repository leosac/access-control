set(WIEGAND_BIN wiegand)

set(WIEGAND_SRCS
    wiegand.cpp
    WiegandReaderImpl.cpp
    SimpleWiegandStrategy.cpp
    WiegandPin4BitsOnly.cpp
    WiegandPin8BitsOnly.cpp
    WiegandPinBuffered.cpp
)

add_library(${WIEGAND_BIN} SHARED ${WIEGAND_SRCS})

set_target_properties(${WIEGAND_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${WIEGAND_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
