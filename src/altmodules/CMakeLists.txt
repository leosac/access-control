
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(MODULE_COMPILE_FLAGS "${LEOSAC_COMPILE_FLAGS}")
endif ()


## Automatically add all subdirectories here.

macro(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    foreach (child ${children})
        if (IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        endif ()
    endforeach ()
    SET(${result} ${dirlist})
endmacro()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(LEOSAC_MODULE_INSTALL_DIR lib/leosac)

foreach (subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach ()
