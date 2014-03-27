set(OSAC_SRCS
  main.cpp
  dynlib/dynamiclibrary.cpp
  thread/thread.cpp
  thread/mutex.cpp
  thread/scopelock.cpp
)

add_executable(osac ${OSAC_SRCS})

list(APPEND CMAKE_CXX_FLAGS ${CXX11_FLAGS})

target_link_libraries(osac dl pthread)

install(TARGETS osac RUNTIME DESTINATION bin)
