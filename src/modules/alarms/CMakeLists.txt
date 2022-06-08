set(ALARMS_BIN alarms)

set(ALARMS_SRCS
        init.cpp
        AlarmsModule.cpp
        AlarmInstance.cpp
        AlarmEntry.cpp
        )

# Database support
set(OdbCMake_ODB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/AlarmEntry.hpp
        )

set(LEOSAC_ODB_INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/deps/flagset
        ${CMAKE_SOURCE_DIR}/deps/date/include
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
        SCHEMA_NAME module_alarms
        PROFILE "boost/date-time" "boost/optional" "boost/uuid" "leosac-profile"
        INCLUDE_PREFIX modules/alarms
        INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        ODB_EPILOGUE "#include \"core/audit/AuditEntry.hpp\""
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/modules/alarms
        )

add_library(${ALARMS_BIN} SHARED
        ${ALARMS_SRCS}
        ${OdbCMake_SOURCES}
        )

set_target_properties(${ALARMS_BIN} PROPERTIES
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
        )

target_include_directories(${ALARMS_BIN} PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${ODB_INCLUDE_DIRS}
        ${ODB_COMPILE_OUTPUT_DIR}
        )

target_link_libraries(${ALARMS_BIN} websock-api)

install(TARGETS ${ALARMS_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
