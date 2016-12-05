PROJECT(DATABASELIB CXX)

# ODB configuration for tools/db (LogEntry).
set(OdbCMake_ODB_HEADERS_TOOLS
        ${CMAKE_SOURCE_DIR}/src/tools/LogEntry.hpp
        ${CMAKE_SOURCE_DIR}/src/tools/SingleTimeFrame.hpp
        ${CMAKE_SOURCE_DIR}/src/tools/Schedule.hpp
        ${CMAKE_SOURCE_DIR}/src/tools/ScheduleMapping.hpp
        )

set(OdbCMake_SOURCES_TOOLS "")
odb_compile(OdbCMake_SOURCES_TOOLS
        FILES ${OdbCMake_ODB_HEADERS_TOOLS}
        MULTI_DATABASE dynamic
        DB pgsql
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME tools
        PROFILE "boost/date-time"
        INCLUDE ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/ ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/flagset ${ZEROMQ_INCLUDE_DIR}
        INCLUDE_PREFIX tools
        INCLUDE_REGEX %tools/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %tools/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/audit/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/audit/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/update/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/update/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/credentials/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/credentials/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
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
        )

set(OdbCMake_SOURCES_AUTH "")
odb_compile(OdbCMake_SOURCES_AUTH
        FILES ${OdbCMake_ODB_HEADERS_AUTH}
        MULTI_DATABASE dynamic
        DB pgsql
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME auth
        PROFILE "boost/date-time"
        HEADER_PROLOGUE ${CMAKE_SOURCE_DIR}/src/tools/db/ODB_Auth_Prologue.hpp
        INCLUDE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/ ${CMAKE_SOURCE_DIR}/deps/flagset ${ZEROMQ_INCLUDE_DIR}
        INCLUDE_PREFIX core/auth/
        INCLUDE_REGEX %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/credentials/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/credentials/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/update/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/update/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/audit/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/audit/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %tools/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %tools/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
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
        )

set(OdbCMake_SOURCES_AUDITLOG "")
odb_compile(OdbCMake_SOURCES_AUDITLOG
        FILES ${OdbCMake_ODB_HEADERS_AUDITLOG}
        MULTI_DATABASE dynamic
        DB pgsql
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME audit
        PROFILE "boost/date-time"
        HXX_PROLOGUE "#include \"tools/db/AuditEventMaskODB.hpp\""
        INCLUDE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/ ${CMAKE_SOURCE_DIR}/deps/flagset ${CMAKE_SOURCE_DIR}/deps/flagset ${ZEROMQ_INCLUDE_DIR}
        INCLUDE_PREFIX core/audit/
        INCLUDE_REGEX %core/audit/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/credentials/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/credentials/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/audit/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %tools/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %tools/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        )

# ODB configuration for Credentials
set(OdbCMake_ODB_HEADERS_CREDENTIALS
        ${CMAKE_SOURCE_DIR}/src/core/credentials/Credential.hpp
        ${CMAKE_SOURCE_DIR}/src/core/credentials/WiegandCard.hpp
        ${CMAKE_SOURCE_DIR}/src/core/credentials/PinCode.hpp
        )

set(OdbCMake_SOURCES_CREDENTIALS "")
odb_compile(OdbCMake_SOURCES_CREDENTIALS
        FILES ${OdbCMake_ODB_HEADERS_CREDENTIALS}
        MULTI_DATABASE dynamic
        DB pgsql
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME credentials
        PROFILE "boost/date-time"
        INCLUDE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/ ${CMAKE_SOURCE_DIR}/deps/flagset ${ZEROMQ_INCLUDE_DIR}
        INCLUDE_PREFIX core/credentials/
        INCLUDE_REGEX %core/audit/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/audit/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/credentials/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/credentials/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %tools/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %tools/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        )


# ODB configuration for Updates
set(OdbCMake_ODB_HEADERS_UPDATES
        ${CMAKE_SOURCE_DIR}/src/core/update/Update.hpp
        )

set(OdbCMake_SOURCES_UPDATES "")
odb_compile(OdbCMake_SOURCES_UPDATES
        FILES ${OdbCMake_ODB_HEADERS_UPDATES}
        MULTI_DATABASE dynamic
        DB pgsql
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME update
        PROFILE "boost/date-time"
        INCLUDE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/ ${CMAKE_SOURCE_DIR}/deps/flagset ${ZEROMQ_INCLUDE_DIR}
        INCLUDE_PREFIX core/update/
        HEADER_PROLOGUE ${CMAKE_SOURCE_DIR}/src/tools/db/ODB_Auth_Prologue.hpp
        INCLUDE_REGEX
        %core/update/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/update/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/audit/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/audit/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/credentials/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/credentials/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %tools/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %tools/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        )

add_library(leosac_db SHARED
        ${OdbCMake_SOURCES_TOOLS} ${OdbCMake_ODB_HEADERS_TOOLS}
        ${OdbCMake_SOURCES_AUTH} ${OdbCMake_ODB_HEADERS_AUTH}
        ${OdbCMake_SOURCES_AUDITLOG} ${OdbCMake_ODB_HEADERS_AUDITLOG}
        ${OdbCMake_SOURCES_CREDENTIALS} ${OdbCMake_ODB_HEADERS_CREDENTIALS}
        ${OdbCMake_SOURCES_UPDATES} ${OdbCMake_ODB_HEADERS_UPDATES}
        )

target_include_directories(leosac_db PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/deps/zmqpp/src
        ${CMAKE_SOURCE_DIR}/deps/flagset
        ${ZEROMQ_INCLUDE_DIR}
        )


## This generate a leosac.sql file
## for creating the database structure.
add_custom_command(TARGET leosac_db
        POST_BUILD
        COMMAND odb --profile boost --std c++14 --database pgsql
        -I ${CMAKE_SOURCE_DIR}/deps/flagset
        -I ${CMAKE_SOURCE_DIR}/deps/zmqpp/src
        -I ${CMAKE_SOURCE_DIR}/src
        --generate-schema-only --at-once
        --input-name leosac
        --generate-query
        ${OdbCMake_ODB_HEADERS_AUDITLOG}
        ${OdbCMake_ODB_HEADERS_AUTH}
        ${OdbCMake_ODB_HEADERS_TOOLS}
        ${OdbCMake_ODB_HEADERS_CREDENTIALS}
        ${OdbCMake_ODB_HEADERS_UPDATES}
        )

install(TARGETS leosac_db DESTINATION lib)
