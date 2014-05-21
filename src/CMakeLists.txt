set(OSAC_BIN osac)

set(OSAC_SRCS
    main.cpp
    osac.cpp
    runtimeoptions.cpp
    config/xmlconfig.cpp
    core/authrequest.cpp
    core/core.cpp
    core/event.cpp
    dynlib/dynamiclibrary.cpp
    hardware/hwmanager.cpp
    hardware/device/wiegandinterface.cpp
    hardware/device/gpio/gpiomanager.cpp
    hardware/device/gpio/gpio.cpp
    signal/signalhandler.cpp
    tools/version.cpp
    tools/unixsyscall.cpp
    tools/unixfs.cpp
)

add_executable(${OSAC_BIN} ${OSAC_SRCS})

set(OSAC_COMPILE_FLAGS "${CMAKE_CXX_FLAGS}")

# Add debug flags
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(OSAC_COMPILE_FLAGS "${OSAC_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef")
    endif()
endif()

#Set compilation flags for current target
set_target_properties(${OSAC_BIN} PROPERTIES
    COMPILE_FLAGS "${OSAC_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${OSAC_BIN} dl pthread ${Boost_LIBRARIES})

# Build modules
if(NOT NO_MODULE)
    add_subdirectory(modules)
endif()
