set(LEDBUZZER_BIN led-buzzer)

set(LEDBUZZER_SRCS
    LEDBuzzerModule.cpp
    LedBuzzerImpl.cpp
    init.cpp
)

add_library(${LEDBUZZER_BIN} SHARED ${LEDBUZZER_SRCS})

set_target_properties(${LEDBUZZER_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${LEDBUZZER_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
