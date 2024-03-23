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

tgui_set_global_compile_flags(gui-builder)
tgui_set_stdlib(gui-builder)

if(TGUI_COMPILER_MSVC)
    # Visual Studio requires the Gui Builder to be build with /bigobj when compiling for x64 (and we are enabling it for x86 as well)
    target_compile_options(gui-builder PRIVATE /bigobj)
endif()

# Copy the executable to the gui-builder folder
add_custom_command(TARGET gui-builder POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:gui-builder>" "${PROJECT_SOURCE_DIR}/gui-builder/"
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/themes" "${PROJECT_SOURCE_DIR}/gui-builder/themes"
                   VERBATIM)

# Copy the resources folder to the build directory to execute the gui-folder from there
add_custom_command(TARGET gui-builder
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/gui-builder/resources" "$<TARGET_FILE_DIR:gui-builder>/resources"
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/gui-builder/themes" "$<TARGET_FILE_DIR:gui-builder>/themes"
                   VERBATIM)

set(target_install_dir "${TGUI_MISC_INSTALL_PREFIX}/gui-builder")

copy_dlls_to_exe("${PROJECT_SOURCE_DIR}/gui-builder/" "${target_install_dir}" gui-builder)

# Set the RPATH of the executable on Linux (and BSD)
if (TGUI_SHARED_LIBS AND TGUI_OS_LINUX)
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
        cmake_path(SET lib_dir NORMALIZE "${CMAKE_INSTALL_PREFIX}")
        cmake_path(APPEND lib_dir "${CMAKE_INSTALL_LIBDIR}")

        cmake_path(SET exe_dir NORMALIZE "${CMAKE_INSTALL_PREFIX}")
        cmake_path(APPEND exe_dir "${target_install_dir}")

        set(rel_lib_dir "${lib_dir}")
        cmake_path(RELATIVE_PATH rel_lib_dir BASE_DIRECTORY "${exe_dir}")
    else()
        # This code for CMake < 3.20 only works if CMAKE_INSTALL_PREFIX is an absolute path
        file(RELATIVE_PATH rel_lib_dir
             "${CMAKE_INSTALL_PREFIX}/${target_install_dir}"
             "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    endif()

    set_target_properties(gui-builder PROPERTIES
                          INSTALL_RPATH "$ORIGIN/${rel_lib_dir}"  # RPATH will contain relative path to where we installed our own library
                          INSTALL_RPATH_USE_LINK_PATH TRUE)       # RPATH will contain paths to our dependencies
endif()

if (TGUI_INSTALL)
    # Install a .desktop file on Linux (and its icon)
    if (TGUI_OS_LINUX)
        set(TGUI_GUI_BUILDER_INSTALL_EXE_PATH "${CMAKE_INSTALL_PREFIX}/${target_install_dir}/gui-builder")
        configure_file("${PROJECT_SOURCE_DIR}/cmake/gui-builder/tgui-gui-builder.desktop.in" "tgui-gui-builder.desktop" @ONLY)
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tgui-gui-builder.desktop"
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
                COMPONENT gui-builder)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/gui-builder/TexusGUI.png"
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps"
                COMPONENT gui-builder)
    endif()

    # Add the install rule for the executable
    install(TARGETS gui-builder
            RUNTIME DESTINATION ${target_install_dir} COMPONENT gui-builder
            BUNDLE DESTINATION ${target_install_dir} COMPONENT gui-builder)

    # Install the resources next to the gui-builder executable
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/gui-builder/resources"
            DESTINATION "${target_install_dir}"
            COMPONENT gui-builder)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/themes"
            DESTINATION "${target_install_dir}"
            COMPONENT gui-builder)
endif()
