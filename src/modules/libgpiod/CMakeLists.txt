find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGPIOD REQUIRED IMPORTED_TARGET libgpiod)

set(LIBGPIOD_BIN libgpiod)

set(LIBGPIOD_SRCS
    init.cpp
    LibgpiodGPIO.cpp
    LibgpiodModule.cpp
    LibgpiodPin.cpp
    LibgpiodConfig.cpp
    CRUDHandler.cpp
    WSHelperThread.cpp
    LibgpiodGPIOSerializer.hpp
)


# Database support
set(OdbCMake_ODB_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/LibgpiodGPIO.hpp
)

set(LEOSAC_ODB_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/deps/zmqpp/src/
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/deps/flagset
    ${CMAKE_SOURCE_DIR}/deps/date/include
    ${CMAKE_SOURCE_DIR}/deps/json/include/
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
    SCHEMA_NAME module_libgpiod
    PROFILE "boost/date-time" "boost/optional" "boost/uuid" "leosac-profile"
    INCLUDE_PREFIX modules/libgpiod
    INCLUDE ${LEOSAC_ODB_INCLUDE_DIRS}
    DEFAULT_POINTER "std::shared_ptr"
    PGSQL_SERVER_VERSION "9.1"
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/odb_gen/modules/libgpiod
)

add_library(${LIBGPIOD_BIN} SHARED
    ${LIBGPIOD_SRCS}
    ${OdbCMake_SOURCES}
)

set_target_properties(${LIBGPIOD_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
)

target_include_directories(${LIBGPIOD_BIN} PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${ODB_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/odb_gen
    ${CMAKE_SOURCE_DIR}/deps/json/include
    ${CMAKE_SOURCE_DIR}/deps/date/include
)

target_link_libraries(${LIBGPIOD_BIN} PkgConfig::LIBGPIOD websock-api)

install(TARGETS ${LIBGPIOD_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
