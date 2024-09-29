####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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
Findraylib
----------

Defines an import target "raylib" based on the raylib_INCLUDE_DIR and raylib_LIBRARY variables.

The raylib_VERSION variable will be set to the found raylib version.

TGUI first searches for a config file in raylib_DIR, which is the recommended way to find raylib.
If this fails then this find script is used so that you can specify the include directory and library manually.

This script is intended for when you download the precompiled raylib release which lacks a cmake config file.
#]=======================================================================]

set(raylib_INCLUDE_DIR "" CACHE PATH "Include directory of raylib")
set(raylib_LIBRARY "" CACHE FILEPATH "raylib library to link to")

# Read raylib version
if(raylib_INCLUDE_DIR AND EXISTS "${raylib_INCLUDE_DIR}/raylib.h")
  file(STRINGS "${raylib_INCLUDE_DIR}/raylib.h" raylib_VERSION_MAJOR_LINE REGEX "^#define[ \t]+RAYLIB_VERSION_MAJOR[ \t]+[0-9]+$")
  file(STRINGS "${raylib_INCLUDE_DIR}/raylib.h" raylib_VERSION_MINOR_LINE REGEX "^#define[ \t]+RAYLIB_VERSION_MINOR[ \t]+[0-9]+$")
  file(STRINGS "${raylib_INCLUDE_DIR}/raylib.h" raylib_VERSION_PATCH_LINE REGEX "^#define[ \t]+RAYLIB_VERSION_PATCH[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+RAYLIB_VERSION_MAJOR[ \t]+([0-9]+)$" "\\1" raylib_VERSION_MAJOR "${raylib_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+RAYLIB_VERSION_MINOR[ \t]+([0-9]+)$" "\\1" raylib_VERSION_MINOR "${raylib_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+RAYLIB_VERSION_PATCH[ \t]+([0-9]+)$" "\\1" raylib_VERSION_PATCH "${raylib_VERSION_PATCH_LINE}")
  set(raylib_VERSION ${raylib_VERSION_MAJOR}.${raylib_VERSION_MINOR}.${raylib_VERSION_PATCH})
  unset(raylib_VERSION_MAJOR_LINE)
  unset(raylib_VERSION_MINOR_LINE)
  unset(raylib_VERSION_PATCH_LINE)
  unset(raylib_VERSION_MAJOR)
  unset(raylib_VERSION_MINOR)
  unset(raylib_VERSION_PATCH)
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(raylib
                                  REQUIRED_VARS raylib_INCLUDE_DIR raylib_LIBRARY
                                  VERSION_VAR raylib_VERSION)

if(raylib_FOUND)
  if(raylib_LIBRARY AND NOT TARGET raylib)
    add_library(raylib UNKNOWN IMPORTED)
    set_target_properties(raylib PROPERTIES
                          IMPORTED_LOCATION "${raylib_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${raylib_INCLUDE_DIR}")

    get_filename_component(raylib_LIBRARY_FILENAME "${raylib_LIBRARY}" NAME)
    if(raylib_LIBRARY_FILENAME STREQUAL "raylibdll.lib" OR raylib_LIBRARY_FILENAME STREQUAL "rayliblibdll.a")
        set_target_properties(raylib PROPERTIES
                              INTERFACE_COMPILE_DEFINITIONS "USE_LIBTYPE_SHARED")
    endif()
  endif()
endif()
