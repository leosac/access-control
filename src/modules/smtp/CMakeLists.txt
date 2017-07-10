set(SMTP_BIN smtp)

set(SMTP_SRCS
        init.cpp
        SMTPModule.cpp
        SMTPConfig.cpp
        SMTPServerInfoSerializer.cpp
        SMTPAudit.cpp
        SMTPAuditSerializer.cpp
        SMTPServiceImpl.cpp
        )

# Database support
set(OdbCMake_ODB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/SMTPConfig.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SMTPAudit.hpp
        )


set(LEOSAC_ODB_INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/deps/flagset
        ${ZEROMQ_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/src/db
        )

set(OdbCMake_SOURCES "")
odb_compile(OdbCMake_SOURCES
        FILES ${OdbCMake_ODB_HEADERS}
        MULTI_DATABASE dynamic
        DB pgsql sqlite
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME module_smtp
        PROFILE "boost/date-time" "boost/optional" "leosac-profile"
        INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
        INCLUDE_REGEX %tools/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %tools/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/credentials/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/credentials/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/audit/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/audit/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/update/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/update/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        )

add_library(${SMTP_BIN} SHARED
        ${SMTP_SRCS}
        ${OdbCMake_SOURCES}
        )

set_target_properties(${SMTP_BIN} PROPERTIES
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
        )

target_include_directories(${SMTP_BIN} PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${ODB_INCLUDE_DIRS}
        ${ODB_COMPILE_OUTPUT_DIR}
        )

target_link_libraries(${SMTP_BIN} curl websock-api)

install(TARGETS ${SMTP_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
