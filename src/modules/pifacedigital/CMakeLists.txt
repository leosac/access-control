set(PIFACEDIGITAL_BIN pifacedigital)

set(PIFACEDIGITAL_SRCS
    init.cpp
    PFDigitalModule.cpp
    PFDigitalPin.cpp
)


add_subdirectory(lib)
add_library(${PIFACEDIGITAL_BIN} SHARED ${PIFACEDIGITAL_SRCS})
target_link_libraries(${PIFACEDIGITAL_BIN} module_piface_deps)


set_target_properties(${PIFACEDIGITAL_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${PIFACEDIGITAL_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
