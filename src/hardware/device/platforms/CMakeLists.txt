if(LEOSAC_PLATFORM_UPPERCASE STREQUAL "CUBIEBOARD")
    message(STATUS "Enabled cubieboard platform features")
    set(LEOSAC_SRCS ${LEOSAC_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/cubieboard/test.cpp
        PARENT_SCOPE
    )
endif()
