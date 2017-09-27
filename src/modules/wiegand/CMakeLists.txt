set(WIEGAND_BIN wiegand)

set(WIEGAND_SRCS
    wiegand.cpp
    WiegandReaderImpl.cpp
    WiegandConfig.cpp
    WiegandConfigManager.cpp
    strategies/SimpleWiegandStrategy.cpp
    strategies/WiegandPinNBitsOnly.cpp
    strategies/WiegandPinBuffered.cpp
    strategies/WiegandCardAndPin.cpp
    strategies/Autodetect.cpp
)

# Database support
set(OdbCMake_ODB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/WiegandConfig.hpp
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
        SCHEMA_NAME module_wiegand
        PROFILE "boost/date-time" "boost/optional" "leosac-profile"
        INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
        HEADER_PROLOGUE ${CMAKE_SOURCE_DIR}/src/tools/db/ODB_Auth_Prologue.hpp
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/modules/wiegand
        )


add_library(${WIEGAND_BIN} SHARED ${WIEGAND_SRCS} ${OdbCMake_SOURCES})

set_target_properties(${WIEGAND_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

target_include_directories(${WIEGAND_BIN} PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/deps/date/include
        ${ODB_INCLUDE_DIRS}
        ${ODB_COMPILE_OUTPUT_DIR}
        )

install(TARGETS ${WIEGAND_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
