set(PIFACEDIGITAL_BIN pifacedigital)

set(PIFACEDIGITAL_SRCS
    init.cpp
    PFDigitalModule.cpp
    PFDigitalPin.cpp
)


## test to fix clion
set(PIFACEDIGITAL_INCL
PFDigitalModule.hpp
PFDigitalPin.hpp
)

add_library(${PIFACEDIGITAL_BIN} SHARED ${PIFACEDIGITAL_SRCS})

set_target_properties(${PIFACEDIGITAL_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${PIFACEDIGITAL_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
