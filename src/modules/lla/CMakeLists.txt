set(LLA_BIN lla)

set(LLA_SRCS
        init.cpp
        LLAModule.cpp
        Worker.cpp
        )

add_library(${LLA_BIN} SHARED ${LLA_SRCS})
target_link_libraries(${LLA_BIN} logicalaccess)
set_target_properties(${LLA_BIN} PROPERTIES
        # -DUNIX=1 is to tell logicalaccess we are building for UNIX
        COMPILE_FLAGS "${MODULE_COMPILE_FLAGS} -DUNIX=1"
        )

install(TARGETS ${LLA_BIN} DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
