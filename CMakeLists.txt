cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
project(LEOSAC)

# Version definitions
add_definitions(-DLEOSAC_VERSION_MAJOR=0)
add_definitions(-DLEOSAC_VERSION_MINOR=8)
add_definitions(-DLEOSAC_VERSION_PATCH=0)

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "8")
set(CPACK_PACKAGE_VERSION_PATCH "0")

include(CPack)

set(CMAKE_INSTALL_RPATH "\$ORIGN:\$ORIGIN/../lib/leosac")
set(LEOSAC_CROSS_COMPILE 0)
if (CMAKE_TOOLCHAIN_FILE)
    # a bit hacky -- if a toolchain file si present, assume
    # cross compilation to raspberry pi
    set(LEOSAC_CROSS_COMPILE 1)
    message(WARNING "CROSS COMPILE detected.")

    # We configure RPATH handling here, assuming that the fakeroot on
    # the raspberry will be located at /opt/rpi_fakeroot
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

    # Set RPATH to "binary directory" (for dev mode) and "/opt/rpi_faekroot"
    # for "installed" mode
    set(CMAKE_INSTALL_RPATH "/opt/rpi_fakeroot/lib:${CMAKE_INSTALL_RPATH}")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()

# Avoid source tree pollution
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "In-source builds are not encouraged. Make a separate folder for building:\nmkdir build; cd build; cmake ..;cd ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif ()

# Force CMAKE_BUILD_TYPE
if (CMAKE_BUILD_TYPE STREQUAL "")
    message(WARNING "Please specify a build type\nExample: cmake .. -DCMAKE_BUILD_TYPE=Debug")
endif ()

set(LEOSAC_BUILD_CMAKE_ZMQ true CACHE BOOL "Build Libzmq through its CMakeFile")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(CXX11 REQUIRED)
find_package(TCLAP REQUIRED)
find_package(Boost 1.62 REQUIRED date_time system serialization regex filesystem)
find_package(LibScrypt REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SQLite REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${TCLAP_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${LIBSCRYPT_INCLUDE_DIR}
                    ${SQLITE_INCLUDE_DIR} ${CURL_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

# ODB stuff
find_package(ODB REQUIRED COMPONENTS pgsql sqlite boost)

include(${ODB_USE_FILE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-undef -Wno-shadow -Wno-unknown-pragmas")

#Build Options
option(LEOSAC_BUILD_MODULES "build-modules" ON)
option(LEOSAC_BUILD_TESTS "build-tests" OFF)
option(LEOSAC_GPROF "gprof" OFF)

if (LEOSAC_GPROF)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})

add_subdirectory(deps/zmqpp)
add_subdirectory(src)
add_subdirectory(cfg)

if (LEOSAC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/build_ipconfig.sh DESTINATION ${LEOSAC_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/load_ipconfig.sh DESTINATION ${LEOSAC_BINARY_DIR})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/build_ipconfig.sh
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
        DESTINATION scripts
        )

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/load_ipconfig.sh
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
        DESTINATION scripts
        )
