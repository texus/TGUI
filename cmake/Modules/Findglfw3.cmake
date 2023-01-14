####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
#
# This software is provided 'as-is', without any express or implied warranty.
# In no event will the authors be held liable for any damages arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it freely,
# subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented;
#    you must not claim that you wrote the original software.
#    If you use this software in a product, an acknowledgment
#    in the product documentation would be appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such,
#    and must not be misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source distribution.
####################################################################################################

#[=======================================================================[.rst:
Findglfw3
---------

Defines an import target "glfw" based on the GLFW_INCLUDE_DIR and GLFW_LIBRARY variables.

The glfw3_VERSION variable will be set to the found GLFW version.

TGUI first searches for a config file in glfw3_DIR, which is the recommended way to find GLFW.
If this fails then this find script is used so that you can specify the include directory and library manually.

This script is intended for when you download the precompiled GLFW release which lacks a cmake config file.
#]=======================================================================]

set(GLFW_INCLUDE_DIR "" CACHE PATH "Include directory of GLFW")
set(GLFW_LIBRARY "" CACHE FILEPATH "GLFW library to link to")

# If the user sets GLFW_INCLUDE_DIR one directory too deep then automatically correct it
if(GLFW_INCLUDE_DIR AND NOT EXISTS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h" AND EXISTS "${GLFW_INCLUDE_DIR}/../GLFW/glfw3.h")
    get_filename_component(GLFW_INCLUDE_DIR_PARENT_DIR "${GLFW_INCLUDE_DIR}/.." ABSOLUTE)
    set(GLFW_INCLUDE_DIR "${GLFW_INCLUDE_DIR_PARENT_DIR}" CACHE PATH "Include directory of GLFW" FORCE)
endif()

# Read GLFW version
if(GLFW_INCLUDE_DIR AND EXISTS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h")
  file(STRINGS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h" GLFW_VERSION_MAJOR_LINE REGEX "^#define[ \t]+GLFW_VERSION_MAJOR[ \t]+[0-9]+$")
  file(STRINGS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h" GLFW_VERSION_MINOR_LINE REGEX "^#define[ \t]+GLFW_VERSION_MINOR[ \t]+[0-9]+$")
  file(STRINGS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h" GLFW_VERSION_REVISION_LINE REGEX "^#define[ \t]+GLFW_VERSION_REVISION[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+GLFW_VERSION_MAJOR[ \t]+([0-9]+)$" "\\1" GLFW_VERSION_MAJOR "${GLFW_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+GLFW_VERSION_MINOR[ \t]+([0-9]+)$" "\\1" GLFW_VERSION_MINOR "${GLFW_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+GLFW_VERSION_REVISION[ \t]+([0-9]+)$" "\\1" GLFW_VERSION_REVISION "${GLFW_VERSION_REVISION_LINE}")
  set(glfw3_VERSION ${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}.${GLFW_VERSION_REVISION})
  unset(GLFW_VERSION_MAJOR_LINE)
  unset(GLFW_VERSION_MINOR_LINE)
  unset(GLFW_VERSION_REVISION_LINE)
  unset(GLFW_VERSION_MAJOR)
  unset(GLFW_VERSION_MINOR)
  unset(GLFW_VERSION_REVISION)
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(glfw3
                                  REQUIRED_VARS GLFW_INCLUDE_DIR GLFW_LIBRARY
                                  VERSION_VAR glfw3_VERSION)

if(glfw3_FOUND)
  if(GLFW_LIBRARY AND NOT TARGET glfw)
    add_library(glfw UNKNOWN IMPORTED)
    set_target_properties(glfw PROPERTIES
                          IMPORTED_LOCATION "${GLFW_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIR}")

    get_filename_component(GLFW_LIBRARY_FILENAME "${GLFW_LIBRARY}" NAME)
    if(GLFW_LIBRARY_FILENAME STREQUAL "glfw3dll.lib" OR GLFW_LIBRARY_FILENAME STREQUAL "libglfw3dll.a")
        set_target_properties(glfw PROPERTIES
                              INTERFACE_COMPILE_DEFINITIONS "GLFW_DLL")
    endif()
  endif()
endif()
