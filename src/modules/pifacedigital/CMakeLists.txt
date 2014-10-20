set(PIFACEDIGITAL_BIN pifacedigital)

set(PIFACEDIGITAL_SRCS
    init.cpp
    PFDigitalModule.cpp
    PFDigitalPin.cpp
)

## Finding libpifacedigital
find_library(
    LIB_PIFACE_DIGITAL pifacedigital
    HINTS "${CMAKE_SOURCE_DIR}/libpifacedigital"
)

if(${LIB_PIFACE_DIGITAL} STREQUAL "LIB_PIFACE_DIGITAL-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'pifacedigital' library")
endif()

## Finding libmcp23s17
find_library(
    LIB_MCP23S17 mcp23s17
    HINTS "${CMAKE_SOURCE_DIR}/libmcp23s17"
)

if(${LIB_MCP23S17} STREQUAL "LIB_MCP23S17-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'mcp23s17' library")
endif()

add_library(${PIFACEDIGITAL_BIN} SHARED ${PIFACEDIGITAL_SRCS})
target_link_libraries(${PIFACEDIGITAL_BIN} ${LIB_PIFACE_DIGITAL} ${LIB_MCP23S17})


set_target_properties(${PIFACEDIGITAL_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

install(TARGETS ${PIFACEDIGITAL_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
