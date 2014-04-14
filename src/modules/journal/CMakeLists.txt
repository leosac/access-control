set(JOURNAL_BIN journal)

set(JOURNAL_SRCS
    journallogger.cpp
    init.cpp
)

add_library(${JOURNAL_BIN} SHARED ${JOURNAL_SRCS})

#Add symbol exporting options flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

target_link_libraries(${JOURNAL_BIN})

install(TARGETS ${JOURNAL_BIN} LIBRARY DESTINATION bin)
