set(JOURNAL_BIN journal)

set(JOURNAL_SRCS
    journallogger.cpp
    init.cpp
)

add_library(${JOURNAL_BIN} SHARED ${JOURNAL_SRCS})

set_target_properties(${JOURNAL_BIN} PROPERTIES
    COMPILE_FLAGS "${MODULE_COMPILE_FLAGS}"
    INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src")
