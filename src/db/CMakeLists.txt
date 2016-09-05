PROJECT(DATABASELIB CXX)

# ODB configuration for tools/db (LogEntry).
set(OdbCMake_ODB_HEADERS_TOOLS
        ${CMAKE_SOURCE_DIR}/src/tools/db/LogEntry.hpp
        )

set(OdbCMake_SOURCES_TOOLS "")
odb_compile(OdbCMake_SOURCES_TOOLS
        FILES ${OdbCMake_ODB_HEADERS_TOOLS}
        MULTI_DATABASE dynamic
        DB mysql sqlite
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME tools
        PROFILE "boost/date-time"
        INCLUDE ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/ ${CMAKE_SOURCE_DIR}/src
        INCLUDE_PREFIX tools/db
        INCLUDE_REGEX %tools/db/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %tools/db/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        )


# ODB configuration for auth stuff (User/Group)
set(OdbCMake_ODB_HEADERS_AUTH
        ${CMAKE_SOURCE_DIR}/src/core/auth/User.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/Group.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/Token.hpp
        ${CMAKE_SOURCE_DIR}/src/core/auth/UserGroupMembership.hpp
        )

set(OdbCMake_SOURCES_AUTH "")
odb_compile(OdbCMake_SOURCES_AUTH
        FILES ${OdbCMake_ODB_HEADERS_AUTH}
        MULTI_DATABASE dynamic
        DB mysql sqlite
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME auth
        PROFILE "boost/date-time"
        INCLUDE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
        INCLUDE_PREFIX core/auth/
        INCLUDE_REGEX %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        )


add_library(leosac_db SHARED
        ${OdbCMake_SOURCES_TOOLS} ${OdbCMake_ODB_HEADERS_TOOLS}
        ${OdbCMake_SOURCES_AUTH} ${OdbCMake_ODB_HEADERS_AUTH}
        )

target_include_directories(leosac_db PRIVATE ${CMAKE_SOURCE_DIR}/src)