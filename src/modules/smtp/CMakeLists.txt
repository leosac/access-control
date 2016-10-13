set(SMTP_BIN smtp)

set(SMTP_SRCS
        init.cpp
        SMTP.cpp
        SMTPConfig.cpp
        )

# Database support
# ODB configuration for tools/db (LogEntry).
set(OdbCMake_ODB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/SMTPConfig.hpp
        )

set(OdbCMake_SOURCES "")
odb_compile(OdbCMake_SOURCES
        FILES ${OdbCMake_ODB_HEADERS}
        MULTI_DATABASE dynamic
        DB pgsql
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME module_smtp
        INCLUDE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/flagset
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

target_link_libraries(${SMTP_BIN} curl)

install(TARGETS ${SMTP_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
