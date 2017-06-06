# This script locates the TGUI library
# ------------------------------------
#
# Usage
# -----
#
# You can enforce a specific version, in the form of MAJOR.MINOR.
# If nothing is specified, the version won't be checked (i.e. any version will be accepted).
# example:
#   find_package(TGUI)     // no specific version required
#   find_package(TGUI 0.7) // version 0.7
#
# By default, the dynamic libraries of TGUI will be found. To find the static ones instead,
# you must set the SFML_STATIC_LIBRARIES variable to TRUE before calling find_package(TGUI).
# Note that it is SFML_STATIC_LIBRARIES and not TGUI_STATIC_LIBRARIES, since both libraries need to be linked in the same way.
# example:
#   set(SFML_STATIC_LIBRARIES TRUE)
#   find_package(SFML 2 COMPONENTS system window graphics)
#   find_package(TGUI 0.7)
#
# If TGUI is not installed in a standard path, you can use the TGUI_ROOT CMake (or environment) variable
# to tell CMake where TGUI is.
#
# Output
# ------
#
# This script defines the following variables:
# - TGUI_LIBRARY_DEBUG:   the name of the debug library (set to TGUI_LIBRARY_RELEASE is no debug version is found)
# - TGUI_LIBRARY_RELEASE: the name of the release library (set to TGUI_LIBRARY_DEBUG is no release version is found)
# - TGUI_LIBRARY:         the name of the library to link to (includes both debug and optimized names if necessary)
# - TGUI_FOUND:           true if either the debug or release library is found
# - TGUI_INCLUDE_DIR:     the path where TGUI headers are located (the directory containing the TGUI/Config.hpp file)
#
# example:
#   find_package(SFML 2 COMPONENTS system window graphics REQUIRED)
#   find_package(TGUI 0.7 REQUIRED)
#   include_directories(${SFML_INCLUDE_DIR} ${TGUI_INCLUDE_DIR})
#   add_executable(myapp ...)
#   target_link_libraries(myapp ${SFML_LIBRARIES} ${TGUI_LIBRARY})
#

