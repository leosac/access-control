set(OSAC_SRCS
  main.cpp
)

add_executable(osac ${OSAC_SRCS} ${OSAC_RCC_SRCS} ${MOC_SOURCES})

list(APPEND CMAKE_CXX_FLAGS ${CXX11_FLAGS})

target_link_libraries(osac)

install(TARGETS osac RUNTIME DESTINATION bin)
