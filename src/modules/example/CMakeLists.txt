set(EXAMPLE_BIN example)

set(EXAMPLE_SRCS
    examplemodule.cpp
    init.cpp
)

add_library(${EXAMPLE_BIN} SHARED ${EXAMPLE_SRCS})

set_target_properties(${EXAMPLE_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../..")
