set(AUTHTEST_BIN authtest)

set(AUTHTEST_SRCS
    init.cpp
    authtestmodule.cpp
    authenticator.cpp
)

add_library(${AUTHTEST_BIN} SHARED ${AUTHTEST_SRCS})

set_target_properties(${AUTHTEST_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(${AUTHTEST_BIN} ${LEOSAC_LIB})

install(TARGETS ${AUTHTEST_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
