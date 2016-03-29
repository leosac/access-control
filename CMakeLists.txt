cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
project(LEOSAC)

# Avoid source tree pollution
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "In-source builds are not encouraged. Make a separate folder for building:\nmkdir build; cd build; cmake ..;cd ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif()

# Force CMAKE_BUILD_TYPE
if(CMAKE_BUILD_TYPE STREQUAL "")
    message(WARNING "Please specify a build type\nExample: cmake .. -DCMAKE_BUILD_TYPE=Debug")
endif()

set(LEOSAC_BUILD_CMAKE_ZMQ true CACHE BOOL "Build Libzmq through its CMakeFile")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(CXX11 REQUIRED)
find_package(TCLAP REQUIRED)
find_package(Boost 1.55.0 REQUIRED system serialization regex filesystem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O0")

#Build Options
option(LEOSAC_BUILD_MODULES "build-modules" ON)
option(LEOSAC_BUILD_TESTS   "build-tests" OFF)
option(LEOSAC_GPROF         "gprof" OFF)

if(LEOSAC_GPROF)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

add_definitions(-DLEOSAC_VERSION_MAJOR=0)
add_definitions(-DLEOSAC_VERSION_MINOR=6)
add_definitions(-DLEOSAC_VERSION_PATCH=3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LEOSAC_BINARY_DIR})
set(CMAKE_INSTALL_RPATH \$ORIGIN/../lib)

if (LEOSAC_BUILD_CMAKE_ZMQ)
add_subdirectory(libzmq)
endif()
add_subdirectory(zmqpp)
add_subdirectory(src)
add_subdirectory(cfg)

if(LEOSAC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

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
