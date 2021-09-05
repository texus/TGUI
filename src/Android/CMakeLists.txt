####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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

# Create the target
add_library(tgui-activity SHARED TGUIActivity.cpp)

# Define the export symbol of the module
string(REPLACE "-" "_" NAME_UPPER "tgui-activity")
string(TOUPPER "${NAME_UPPER}" NAME_UPPER)
target_compile_definitions(tgui-activity PRIVATE ${NAME_UPPER}_EXPORTS)

# Add a -d suffix when in debug mode
set_target_properties(tgui-activity PROPERTIES DEBUG_POSTFIX -d)

# Hide public symbols by default (exported ones are explicitly marked)
target_compile_options(tgui-activity PRIVATE -fvisibility=hidden)

if (CMAKE_ANDROID_STL_TYPE MATCHES "_shared")
    target_compile_definitions(tgui-activity PRIVATE STL_LIBRARY=${CMAKE_ANDROID_STL_TYPE})
endif()

tgui_set_global_compile_flags(tgui-activity)
target_compile_options(tgui-activity PRIVATE -fpermissive)
target_link_libraries(tgui-activity PRIVATE android log)

install(TARGETS tgui-activity
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        FRAMEWORK DESTINATION "." COMPONENT bin)

# Install Android.mk so the NDK knows how to set up TGUI
if(TGUI_OS_ANDROID)
    install(FILES Android.mk DESTINATION .)
endif()
