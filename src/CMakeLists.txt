set(LEOSAC_BIN leosac)
set(LEOSAC_LIB leosac_lib)

set(LEOSAC_SRCS
    main.cpp
)

set(LEOSAC_LIB_SRCS
    config/xmlconfig.cpp
    core/kernel.cpp
    core/Logger.cpp
    core/zmodule_manager.cpp
    core/MessageBus.cpp
    core/netconfig/networkconfig.cpp
    dynlib/dynamiclibrary.cpp
    modules/BaseModule.cpp
    hardware/device/FGPIO.cpp
    hardware/device/FLED.cpp
    hardware/device/FWiegandReader.cpp
    hardware/device/button.cpp
    hardware/device/buzzer.cpp
    hardware/device/dipswitch.cpp
    hardware/device/relay.cpp
    hardware/device/gpiodevice.cpp
    hardware/device/gpio/gpiomanager.cpp
    hardware/device/gpio/gpio.cpp
    hardware/device/gpio/AGPIO.cpp
    tools/leosac.cpp
    tools/runtimeoptions.cpp
    tools/signalhandler.cpp
    tools/unixshellscript.cpp
    tools/unixsyscall.cpp
    tools/unixfilewatcher.cpp
    tools/unixfs.cpp
    tools/version.cpp
)

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
${CMAKE_SOURCE_DIR}/libpifacedigital/src/
${CMAKE_SOURCE_DIR}/libmcp23s17/src/
${CMAKE_SOURCE_DIR}/zmqpp/src/
${CMAKE_SOURCE_DIR}/libzmq/include
)

#Set compilation flags for current target
set_target_properties(${LEOSAC_BIN} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

set_target_properties(${LEOSAC_LIB} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

target_link_libraries(${LEOSAC_BIN} ${LEOSAC_LIB})
target_link_libraries(${LEOSAC_LIB} dl pthread zmqpp ${Boost_LIBRARIES})

install(TARGETS ${LEOSAC_BIN} DESTINATION bin)
install(TARGETS ${LEOSAC_LIB} DESTINATION lib)

# Build modules
if(LEOSAC_BUILD_MODULES)
    add_subdirectory(modules)
endif()
