set(SMTP_BIN smtp)

set(SMTP_SRCS
    init.cpp
    SMTP.cpp
)

add_library(${SMTP_BIN} SHARED ${SMTP_SRCS})

set_target_properties(${SMTP_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    )

target_link_libraries(${SMTP_BIN} curl)

install(TARGETS ${SMTP_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
