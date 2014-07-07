cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(LEOSAC CXX)

# Avoid source tree pollution
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "In-source builds are not encouraged. Make a separate folder for building:\nmkdir build; cd build; cmake ..;cd ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif()

# Force CMAKE_BUILD_TYPE
if(CMAKE_BUILD_TYPE STREQUAL "")
    message(WARNING "Please specify a build type\nExample: cmake .. -DCMAKE_BUILD_TYPE=Debug")
endif()

# Set correct hardware platform macro
set(LEOSAC_PLATFORM CACHE STRING "Hardware platform on which to build Leosac")
if (NOT LEOSAC_PLATFORM)
    message(WARNING "No hardware platform is specified\nExample: -DLEOSAC_PLATFORM=RaspberryPi")
else()
    string(TOUPPER ${LEOSAC_PLATFORM} LEOSAC_PLATFORM_UPPERCASE)
    add_definitions(-DLEOSAC_PLATFORM_${LEOSAC_PLATFORM_UPPERCASE})
    unset(LEOSAC_PLATFORM_STRING_UPPERCASE)
    unset(LEOSAC_PLATFORM)
endif()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(CXX11 REQUIRED)
find_package(TCLAP REQUIRED)
find_package(Boost 1.41.0 REQUIRED)

list(APPEND CMAKE_CXX_FLAGS ${CXX11_FLAGS})

#Build Options
option(LEOSAC_BUILD_MODULES "build-modules" ON)
option(LEOSAC_GPROF         "gprof" OFF)

if (LEOSAC_GPROF)
    message(STATUS "Gprof flag active")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

add_definitions(-DLEOSAC_VERSION_MAJOR=0)
add_definitions(-DLEOSAC_VERSION_MINOR=1)
add_definitions(-DLEOSAC_VERSION_PATCH=0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(cfg)
