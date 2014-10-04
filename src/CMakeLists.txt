set(LEOSAC_BIN leosac)
set(LEOSAC_LIB leosac_lib)

set(LEOSAC_SRCS
    main.cpp
)

set(LEOSAC_LIB_SRCS
    config/xmlconfig.cpp
    #config/ConfigTree.cpp
    core/kernel.cpp
    core/zmodule_manager.cpp
    core/core.cpp
    core/modulemanager.cpp
    core/netconfig/networkconfig.cpp
    core/moduleprotocol/authcommands/aauthcommand.cpp
    core/moduleprotocol/authcommands/authcmdcreaterequest.cpp
    core/moduleprotocol/authcommands/authcmddenyaccess.cpp
    core/moduleprotocol/authcommands/authcmdgrantaccess.cpp
    core/moduleprotocol/authrequest.cpp
    core/moduleprotocol/moduleprotocol.cpp
    dynlib/dynamiclibrary.cpp
    hardware/hwmanager.cpp
    hardware/device/button.cpp
    hardware/device/buzzer.cpp
    hardware/device/dipswitch.cpp
    hardware/device/gpiodevice.cpp
    hardware/device/led.cpp
    hardware/device/relay.cpp
    hardware/device/sysfsled.cpp
    hardware/device/wiegandreader.cpp
    hardware/device/gpio/gpiomanager.cpp
    hardware/device/gpio/gpio.cpp
    hardware/device/gpio/AGPIO.cpp
    hardware/device/gpio/pfdigital.cpp
    hardware/device/gpio/PFDigitalGPIO.cpp
    tools/leosac.cpp
    tools/runtimeoptions.cpp
    tools/signalhandler.cpp
    tools/unixshellscript.cpp
    tools/unixsyscall.cpp
    tools/unixfilewatcher.cpp
    tools/unixfs.cpp
    tools/version.cpp
)

if (LEOSAC_PLATFORM)
    add_subdirectory(hardware/device/platforms)
endif()


## Finding libpifacedigital
find_library(
    LIB_PIFACE_DIGITAL pifacedigital
    HINTS "${CMAKE_SOURCE_DIR}/libpifacedigital"
)

if(${LIB_PIFACE_DIGITAL} STREQUAL "LIB_PIFACE_DIGITAL-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'pifacedigital' library")
endif()

message(STATUS "Found 'pifacedigitial' at: ${LIB_PIFACE_DIGITAL}")

## Finding libmcp23s17
find_library(
    LIB_MCP23S17 mcp23s17
    HINTS "${CMAKE_SOURCE_DIR}/libmcp23s17"
)

if(${LIB_MCP23S17} STREQUAL "LIB_MCP23S17-NOTFOUND")
    message(FATAL_ERROR "Couldn't find the 'mcp23s17' library")
endif()

message(STATUS "Found 'mcp23s17' at: ${LIB_MCP23S17}")

add_executable(${LEOSAC_BIN} ${LEOSAC_SRCS})
add_library(${LEOSAC_LIB} SHARED ${LEOSAC_LIB_SRCS})

set(LEOSAC_COMPILE_FLAGS "${CMAKE_CXX_FLAGS}")

#Add GProf compile flag
if(LEOSAC_GPROF)
    set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -pg")
endif()

# Add debug flags
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef -g")
    endif()
endif()

## Add optimization flags
if (CMAKE_BUILD_TYPE MATCHES "Opti")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef -O2")
    endif()
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/libpifacedigital/src/
${CMAKE_SOURCE_DIR}/libpifacedigital/src/
${CMAKE_SOURCE_DIR}/zmqpp/src/
)

#Set compilation flags for current target
set_target_properties(${LEOSAC_BIN} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

set_target_properties(${LEOSAC_LIB} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

target_link_libraries(${LEOSAC_BIN} ${LEOSAC_LIB})
target_link_libraries(${LEOSAC_LIB} dl pthread zmqpp ${Boost_LIBRARIES} ${LIB_PIFACE_DIGITAL} ${LIB_MCP23S17})

install(TARGETS ${LEOSAC_BIN} DESTINATION bin)
install(TARGETS ${LEOSAC_LIB} DESTINATION lib)

# Build modules
if(LEOSAC_BUILD_MODULES)
    add_subdirectory(modules)
endif()
