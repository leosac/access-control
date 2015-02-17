set(WIEGAND_BIN wiegand)

set(WIEGAND_SRCS
    wiegand.cpp
    WiegandReaderImpl.cpp
    strategies/SimpleWiegandStrategy.cpp
    strategies/WiegandPin4BitsOnly.cpp
    strategies/WiegandPin8BitsOnly.cpp
    strategies/WiegandPinBuffered.cpp
    strategies/WiegandCardAndPin.cpp
)

add_library(${WIEGAND_BIN} SHARED ${WIEGAND_SRCS})

set_target_properties(${WIEGAND_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${WIEGAND_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
