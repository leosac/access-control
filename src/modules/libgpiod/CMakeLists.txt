find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGPIOD REQUIRED IMPORTED_TARGET libgpiod)

set(LIBGPIOD_BIN libgpiod)

set(LIBGPIOD_SRCS
    init.cpp
    LibgpiodModule.cpp
    LibgpiodPin.cpp
    LibgpiodConfig.cpp
)

add_library(${LIBGPIOD_BIN} SHARED ${LIBGPIOD_SRCS})

target_link_libraries(${LIBGPIOD_BIN}
    PRIVATE
    PkgConfig::LIBGPIOD
)

set_target_properties(${LIBGPIOD_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${LIBGPIOD_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
