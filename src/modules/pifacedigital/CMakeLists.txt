set(PIFACEDIGITAL_BIN pifacedigital)

set(PIFACEDIGITAL_SRCS
        init.cpp
        PFDigitalModule.cpp
        PFDigitalPin.cpp
        CRUDHandler.cpp
        PFGPIO.cpp
        )

# Database support
set(OdbCMake_ODB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/PFGPIO.hpp
        )

set(LEOSAC_ODB_INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/deps/flagset
        ${CMAKE_SOURCE_DIR}/deps/json/src/
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
        SCHEMA_NAME module_pifacedigital
        PROFILE "boost/date-time" "boost/optional" "leosac-profile"
        INCLUDE_PREFIX modules/pifacedigital
        INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
        DEFAULT_POINTER "std::shared_ptr"
        PGSQL_SERVER_VERSION "9.1"
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/modules/pifacedigital
        )

add_subdirectory(lib)
add_library(${PIFACEDIGITAL_BIN} SHARED
        ${PIFACEDIGITAL_SRCS}
        ${OdbCMake_SOURCES}
        )

target_link_libraries(${PIFACEDIGITAL_BIN} module_piface_deps websock-api)

set_target_properties(${PIFACEDIGITAL_BIN} PROPERTIES
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
        )

install(TARGETS ${PIFACEDIGITAL_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
