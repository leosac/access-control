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
