
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(MODULE_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS} -fvisibility=hidden")
endif()

set(LEOSAC_MODULE_INSTALL_DIR lib/leosac/auth)

add_subdirectory(auth-file)
