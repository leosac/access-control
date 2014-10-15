set(LED_BIN led)

set(LED_SRCS
    LEDModule.cpp
    LedImpl.cpp
    init.cpp
)

add_library(${LED_BIN} SHARED ${LED_SRCS})

set_target_properties(${LED_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${LED_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
