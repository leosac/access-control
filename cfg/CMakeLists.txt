
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CONFIG_FILES
    default.xml
    defaultDebug.xml
    hardwareCubie2.xml
    hardwarePi.xml
    hardwareDebug.xml
    auth.conf
    doorA.conf
    doorB.conf
)

file(COPY ${CONFIG_FILES} DESTINATION ${LEOSAC_BINARY_DIR}/cfg/)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION share/leosac FILES_MATCHING PATTERN "*.xml")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION share/leosac FILES_MATCHING PATTERN "*.conf")
