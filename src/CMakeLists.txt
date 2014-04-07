set(OSAC_BIN osac)

set(OSAC_SRCS
    core.cpp
    event.cpp
    main.cpp
    osac.cpp
    dynlib/dynamiclibrary.cpp
    hardware/hwmanager.cpp
    hardware/device/gpiomanager.cpp
    hardware/device/gpio.cpp
    hardware/device/gpiopoller.cpp
    modules/journallogger.cpp
    signal/signalhandler.cpp
    tools/version.cpp
    tools/unixsyscall.cpp
)

add_executable(${OSAC_BIN} ${OSAC_SRCS})

# Add C++11 support flags
list(APPEND CMAKE_CXX_FLAGS ${CXX11_FLAGS})

# Add debug flags
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wundef -O")
    endif()
endif()

target_link_libraries(${OSAC_BIN} dl pthread)

install(TARGETS ${OSAC_BIN} RUNTIME DESTINATION bin)

# Build modules
add_subdirectory(modules)
