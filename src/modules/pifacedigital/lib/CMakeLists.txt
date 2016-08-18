set(PIFACE_MODULE_EXTERNAL_LIB_SRCS
        ${CMAKE_SOURCE_DIR}/deps/libmcp23s17/src/mcp23s17.c
        ${CMAKE_SOURCE_DIR}/deps/libpifacedigital/src/pifacedigital.c
        )

add_library(module_piface_deps STATIC ${PIFACE_MODULE_EXTERNAL_LIB_SRCS})

set_target_properties(module_piface_deps PROPERTIES
        COMPILE_FLAGS -fPIC
        LINKER_LANGUAGE C
        )

install(TARGETS module_piface_deps DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
