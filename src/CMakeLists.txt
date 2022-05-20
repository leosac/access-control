list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

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
    core/auth/SimpleAccessProfile.cpp
    core/auth/BaseAuthSource.cpp
    core/auth/AuthTarget.cpp
    core/auth/User.cpp
    core/auth/serializers/UserSerializer.cpp
    core/auth/serializers/GroupSerializer.cpp
    core/auth/serializers/UserGroupMembershipSerializer.cpp
    core/auth/serializers/DoorSerializer.cpp
    core/auth/serializers/AccessPointSerializer.cpp
    core/auth/serializers/ZoneSerializer.cpp
    core/auth/Token.cpp
    core/auth/UserGroupMembership.cpp
    core/auth/AuthSourceBuilder.cpp
    core/auth/ValidityInfo.cpp
    core/auth/Door.cpp
    core/auth/AccessPoint.cpp
    core/auth/AccessPointUpdate.cpp
    core/auth/AccessPointService.cpp
    core/auth/Zone.cpp
    core/credentials/Credential.cpp
    core/credentials/CredentialValidator.cpp
    core/credentials/RFIDCard.cpp
    core/credentials/PinCode.cpp
    core/credentials/RFIDCardPin.cpp
    core/credentials/serializers/CredentialSerializer.cpp
    core/credentials/serializers/RFIDCardSerializer.cpp
    core/credentials/serializers/PolymorphicCredentialSerializer.cpp
    core/credentials/serializers/PinCodeSerializer.cpp
    core/UserSecurityContext.cpp
    core/GetServiceRegistry.cpp
    dynlib/dynamiclibrary.cpp
    modules/AsioModule.cpp
    modules/BaseModule.cpp
    exception/ExceptionsTools.cpp
    exception/ModelException.cpp
    exception/EntityNotFound.cpp
    exception/PermissionDenied.cpp
    hardware/facades/FGPIO.cpp
    hardware/facades/FLED.cpp
    hardware/facades/FWiegandReader.cpp
    hardware/facades/FExternalServer.cpp
    hardware/Device.cpp
    hardware/GPIO.cpp
    hardware/RFIDReader.cpp
    hardware/Buzzer.cpp
    hardware/LED.cpp
    hardware/ExternalMessage.cpp
    hardware/ExternalServer.cpp
    hardware/HardwareService.cpp
    hardware/serializers/RFIDReaderSerializer.cpp
    hardware/serializers/GPIOSerializer.cpp
    hardware/serializers/DeviceSerializer.cpp
    hardware/serializers/LEDSerializer.cpp
    hardware/serializers/BuzzerSerializer.cpp
    hardware/serializers/ExternalMessageSerializer.cpp
    hardware/serializers/ExternalServerSerializer.cpp
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
    tools/ThreadUtils.cpp
    tools/GenGuid.cpp
    tools/PropertyTreeExtractor.cpp
    tools/log.cpp
    tools/DatabaseLogSink.cpp
    tools/ElapsedTimeCounter.cpp
    tools/XmlNodeNameEnforcer.cpp
    tools/Stacktrace.cpp
    tools/LogEntry.cpp
    tools/db/DBService.cpp
    tools/db/MultiplexedSession.cpp
    tools/db/MultiplexedTransaction.cpp
    tools/db/OptionalTransaction.cpp
    tools/db/Savepoint.cpp
    tools/scrypt/Random.cpp
    tools/scrypt/Scrypt.cpp
    tools/registry/ThreadLocalRegistry.cpp
    tools/registry/GlobalRegistry.cpp
    tools/JSONUtils.cpp
    tools/MyTime.cpp
    tools/SingleTimeFrame.cpp
    tools/serializers/ScheduleSerializer.cpp
    tools/serializers/ScheduleMappingSerializer.cpp
    tools/ScheduleMapping.cpp
    core/tasks/GetLocalConfigVersion.cpp
    core/tasks/GetRemoteConfigVersion.cpp
    core/tasks/FetchRemoteConfig.cpp
    core/tasks/SyncConfig.cpp
    core/tasks/RemoteControlAsyncResponse.cpp
    core/audit/AuditEntry.cpp
    core/audit/UserEvent.cpp
    core/audit/WSAPICall.cpp
    core/audit/AuditFactory.cpp
    core/audit/GroupEvent.cpp
    core/audit/UserGroupMembershipEvent.cpp
    core/audit/CredentialEvent.cpp
    core/audit/ScheduleEvent.cpp
    core/audit/DoorEvent.cpp
    core/audit/AuthEvent.cpp
    core/audit/UpdateEvent.cpp
    core/audit/AccessPointEvent.cpp
    core/audit/AuditTracker.cpp
    core/audit/ZoneEvent.cpp
    core/audit/serializers/AuditSerializer.cpp
    core/audit/serializers/UserEventSerializer.cpp
    core/audit/serializers/PolymorphicAuditSerializer.cpp
    core/audit/serializers/WSAPICallSerializer.cpp
    core/audit/serializers/CredentialEventSerializer.cpp
    core/audit/serializers/GroupEventSerializer.cpp
    core/audit/serializers/ScheduleEventSerializer.cpp
    core/audit/serializers/DoorEventSerializer.cpp
    core/audit/serializers/AuthEventSerializer.cpp
    core/audit/serializers/UserGroupMembershipEventSerializer.cpp
    core/audit/serializers/AccessPointEventSerializer.cpp
    core/audit/serializers/ZoneEventSerializer.cpp
    core/alarms/Alarm.cpp
    core/alarms/AlarmEntry.cpp
    core/alarms/serializers/AlarmSerializer.cpp
    core/update/UpdateService.cpp
    core/update/Update.cpp
    core/update/serializers/AccessPointUpdateSerializer.cpp
    core/update/serializers/UpdateSerializer.cpp
    core/update/serializers/UpdateDescriptorSerializer.cpp
    tools/db/PGSQLTracer.cpp
    tools/Visitor.cpp
    core/SecurityContext.cpp
    core/audit/serializers/UpdateEventSerializer.cpp
    hardware/serializers/BuzzerSerializer.cpp
    hardware/serializers/BuzzerSerializer.hpp)

