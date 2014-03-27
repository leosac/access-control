set(OSAC_SRCS
  main.cpp
  DynLib/DynamicLibrary.cpp
)

add_executable(osac ${OSAC_SRCS})

list(APPEND CMAKE_CXX_FLAGS ${CXX11_FLAGS})

target_link_libraries(osac dl pthread)

install(TARGETS osac RUNTIME DESTINATION bin)
