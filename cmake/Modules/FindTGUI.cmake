#
# Try to find TGUI library and include path.
# Once done this will define
#
# TGUI_FOUND
# TGUI_INCLUDE_PATH
# TGUI_LIBRARY
#

IF (WIN32)
    FIND_PATH( TGUI_INCLUDE_PATH TGUI/TGUI.hpp
        $ENV{TGUI_ROOT}/include
        ${TGUI_ROOT}/include
        ${PROJECT_SOURCE_DIR}/src/nvgl/TGUI/include
        DOC "The directory where TGUI.hpp resides")
    FIND_LIBRARY( TGUI_LIBRARY
        NAMES TGUI
        PATHS
        $ENV{TGUI_ROOT}/build/lib/Release
        ${TGUI_ROOT}/build/lib/Release
        ${PROJECT_SOURCE_DIR}/TGUI/bin
        ${PROJECT_SOURCE_DIR}/TGUI/lib
        DOC "The TGUI library")
ELSE (WIN32)
    FIND_PATH( TGUI_INCLUDE_PATH TGUI/TGUI.hpp
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where TGUI.hpp resides")
    FIND_LIBRARY( TGUI_LIBRARY
        NAMES TGUI
        PATHS
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        DOC "The TGUI library")
ENDIF (WIN32)

IF (TGUI_INCLUDE_PATH)
    SET( TGUI_FOUND 1 CACHE STRING "Set to 1 if TGUI is found, 0 otherwise")
ELSE (TGUI_INCLUDE_PATH)
    SET( TGUI_FOUND 0 CACHE STRING "Set to 1 if TGUI is found, 0 otherwise")
ENDIF (TGUI_INCLUDE_PATH)

MARK_AS_ADVANCED( TGUI_FOUND )
