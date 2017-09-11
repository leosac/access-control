set(NOTIFD_BIN notifd)

set(NOTIFD_SRCS
        init.cpp
        NotifdModule.cpp
        )

# Database support
set(OdbCMake_ODB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/Notification.hpp
        )

set(OdbCMake_SOURCES "")
odb_compile(OdbCMake_SOURCES
        FILES ${OdbCMake_ODB_HEADERS}
        MULTI_DATABASE dynamic
        DB pgsql
        STANDARD "c++14"
        GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA
        SCHEMA_FORMAT embedded
        SCHEMA_NAME module_NOTIFD
        PROFILE "boost/date-time"
        INCLUDE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/deps/flagset
        INCLUDE_REGEX %tools/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %tools/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/auth/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/auth/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/credentials/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/credentials/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        %core/audit/\(.+\)_odb_inline\(.*\).h%$1_odb_inline$2.h%
        %core/audit/\(.+\)_odb\(.*\).h%$1_odb$2.h%
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/modules/notifd
        )

add_library(${NOTIFD_BIN} SHARED
        ${NOTIFD_SRCS}
        ${OdbCMake_SOURCES}
        )

set_target_properties(${NOTIFD_BIN} PROPERTIES
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
        )

target_include_directories(${NOTIFD_BIN} PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${ODB_INCLUDE_DIRS}
        ${ODB_COMPILE_OUTPUT_DIR}
        )

target_link_libraries(${NOTIFD_BIN})

install(TARGETS ${NOTIFD_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