# define the list of search paths for headers and libraries
set(FIND_TGUI_PATHS
    ${TGUI_ROOT}
    $ENV{TGUI_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)

# find the TGUI include directory
find_path(TGUI_INCLUDE_DIR TGUI/Config.hpp
          PATH_SUFFIXES include
          PATHS ${FIND_TGUI_PATHS})

# check the version number
set(TGUI_VERSION_OK TRUE)
if(TGUI_INCLUDE_DIR)
    # extract the major and minor version numbers from TGUI/Config.hpp
    # we have to handle framework a little bit differently:
    if("${TGUI_INCLUDE_DIR}" MATCHES "TGUI.framework")
        set(TGUI_CONFIG_HPP_INPUT "${TGUI_INCLUDE_DIR}/Headers/Config.hpp")
    else()
        set(TGUI_CONFIG_HPP_INPUT "${TGUI_INCLUDE_DIR}/TGUI/Config.hpp")
    endif()
    FILE(READ "${TGUI_CONFIG_HPP_INPUT}" TGUI_CONFIG_HPP_CONTENTS)
    STRING(REGEX MATCH ".*#define TGUI_VERSION_MAJOR ([0-9]+).*#define TGUI_VERSION_MINOR ([0-9]+).*#define TGUI_VERSION_PATCH ([0-9]+).*" TGUI_CONFIG_HPP_CONTENTS "${TGUI_CONFIG_HPP_CONTENTS}")
    STRING(REGEX REPLACE ".*#define TGUI_VERSION_MAJOR ([0-9]+).*" "\\1" TGUI_VERSION_MAJOR "${TGUI_CONFIG_HPP_CONTENTS}")
    STRING(REGEX REPLACE ".*#define TGUI_VERSION_MINOR ([0-9]+).*" "\\1" TGUI_VERSION_MINOR "${TGUI_CONFIG_HPP_CONTENTS}")
    STRING(REGEX REPLACE ".*#define TGUI_VERSION_PATCH ([0-9]+).*" "\\1" TGUI_VERSION_PATCH "${TGUI_CONFIG_HPP_CONTENTS}")

    if (TGUI_FIND_VERSION)
        math(EXPR TGUI_REQUESTED_VERSION "${TGUI_FIND_VERSION_MAJOR} * 10000 + ${TGUI_FIND_VERSION_MINOR} * 100 + ${TGUI_FIND_VERSION_PATCH}")

        # transform version numbers to an integer
        math(EXPR TGUI_VERSION "${TGUI_VERSION_MAJOR} * 10000 + ${TGUI_VERSION_MINOR} * 100 + ${TGUI_VERSION_PATCH}")
        math(EXPR TGUI_VERSION_NOPATCH "${TGUI_VERSION_MAJOR} * 10000 + ${TGUI_VERSION_MINOR} * 100")

        # compare them
        if(TGUI_FIND_VERSION_PATCH)
            if(NOT TGUI_VERSION EQUAL TGUI_REQUESTED_VERSION)
                set(TGUI_VERSION_OK FALSE)
            endif()
        else()
            if(NOT TGUI_VERSION_NOPATCH EQUAL TGUI_REQUESTED_VERSION)
                set(TGUI_VERSION_OK FALSE)
            endif()
        endif()
    endif()
endif()

# find the static debug library
find_library(TGUI_LIBRARY_STATIC_DEBUG
             NAMES tgui-s-d
             PATH_SUFFIXES lib64 lib lib/TGUI build/lib build/lib/Debug
             PATHS ${FIND_TGUI_PATHS})

# find the static release library
find_library(TGUI_LIBRARY_STATIC_RELEASE
             NAMES tgui-s
             PATH_SUFFIXES lib64 lib lib/TGUI build/lib build/lib/Release
             PATHS ${FIND_TGUI_PATHS})

# find the dynamic debug library
find_library(TGUI_LIBRARY_DYNAMIC_DEBUG
             NAMES tgui-d
             PATH_SUFFIXES lib64 lib lib/TGUI build/lib build/lib/Debug
             PATHS ${FIND_TGUI_PATHS})

# find the dynamic release library
find_library(TGUI_LIBRARY_DYNAMIC_RELEASE
             NAMES tgui
             PATH_SUFFIXES lib64 lib lib/TGUI build/lib build/lib/Release
             PATHS ${FIND_TGUI_PATHS})

# choose the entries that fit the requested link type
if(SFML_STATIC_LIBRARIES)
    if(TGUI_LIBRARY_STATIC_RELEASE)
        set(TGUI_LIBRARY_RELEASE ${TGUI_LIBRARY_STATIC_RELEASE})
    endif()
    if(TGUI_LIBRARY_STATIC_DEBUG)
        set(TGUI_LIBRARY_DEBUG ${TGUI_LIBRARY_STATIC_DEBUG})
    endif()
else()
    if(TGUI_LIBRARY_DYNAMIC_RELEASE)
        set(TGUI_LIBRARY_RELEASE ${TGUI_LIBRARY_DYNAMIC_RELEASE})
    endif()
    if(TGUI_LIBRARY_DYNAMIC_DEBUG)
        set(TGUI_LIBRARY_DEBUG ${TGUI_LIBRARY_DYNAMIC_DEBUG})
    endif()
endif()

if (TGUI_LIBRARY_DEBUG OR TGUI_LIBRARY_RELEASE)
    # library found
    set(TGUI_FOUND TRUE)

    # if both are found, set TGUI_LIBRARY to contain both
    if (TGUI_LIBRARY_DEBUG AND TGUI_LIBRARY_RELEASE)
        set(TGUI_LIBRARY debug     ${TGUI_LIBRARY_DEBUG}
                         optimized ${TGUI_LIBRARY_RELEASE})
    endif()

    # if only one debug/release variant is found, set the other to be equal to the found one
    if (TGUI_LIBRARY_DEBUG AND NOT TGUI_LIBRARY_RELEASE)
        # debug and not release
        set(TGUI_LIBRARY_RELEASE ${TGUI_LIBRARY_DEBUG})
        set(TGUI_LIBRARY         ${TGUI_LIBRARY_DEBUG})
    endif()
    if (TGUI_LIBRARY_RELEASE AND NOT TGUI_LIBRARY_DEBUG)
        # release and not debug
        set(TGUI_LIBRARY_DEBUG ${TGUI_LIBRARY_RELEASE})
        set(TGUI_LIBRARY       ${TGUI_LIBRARY_RELEASE})
    endif()
else()
    # library not found
    set(TGUI_FOUND FALSE)
    set(TGUI_LIBRARY "")
endif()

# mark variables as advanced so that they won't show up unless the 'advanced' option is checked
mark_as_advanced(TGUI_LIBRARY
                 TGUI_LIBRARY_RELEASE
                 TGUI_LIBRARY_DEBUG
                 TGUI_LIBRARY_STATIC_DEBUG
                 TGUI_LIBRARY_STATIC_RELEASE
                 TGUI_LIBRARY_DYNAMIC_DEBUG
                 TGUI_LIBRARY_DYNAMIC_RELEASE)

# handle errors
if(NOT TGUI_VERSION_OK)
    # TGUI version not ok
    set(FIND_TGUI_ERROR "TGUI found in ${TGUI_INCLUDE_DIR} but version was wrong (requested: ${TGUI_FIND_VERSION}, found: ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})")
    set(TGUI_FOUND FALSE)
elseif(NOT TGUI_FOUND)
    # include directory or library not found
    set(FIND_TGUI_ERROR "Could NOT find TGUI")
endif()
if (NOT TGUI_FOUND)
    if(TGUI_FIND_REQUIRED)
        # fatal error
        message(FATAL_ERROR ${FIND_TGUI_ERROR})
    elseif(NOT TGUI_FIND_QUIETLY)
        # error but continue
        message("${FIND_TGUI_ERROR}")
    endif()
endif()

# handle success
if(TGUI_FOUND AND NOT TGUI_FIND_QUIETLY)
    message(STATUS "Found TGUI ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH} in ${TGUI_INCLUDE_DIR}")
endif()
