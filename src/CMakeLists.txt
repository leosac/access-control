set(LEOSAC_BIN leosac)
set(LEOSAC_LIB leosac_lib)

set(LEOSAC_SRCS
    main.cpp
)

set(LEOSAC_LIB_SRCS
    config/xmlconfig.cpp
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
        set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef")
    endif()
endif()

#Set compilation flags for current target
set_target_properties(${LEOSAC_BIN} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${LEOSAC_LIB} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${LEOSAC_BIN} ${LEOSAC_LIB})
target_link_libraries(${LEOSAC_LIB} dl pthread ${Boost_LIBRARIES})

install(TARGETS ${LEOSAC_BIN} DESTINATION bin)
install(TARGETS ${LEOSAC_LIB} DESTINATION lib)

# Build modules
if(LEOSAC_BUILD_MODULES)
    add_subdirectory(modules)
endif()
