
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(MODULE_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")
endif()

set(LEOSAC_MODULE_INSTALL_DIR lib/leosac)

add_subdirectory(rpleth)
add_subdirectory(sysfsgpio)
add_subdirectory(stdin-controller)
add_subdirectory(bench-toggle)
add_subdirectory(monitor)
add_subdirectory(pifacedigital)
add_subdirectory(wiegand)
add_subdirectory(auth)
add_subdirectory(doorman)
add_subdirectory(led-buzzer)
add_subdirectory(test-and-reset)
add_subdirectory(instrumentation)
add_subdirectory(event-publish)
add_subdirectory(replication)
add_subdirectory(ws-notifier)
add_subdirectory(tcp-notifier)
add_subdirectory(websock-api)
add_subdirectory(smtp)
add_subdirectory(notifd)
if (LEOSAC_BUILD_MODULE_MQTT)
  add_subdirectory(mqtt)
endif ()
add_subdirectory(libgpiod)
if (LEOSAC_BUILD_MODULE_LLA)
  add_subdirectory(lla)
endif ()
add_subdirectory(alarms)
