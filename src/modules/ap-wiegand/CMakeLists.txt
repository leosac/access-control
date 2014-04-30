set(WIEGAND_BIN wiegand)

set(WIEGAND_SRCS
    wiegandmodule.cpp
    init.cpp
)

add_library(${WIEGAND_BIN} SHARED ${WIEGAND_SRCS})

#Add symbol exporting options flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

target_link_libraries(${WIEGAND_BIN})

install(TARGETS ${WIEGAND_BIN} LIBRARY DESTINATION bin)
