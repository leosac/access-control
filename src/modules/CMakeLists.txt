
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(MODULE_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -fvisibility=hidden")
endif()

set(LEOSAC_MODULE_INSTALL_DIR lib/leosac)

add_subdirectory(ap-wiegand)
add_subdirectory(authtest)
add_subdirectory(door)
add_subdirectory(journal)
add_subdirectory(ledmonitor)
add_subdirectory(rpleth)
add_subdirectory(sysfsgpio)
add_subdirectory(stdin-controller)
add_subdirectory(bench-toggle)
add_subdirectory(persistent-monitor)
add_subdirectory(pifacedigital)
add_subdirectory(wiegand)
