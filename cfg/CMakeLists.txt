
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CONFIG_FILES
    default.xml
    hardware.xml
)

file(COPY ${CONFIG_FILES} DESTINATION ${OSAC_BINARY_DIR}/cfg/)
