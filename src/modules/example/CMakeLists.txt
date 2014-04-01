set(EXAMPLE_BIN example)

set(EXAMPLE_SRCS
    examplemodule.cpp
    examplemoduleloader.cpp
    init.cpp
    ../amoduleloader.cpp
)

add_library(${EXAMPLE_BIN} SHARED ${EXAMPLE_SRCS})

#Add symbol export options flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

target_link_libraries(${EXAMPLE_BIN})

install(TARGETS ${EXAMPLE_BIN} LIBRARY DESTINATION bin)
