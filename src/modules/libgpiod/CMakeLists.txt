set(LIBGPIOD_BIN libgpiod)

set(LIBGPIOD_SRCS
    init.cpp
    LibgpiodModule.cpp
    LibgpiodPin.cpp
    LibgpiodConfig.cpp
)

add_library(${LIBGPIOD_BIN} SHARED ${LIBGPIOD_SRCS})

set_target_properties(${LIBGPIOD_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${LIBGPIOD_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
