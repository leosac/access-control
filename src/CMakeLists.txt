set(LEOSAC_BIN leosac)
set(LEOSAC_LIB leosac_lib)

set(LEOSAC_SRCS
    main.cpp
)

set(LEOSAC_LIB_SRCS
    core/kernel.cpp
    core/config/ConfigManager.cpp
    core/config/RemoteConfigCollector.cpp
    core/RemoteControl.cpp
    core/RemoteControlSecurity.cpp
    core/zmodule_manager.cpp
    core/MessageBus.cpp
    core/Scheduler.cpp
    core/tasks/Task.cpp
    core/netconfig/networkconfig.cpp
    core/auth/Auth.cpp
    core/auth/Group.cpp
    core/auth/ProfileMerger.cpp
    core/auth/WiegandCard.cpp
    core/auth/WiegandCardPin.cpp
    core/auth/SimpleAccessProfile.cpp
    core/auth/BaseAuthSource.cpp
    core/auth/AuthTarget.cpp
    core/auth/Interfaces/IUser.cpp
    core/auth/AuthSourceBuilder.cpp
    core/auth/CredentialValidity.cpp
    dynlib/dynamiclibrary.cpp
    modules/BaseModule.cpp
    exception/ExceptionsTools.cpp
    hardware/FGPIO.cpp
    hardware/FLED.cpp
    hardware/FWiegandReader.cpp
    tools/leosac.cpp
    tools/runtimeoptions.cpp
    tools/signalhandler.cpp
    tools/unixshellscript.cpp
    tools/unixsyscall.cpp
    tools/unixfilewatcher.cpp
    tools/unixfs.cpp
    tools/version.cpp
    tools/Schedule.cpp
    tools/XmlPropertyTree.cpp
    tools/XmlScheduleLoader.cpp
    tools/gettid.cpp
    core/tasks/SyncFrom.cpp
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cotire)
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
if (CMAKE_BUILD_TYPE MATCHES "Release")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef -O2")
    endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/libpifacedigital/src/
${CMAKE_SOURCE_DIR}/libmcp23s17/src/
${CMAKE_SOURCE_DIR}/zmqpp/src/
${CMAKE_SOURCE_DIR}/libzmq/include
${CMAKE_SOURCE_DIR}/spdlog/include
)

#Set compilation flags for current target
set_target_properties(${LEOSAC_BIN} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

set_target_properties(${LEOSAC_LIB} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

target_link_libraries(${LEOSAC_BIN} ${LEOSAC_LIB})
target_link_libraries(${LEOSAC_LIB} dl pthread zmqpp ${Boost_LIBRARIES})

set_target_properties(${LEOSAC_LIB} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "my_pch.hpp")
cotire(${LEOSAC_LIB})
set_target_properties(${LEOSAC_BIN} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "my_pch.hpp")
cotire(${LEOSAC_BIN})

install(TARGETS ${LEOSAC_BIN} DESTINATION bin)
install(TARGETS ${LEOSAC_LIB} DESTINATION lib)

# Build modules
if(LEOSAC_BUILD_MODULES)
    add_subdirectory(modules)
endif()