# Include mode to extract Git Revision.
include(GetGitRevisionDescription)

# Store revision in GIT_SHA1 var.
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

# Automatically generates a file to store the GIT SHA1 version.
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tools/GitSHA1.cpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp" @ONLY)
list(APPEND LEOSAC_LIB_SRCS "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp")

# Rule for ODB code generation.
# Build a shared library: leosac_db
add_subdirectory(db)

add_executable(${LEOSAC_BIN} ${LEOSAC_SRCS})

add_library(${LEOSAC_LIB} SHARED
        ${LEOSAC_LIB_SRCS}
        ${OdbCMake_SOURCES_TOOLS}
        ${OdbCMake_SOURCES_AUTH}
        )

target_include_directories(${LEOSAC_LIB}
        PUBLIC
        ${ODB_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/odb_gen/
        ${CMAKE_SOURCE_DIR}/deps/flagset
        ${CMAKE_SOURCE_DIR}/deps/date/include
        )

target_compile_definitions(${LEOSAC_LIB}
        PRIVATE
        -DSPDLOG_ENABLE_SYSLOG=1
        )

target_compile_definitions(${LEOSAC_BIN}
        PUBLIC
        -DUNIX=1 #LLA flags'
        )

set(LEOSAC_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wno-deprecated-declarations")

#Add GProf compile flag
if(LEOSAC_GPROF)
    set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -pg")
endif()

# Add debug flags
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -Wall -O0 -Wextra -g3")
    endif()
endif()

## Add optimization flags
if (CMAKE_BUILD_TYPE MATCHES "Release")
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        set(LEOSAC_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef -O2")
    endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/deps/libpifacedigital/src/
${CMAKE_SOURCE_DIR}/deps/libmcp23s17/src/
${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
${CMAKE_SOURCE_DIR}/deps/spdlog/include
${CMAKE_SOURCE_DIR}/deps/flagset
${CMAKE_SOURCE_DIR}/deps/json/include
${Boost_INCLUDE_DIRS}
)

#Set compilation flags for current target
set_target_properties(${LEOSAC_BIN} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

set_target_properties(${LEOSAC_LIB} PROPERTIES
    COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")

target_link_libraries(${LEOSAC_BIN} ${LEOSAC_LIB} backtrace)
target_link_libraries(${LEOSAC_LIB} dl pthread zmqpp ${Boost_LIBRARIES}
        ${ODB_LIBRARIES} backtrace scrypt
        leosac_db
        )

# Somehow, cross compile requires us to link against libcrypto
# and librt
if (LEOSAC_CROSS_COMPILE)
    target_link_libraries(${LEOSAC_LIB} crypto rt)

    # We disable per-exception stacktrace generation when cross compiling
    # because that is a debug feature that takes a lot of ram and can't
    # be properly freed.
    target_compile_definitions(${LEOSAC_LIB} PUBLIC -DLEOSAC_STACKTRACE_DISABLED=1)
endif()

if (${LEOSAC_STACKTRACE_DISABLED})
    # Disable stacktrace generation.
    target_compile_definitions(${LEOSAC_LIB} PUBLIC -DLEOSAC_STACKTRACE_DISABLED=1)
endif()

install(TARGETS ${LEOSAC_BIN} DESTINATION bin)
install(TARGETS ${LEOSAC_LIB} DESTINATION lib)

# Build modules
if(LEOSAC_BUILD_MODULES)
    add_subdirectory(modules)
    add_subdirectory(altmodules)
endif()
