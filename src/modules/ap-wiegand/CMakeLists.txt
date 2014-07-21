set(WIEGAND_BIN wiegand)

set(WIEGAND_SRCS
    wiegandmodule.cpp
    wiegandrequester.cpp
    init.cpp
)

add_library(${WIEGAND_BIN} SHARED ${WIEGAND_SRCS})

set_target_properties(${WIEGAND_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src")

install(TARGETS ${WIEGAND_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
