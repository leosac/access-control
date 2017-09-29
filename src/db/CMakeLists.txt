PROJECT(DATABASELIB CXX)

## Build scripts for database support code.
## We run multiple compilation with close but slightly different
## compilation flag (most notably the INCLUDE_PREFIX variable).
##
## All those compilation are part of the same ODB schema: "core".
## This schema hold database structure information for the leosac
## core system. Modules can create additional schema.

# ODB configuration for tools/db (LogEntry).
set(OdbCMake_ODB_HEADERS_TOOLS
        ${CMAKE_SOURCE_DIR}/src/tools/LogEntry.hpp
        ${CMAKE_SOURCE_DIR}/src/tools/SingleTimeFrame.hpp
        ${CMAKE_SOURCE_DIR}/src/tools/Schedule.hpp
        ${CMAKE_SOURCE_DIR}/src/tools/ScheduleMapping.hpp
        )

set(LEOSAC_ODB_INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/deps/flagset
        ${CMAKE_SOURCE_DIR}/deps/date/include
        ${ZEROMQ_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/src/db
        )

set(OdbCMake_SOURCES_TOOLS "")
odb_compile(OdbCMake_SOURCES_TOOLS
        FILES ${OdbCMake_ODB_HEADERS_TOOLS}
        MULTI_DATABASE dynamic
        DB pgsql sqlite 
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION
        SCHEMA_NAME core
        PROFILE "boost/date-time" "boost/optional" "boost/uuid" leosac-profile
        INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
        INCLUDE_PREFIX tools
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        ODB_EPILOGUE "#include \"core/audit/AuditEntry.hpp\""
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/tools/
        )


# ODB configuration for auth stuff (User/Group)
set(OdbCMake_ODB_HEADERS_AUTH
        ${CMAKE_SOURCE_DIR}/src/core/auth/User.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/Group.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/Token.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/UserGroupMembership.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/ValidityInfo.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/Door.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/AccessPoint.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/AccessPointUpdate.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/Zone.hpp
        )

set(OdbCMake_SOURCES_AUTH "")
odb_compile(OdbCMake_SOURCES_AUTH
        FILES ${OdbCMake_ODB_HEADERS_AUTH}
        MULTI_DATABASE dynamic
        DB pgsql sqlite
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION
        SCHEMA_NAME core
        PROFILE "boost/date-time" "boost/optional" "boost/uuid" leosac-profile
        HEADER_PROLOGUE ${CMAKE_SOURCE_DIR}/src/tools/db/ODB_Auth_Prologue.hpp
        INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
        INCLUDE_PREFIX core/auth/
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        ODB_EPILOGUE "#include \"core/audit/AuditEntry.hpp\""
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/core/auth
        )

# ODB configuration for Audit log
set(OdbCMake_ODB_HEADERS_AUDITLOG
        ${CMAKE_SOURCE_DIR}/src/core/audit/AuditEntry.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/AuditTracker.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/WSAPICall.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/UserEvent.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/GroupEvent.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/UserGroupMembershipEvent.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/CredentialEvent.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/ScheduleEvent.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/DoorEvent.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/AccessPointEvent.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/UpdateEvent.hpp
        ${CMAKE_SOURCE_DIR}/src/core/audit/ZoneEvent.hpp
        )

set(OdbCMake_SOURCES_AUDITLOG "")
odb_compile(OdbCMake_SOURCES_AUDITLOG
        FILES ${OdbCMake_ODB_HEADERS_AUDITLOG}
        MULTI_DATABASE dynamic
        DB pgsql sqlite
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION
        SCHEMA_NAME core
        PROFILE "boost/date-time" "boost/optional" "boost/uuid" leosac-profile
        HXX_PROLOGUE "#include \"tools/db/AuditEventMaskODB.hpp\""
        INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
        INCLUDE_PREFIX core/audit/
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        ODB_EPILOGUE "#include \"core/audit/AuditEntry.hpp\""
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/core/audit
        )

# ODB configuration for Credentials
set(OdbCMake_ODB_HEADERS_CREDENTIALS
        ${CMAKE_SOURCE_DIR}/src/core/credentials/Credential.hpp
        ${CMAKE_SOURCE_DIR}/src/core/credentials/RFIDCard.hpp
        ${CMAKE_SOURCE_DIR}/src/core/credentials/PinCode.hpp
        )

set(OdbCMake_SOURCES_CREDENTIALS "")
odb_compile(OdbCMake_SOURCES_CREDENTIALS
        FILES ${OdbCMake_ODB_HEADERS_CREDENTIALS}
        MULTI_DATABASE dynamic
        DB pgsql sqlite
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION
        SCHEMA_NAME core
        PROFILE "boost/date-time" "boost/optional" "boost/uuid" leosac-profile
        INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
        INCLUDE_PREFIX core/credentials/
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        ODB_EPILOGUE "#include \"core/audit/AuditEntry.hpp\""
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/core/credentials
        )


# ODB configuration for Updates
set(OdbCMake_ODB_HEADERS_UPDATES
        ${CMAKE_SOURCE_DIR}/src/core/update/Update.hpp
        )

set(OdbCMake_SOURCES_UPDATES "")
odb_compile(OdbCMake_SOURCES_UPDATES
        FILES ${OdbCMake_ODB_HEADERS_UPDATES}
        MULTI_DATABASE dynamic
        DB pgsql sqlite
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION
        SCHEMA_NAME core
        PROFILE "boost/date-time" "boost/optional" leosac-profile
        INCLUDE  ${LEOSAC_ODB_INCLUDE_DIRS}
        INCLUDE_PREFIX core/update/
        HEADER_PROLOGUE ${CMAKE_SOURCE_DIR}/src/tools/db/ODB_Auth_Prologue.hpp
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        ODB_EPILOGUE "#include \"core/audit/AuditEntry.hpp\""
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/core/update
        )

# ODB configuration for Hardware devices
set(OdbCMake_ODB_HEADERS_HARDWARE
        ${CMAKE_SOURCE_DIR}/src/hardware/Device.hpp
        ${CMAKE_SOURCE_DIR}/src/hardware/GPIO.hpp
        )

set(OdbCMake_SOURCES_HARDWARE "")
odb_compile(OdbCMake_SOURCES_HARDWARE
        FILES ${OdbCMake_ODB_HEADERS_HARDWARE}
        MULTI_DATABASE dynamic
        DB pgsql sqlite
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION
        SCHEMA_NAME core
        PROFILE "boost/date-time" "boost/optional" "boost/uuid" leosac-profile
        INCLUDE  ${LEOSAC_ODB_INCLUDE_DIRS}
        INCLUDE_PREFIX hardware/
        HEADER_PROLOGUE ${CMAKE_SOURCE_DIR}/src/tools/db/ODB_Auth_Prologue.hpp
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/hardware
        )

# leosac_db library that contains database support code
add_library(leosac_db SHARED
        ${OdbCMake_SOURCES_TOOLS} ${OdbCMake_ODB_HEADERS_TOOLS}
        ${OdbCMake_SOURCES_AUTH} ${OdbCMake_ODB_HEADERS_AUTH}
        ${OdbCMake_SOURCES_AUDITLOG} ${OdbCMake_ODB_HEADERS_AUDITLOG}
        ${OdbCMake_SOURCES_CREDENTIALS} ${OdbCMake_ODB_HEADERS_CREDENTIALS}
        ${OdbCMake_SOURCES_UPDATES} ${OdbCMake_ODB_HEADERS_UPDATES}
        ${OdbCMake_SOURCES_HARDWARE} ${OdbCMake_ODB_HEADERS_HARDWARE}
        ${CMAKE_BINARY_DIR}/src/db/core-schema-sqlite.cxx
        ${CMAKE_BINARY_DIR}/src/db/core-schema-pgsql.cxx
        )

target_include_directories(leosac_db PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/deps/zmqpp/src
        ${CMAKE_SOURCE_DIR}/deps/flagset
        ${CMAKE_SOURCE_DIR}/deps/date/include
        ${CMAKE_BINARY_DIR}/odb_gen
        ${ZEROMQ_INCLUDE_DIR}
        ${ODB_INCLUDE_DIRS}
        )

if (LEOSAC_CROSS_COMPILE)
    set(__LEOSAC_SQL_GENERATION_ADDITIONAL_INCLUDES -I ${CMAKE_SYSROOT}/include)
endif()

set(LEOSAC_DATABASE_CORE_HEADERS
        ${OdbCMake_ODB_HEADERS_TOOLS}
        ${OdbCMake_ODB_HEADERS_AUTH}
        ${OdbCMake_ODB_HEADERS_AUDITLOG}
        ${OdbCMake_ODB_HEADERS_CREDENTIALS}
        ${OdbCMake_ODB_HEADERS_UPDATES}
        ${OdbCMake_ODB_HEADERS_HARDWARE}
        ${CMAKE_SOURCE_DIR}/src/tools/db/database.hpp
        )

## This generate a leosac.sql file
## for creating the database structure.
##
## Also generate changelog and database schema as separate C++ source file.
add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/src/db/core-schema-sqlite.cxx ${CMAKE_BINARY_DIR}/src/db/core-schema-pgsql.cxx
        DEPENDS ${LEOSAC_DATABASE_CORE_HEADERS}
        COMMAND odb
        --multi-database dynamic
        --profile boost/date-time
        --profile boost/optional
        --profile boost/uuid
        --profile leosac-profile
        --database pgsql
        --database sqlite
        --std c++14
        -I ${CMAKE_SOURCE_DIR}/deps/flagset
        -I ${CMAKE_SOURCE_DIR}/deps/date/include
        -I ${CMAKE_SOURCE_DIR}/deps/zmqpp/src
        -I ${CMAKE_SOURCE_DIR}/src
        -I ${CMAKE_SOURCE_DIR}/src/db
        ${__LEOSAC_SQL_GENERATION_ADDITIONAL_INCLUDES}
        --schema-format separate
        --schema-format sql
        --generate-schema-only
        --at-once
        --input-name core
        --generate-query
        --schema-name core
        ${LEOSAC_DATABASE_CORE_HEADERS}
        )

install(TARGETS leosac_db DESTINATION lib)
