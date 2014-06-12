
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(MODULE_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(MODULE_COMPILE_FLAGS "${MODULE_COMPILE_FLAGS} -Wall -Wextra -Wshadow -Wundef")
    endif()
endif()

add_subdirectory(ap-wiegand)
add_subdirectory(authtest)
add_subdirectory(door)
add_subdirectory(journal)
# add_subdirectory(rpleth)
