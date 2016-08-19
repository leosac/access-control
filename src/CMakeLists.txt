set(LEOSAC_BIN leosac)
set(LEOSAC_LIB leosac_lib)

set(LEOSAC_SRCS
    main.cpp
)

set(LEOSAC_LIB_SRCS
    core/kernel.cpp
    core/CoreAPI.cpp
    core/config/ConfigManager.cpp
    core/config/RemoteConfigCollector.cpp
    core/config/ConfigChecker.cpp
    core/CoreUtils.cpp
    core/RemoteControl.cpp
    core/RemoteControlSecurity.cpp
    core/module_manager.cpp
    core/MessageBus.cpp
    core/Scheduler.cpp
    core/tasks/Task.cpp
    core/tasks/GenericTask.cpp
    core/netconfig/networkconfig.cpp
    core/auth/Auth.cpp
    core/auth/Group.cpp
    core/auth/ProfileMerger.cpp
    core/auth/WiegandCard.cpp
    core/auth/WiegandCardPin.cpp
    core/auth/SimpleAccessProfile.cpp
    core/auth/BaseAuthSource.cpp
    core/auth/AuthTarget.cpp
    core/auth/User.cpp
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
    tools/GenGuid.cpp
    tools/PropertyTreeExtractor.cpp
    tools/log.cpp
    tools/DatabaseLogSink.cpp
    tools/ElapsedTimeCounter.cpp
    tools/XmlNodeNameEnforcer.cpp
    core/tasks/GetLocalConfigVersion.cpp
    core/tasks/GetRemoteConfigVersion.cpp
    core/tasks/FetchRemoteConfig.cpp
    core/tasks/SyncConfig.cpp
    core/tasks/RemoteControlAsyncResponse.cpp
)


# ODB configuration for tools/db (LogEntry).
set(OdbCMake_ODB_HEADERS
        tools/db/LogEntry.hpp
        )

set(OdbCMake_SOURCES_TOOLS "")
odb_compile(OdbCMake_SOURCES_TOOLS
        FILES ${OdbCMake_ODB_HEADERS}
        MULTI_DATABASE dynamic
        DB mysql sqlite
        STANDARD "c++11"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME tools
        PROFILE "boost/date-time"
        INCLUDE ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
        INCLUDE_PREFIX tools/db
        INCLUDE_REGEX %tools/db/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
                      %tools/db/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        )

# ODB configuration for auth stuff (User/Group)
set(OdbCMake_ODB_HEADERS
        core/auth/User.hpp
        #core/auth/Group.hpp
        )

set(OdbCMake_SOURCES_AUTH "")
odb_compile(OdbCMake_SOURCES_AUTH
        FILES ${OdbCMake_ODB_HEADERS}
        MULTI_DATABASE dynamic
        DB mysql sqlite
        STANDARD "c++11"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME auth
        PROFILE "boost/date-time"
        INCLUDE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
        INCLUDE_PREFIX core/auth/
        INCLUDE_REGEX %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
                      %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        )

## END ODB CONFIGURATION

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
add_executable(${LEOSAC_BIN} ${LEOSAC_SRCS})

add_library(${LEOSAC_LIB} SHARED
        ${LEOSAC_LIB_SRCS}
        ${OdbCMake_SOURCES_TOOLS}
        ${OdbCMake_SOURCES_AUTH}
        )

target_include_directories(${LEOSAC_LIB}
        PUBLIC
        ${ODB_INCLUDE_DIRS}
        ${ODB_COMPILE_OUTPUT_DIR}
        )

target_compile_definitions(${LEOSAC_LIB}
        PRIVATE
        DATABASE_MYSQL
        )

set(LEOSAC_COMPILE_FLAGS "${CMAKE_CXX_FLAGS}")

#Add GProf compile flag
if(LEOSAC_GPROF)
    set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -pg")
endif()

# Add debug flags
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef -g -Wl,-z,now")
    endif()
endif()

## Add optimization flags
if (CMAKE_BUILD_TYPE MATCHES "Release")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef -O2 -Wl,-z,now")
    endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/deps/libpifacedigital/src/
${CMAKE_SOURCE_DIR}/deps/libmcp23s17/src/
${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
${CMAKE_SOURCE_DIR}/deps/libzmq/include
${CMAKE_SOURCE_DIR}/deps/spdlog/include
)

#Set compilation flags for current target
set_target_properties(${LEOSAC_BIN} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

set_target_properties(${LEOSAC_LIB} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

target_link_libraries(${LEOSAC_BIN} ${LEOSAC_LIB})
target_link_libraries(${LEOSAC_LIB} dl pthread zmqpp ${Boost_LIBRARIES}
        ${ODB_LIBRARIES} odb-boost odb-mysql)

install(TARGETS ${LEOSAC_BIN} DESTINATION bin)
install(TARGETS ${LEOSAC_LIB} DESTINATION lib)

# Build modules
if(LEOSAC_BUILD_MODULES)
    add_subdirectory(modules)
endif()
