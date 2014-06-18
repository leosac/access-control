
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CONFIG_FILES
    default.xml
    hardware.xml
    doorA.conf
    doorB.conf
)

file(COPY ${CONFIG_FILES} DESTINATION ${OSAC_BINARY_DIR}/cfg/)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION share/osac FILES_MATCHING PATTERN "*.xml")
