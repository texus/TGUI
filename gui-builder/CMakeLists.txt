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

if(NOT TGUI_DEFAULT_BACKEND)
    message(WARNING "Warning: Skipping building Gui Builder. The TGUI_BUILD_GUI_BUILDER option was TRUE but no backend was selected")
    return()
endif()

set(GUI_BUILDER_SOURCES
    src/main.cpp
    src/Form.cpp
    src/GuiBuilder.cpp
)

# Make a GUI application on windows (without having the command line window)
if(TGUI_OS_WINDOWS)
    set(GUI_TYPE WIN32)
endif()

add_executable(gui-builder ${GUI_TYPE} ${GUI_BUILDER_SOURCES})
target_include_directories(gui-builder PRIVATE include)
target_link_libraries(gui-builder PRIVATE tgui-gui-app-interface)

# If we use CMake < 3.13 then we can't add link options to tgui-gui-app-interface and have to add them manually for each app
if(TGUI_OS_WINDOWS AND (${CMAKE_VERSION} VERSION_LESS "3.13") AND (${TGUI_DEFAULT_BACKEND} STREQUAL "GLFW_OPENGL3" OR ${TGUI_DEFAULT_BACKEND} STREQUAL "GLFW_GLES2"))
    if(TGUI_COMPILER_MSVC)
        target_link_options(gui-builder PRIVATE "/ENTRY:mainCRTStartup" "/SUBSYSTEM:WINDOWS")
    else()
        target_link_options(gui-builder PRIVATE "-mwindow")
    endif()
endif()

tgui_set_global_compile_flags(gui-builder)
tgui_set_stdlib(gui-builder)

# Copy the executable to the gui-builder folder
add_custom_command(TARGET gui-builder POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:gui-builder>" "${PROJECT_SOURCE_DIR}/gui-builder/"
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/themes" "${PROJECT_SOURCE_DIR}/gui-builder/themes")

set(target_install_dir "${TGUI_MISC_INSTALL_PREFIX}/gui-builder")

copy_dlls_to_exe("${PROJECT_SOURCE_DIR}/gui-builder/" "${target_install_dir}" gui-builder)

# Set the RPATH of the executable on Linux (and BSD)
if (TGUI_SHARED_LIBS AND TGUI_OS_LINUX)
    file(RELATIVE_PATH rel_lib_dir
         ${CMAKE_INSTALL_PREFIX}/${target_install_dir}
         ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

    set_target_properties(gui-builder PROPERTIES
                          INSTALL_RPATH "$ORIGIN/${rel_lib_dir}")
endif()

# Add the install rule for the executable
install(TARGETS gui-builder
        RUNTIME DESTINATION ${target_install_dir} COMPONENT gui-builder
        BUNDLE DESTINATION ${target_install_dir} COMPONENT gui-builder)

# Install the resources next to the test executable
install(DIRECTORY "${PROJECT_SOURCE_DIR}/gui-builder/resources"
        DESTINATION "${target_install_dir}"
        COMPONENT gui-builder)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/themes"
        DESTINATION "${target_install_dir}"
        COMPONENT gui-builder)
