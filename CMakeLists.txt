cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(OSAC)

# Avoid source tree pollution
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "In-source builds are not encouraged. Make a separate folder for building:\nmkdir build; cd build; cmake ..;cd ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(CXX11 REQUIRED)

#Build Options
option(NO_HW "NoHardware" OFF)

#Define the corresponding macro
if(NO_HW)
    add_definitions(-DNO_HW)
endif()

add_definitions(-DOSAC_VERSION_MAJOR=0)
add_definitions(-DOSAC_VERSION_MINOR=1)
add_definitions(-DOSAC_VERSION_PATCH=0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OSAC_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OSAC_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OSAC_BINARY_DIR})

add_subdirectory(src)
