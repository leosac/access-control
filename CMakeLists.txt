cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(OSAC)

# Avoid source tree pollution
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "In-source builds are not encouraged. Make a separate folder for building:\nmkdir build; cd build; cmake ..;cd ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif()

# Force CMAKE_BUILD_TYPE
if(CMAKE_BUILD_TYPE STREQUAL "")
    message(WARNING "Please specify a build type\nExample: cmake .. -DCMAKE_BUILD_TYPE=Debug")
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
option(OSAC_NO_HW        "no-hardware" OFF)
option(OSAC_NO_MODULES   "no-modules" OFF)

#Define the corresponding macro
if(OSAC_NO_HW)
    add_definitions(-DOSAC_NO_HW)
endif()

add_definitions(-DOSAC_VERSION_MAJOR=0)
add_definitions(-DOSAC_VERSION_MINOR=1)
add_definitions(-DOSAC_VERSION_PATCH=2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OSAC_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OSAC_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OSAC_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(cfg)
