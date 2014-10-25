

set(PIFACE_MODULE_EXTERNAL_LIB_SRCS
  ${CMAKE_SOURCE_DIR}/libmcp23s17/src/mcp23s17.c
  ${CMAKE_SOURCE_DIR}/libpifacedigital/src/pifacedigital.c
)


add_library(module_piface_deps SHARED ${PIFACE_MODULE_EXTERNAL_LIB_SRCS})
set_target_properties(module_piface_deps PROPERTIES
  LINKER_LANGUAGE CXX
    )

install(TARGETS module_piface_deps DESTINATION ${LEOSAC_MODULE_INSTALL_DIR})
