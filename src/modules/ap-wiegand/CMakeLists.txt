set(WIEGAND_BIN wiegand)

set(WIEGAND_SRCS
    wiegandmodule.cpp
    init.cpp
)

add_library(${WIEGAND_BIN} SHARED ${WIEGAND_SRCS})

set_target_properties(${WIEGAND_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../..")
