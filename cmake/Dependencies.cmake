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

# Find SFML, but don't add it as a dependency to TGUI (used for e.g. building the examples and Gui Builder)
macro(tgui_find_dependency_sfml component optional_quiet)
    if(NOT TARGET sfml-${component})
        # Link SFML in the same way as TGUI, unless SFML_STATIC_LIBRARIES is manually specified
        if(NOT DEFINED SFML_STATIC_LIBRARIES)
            tgui_assign_bool(SFML_STATIC_LIBRARIES NOT TGUI_SHARED_LIBS)
        endif()

        if(TGUI_SHARED_LIBS AND SFML_STATIC_LIBRARIES)
            # The user has to link SFML in his own program, which would conflict with the one already inside the TGUI dll.
            message(FATAL_ERROR "Linking statically to SFML isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SFML library by setting SFML_STATIC_LIBRARIES to FALSE.")
        endif()

        if(TGUI_OS_ANDROID AND NOT SFML_DIR AND NOT SFML_PATH)
            # Search for SFML in the android NDK (if no other directory is specified).
            # Passing PATHS or HINTS to find_package doesn't seem to work anymore, unless we
            # set CMAKE_FIND_ROOT_PATH_MODE_PACKAGE to NEVER. So we just set SFML_DIR directly.
            set(SFML_DIR "${CMAKE_ANDROID_NDK}/sources/third_party/sfml/lib/${CMAKE_ANDROID_ARCH_ABI}/cmake/SFML")
            find_package(SFML CONFIG ${optional_quiet} COMPONENTS ${component})
        elseif(TGUI_OS_IOS)  # Use the find_host_package macro from the toolchain on iOS
            find_host_package(SFML CONFIG ${optional_quiet} COMPONENTS ${component})
        else()
            find_package(SFML CONFIG ${optional_quiet} COMPONENTS ${component})
        endif()

        # Don't let SFML_DOC_DIR pollute the cmake entries, unless the variable was set by the user
        if(NOT SFML_DOC_DIR)
            unset(SFML_DOC_DIR CACHE)
        endif()

        # find_package couldn't find SFML
        if(NOT SFML_FOUND)
            set(SFML_DIR "" CACHE PATH "Path to SFMLConfig.cmake")
            message(FATAL_ERROR
                "CMake couldn't find SFML.\n"
                "Set SFML_DIR to the directory containing SFMLConfig.cmake (usually something like SFML_ROOT/lib/cmake/SFML)\n")
        endif()

        if (${SFML_VERSION} VERSION_LESS "2.5.0")
            message(FATAL_ERROR "SFML 2.5 or higher is required")
        endif()
    endif()
endmacro()

# Find SFML and add it as a dependency (component should be either "graphics", "window" or "main")
macro(tgui_add_dependency_sfml component)
    tgui_find_dependency_sfml(${component} "")

    # Link to SFML and set include and library search directories
    target_link_libraries(tgui PUBLIC sfml-${component})
endmacro()


# Check if we can find SDL with a config file, or whether we need to use our FindSDL2.cmake file
function(tgui_try_find_sdl_config)
    find_package(SDL2 CONFIG QUIET)

    # Not all SDL config file are created equal. On ubuntu 20.04 it doesn't seem to create a target yet.
    # If the config file doesn't provide both the SDL2::SDL2 and SDL2::SDL2main targets then we will
    # ignore it and still fall back to our own FindSDL2.cmake file.
    if(SDL2_FOUND AND TARGET SDL2::SDL2 AND TARGET SDL2::SDL2main)
        set(TGUI_FOUND_SDL2_CONFIG TRUE PARENT_SCOPE)
    else()
        set(TGUI_FOUND_SDL2_CONFIG FALSE PARENT_SCOPE)

        # Remove the empty SDL2_DIR variable as it will cause confusion with SDL2_PATH which we will create next
        if(NOT SDL2_DIR)
            unset(SDL2_DIR CACHE)
        endif()
    endif()
endfunction()

# Find SDL, but don't add it as a dependency to TGUI (used for e.g. building the examples and Gui Builder)
macro(tgui_find_dependency_sdl)
    if(NOT TARGET SDL2::SDL2)
        # First try looking for an SDL config file, which will be found on Linux or when using vcpkg
        if(NOT TGUI_SKIP_SDL_CONFIG) # e.g. to skip macOS config file when building for iOS
            tgui_try_find_sdl_config()
        endif()

        if(TGUI_FOUND_SDL2_CONFIG)
            find_package(SDL2 CONFIG REQUIRED)
        else() # If it fails then use the FindSDL2.cmake file that ships with TGUI
            if(TGUI_OS_WINDOWS)
                find_package(SDL2)
                if(NOT SDL2_FOUND)
                    message(FATAL_ERROR
                            "CMake couldn't find SDL2.\n"
                            "Set SDL2_PATH to the root folder of the Development Libraries that you downloaded from https://libsdl.org/download-2.0.php \n")
                endif()
            elseif(TGUI_OS_MACOS)
                find_package(SDL2)
                if(NOT SDL2_FOUND)
                    message(FATAL_ERROR
                            "CMake couldn't find SDL2.\n"
                            "Set SDL2_PATH to the folder that contains the 'include' and 'lib' subdirectories, or to the folder that contains the SDL2.framework file. You can get the framework file by downloading the Development Libraries from https://libsdl.org/download-2.0.php and extracting it from the downloaded .dmg file.\n")
                endif()
            else()
                # On Linux we should always find SDL2 automatically when it has been installed
                find_package(SDL2 REQUIRED)
            endif()
        endif()

        # The version doesn't seem to always be defined (e.g. Ubuntu doesn't seem to have it while Arch Linux
        # and the fallback FindSDL2.cmake module do provide the variable). So only check the version if we can.
        if(DEFINED SDL2_VERSION)
            # The minimum SDL version for using the SDL_RENDERER backend renderer lies a lot higher than what
            # the rest of the SDL code requires.
            if (TGUI_HAS_BACKEND_SDL_RENDERER OR TGUI_CUSTOM_BACKEND_HAS_RENDERER_SDL_RENDERER)
                if (${SDL2_VERSION} VERSION_LESS "2.0.18")
                    message(FATAL_ERROR "SDL 2.0.18 or higher is required for SDL_RENDERER backend")
                endif()
            else()
                if (${SDL2_VERSION} VERSION_LESS "2.0.6")
                    message(FATAL_ERROR "SDL 2.0.6 or higher is required")
                endif()
            endif()
        endif()
    endif()
endmacro()

# Find SDL and add it as a dependency
macro(tgui_add_dependency_sdl)
    tgui_find_dependency_sdl()

    if(TGUI_SHARED_LIBS)
        get_target_property(sdl_target_type SDL2::SDL2 TYPE)
        if (sdl_target_type STREQUAL "STATIC_LIBRARY")
            # The user has to link SDL in his own program, which would conflict with the one already inside the TGUI dll.
            message(FATAL_ERROR "Linking statically to SDL isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SDL library.")
        endif()
    endif()

    # Link to SDL and set include and library search directories
    target_link_libraries(tgui PUBLIC SDL2::SDL2)
endmacro()


# Check if we can find GLFW with a config file, or whether we need to use our FindGLFW3.cmake file
function(tgui_try_find_glfw_config)
    find_package(glfw3 CONFIG QUIET)

    if(glfw3_FOUND AND TARGET glfw)
        set(TGUI_FOUND_GLFW_CONFIG TRUE PARENT_SCOPE)
    else()
        set(TGUI_FOUND_GLFW_CONFIG FALSE PARENT_SCOPE)
    endif()
endfunction()

# Find GLFW and add it as a dependency
macro(tgui_add_dependency_glfw)
    if(NOT TARGET glfw)
        # First try looking for an GLFW config file, which will be found on Linux or when using vcpkg
        tgui_try_find_glfw_config()

        if(TGUI_FOUND_GLFW_CONFIG)
            find_package(glfw3 CONFIG REQUIRED)

            if(NOT TGUI_OS_LINUX)
                # In case we used our find module earlier, we will remove our custom GLFW_INCLUDE_DIR and GLFW_LIBRARY variables to avoid confusion as they are no longer used
                unset(GLFW_INCLUDE_DIR CACHE)
                unset(GLFW_LIBRARY CACHE)
            endif()
        else() # If it fails then use the FindGLFW3.cmake file that ships with TGUI
            find_package(glfw3)

            if(NOT glfw3_FOUND)
                message(FATAL_ERROR
                    "CMake couldn't find GLFW3.\n"
                    "If GLFW was build with CMake then set the glfw3_DIR variable to the directory containing glfw3Config.cmake (i.e. GLFW_ROOT/lib/cmake/glfw3)\n"
                    "Alternatively you can manually set GLFW_INCLUDE_DIR to the 'include' directory and GLFW_LIBRARY to the correct library file. You are responsible for making sure the selected library is compatible.\n")
            endif()

            # Remove the empty glfw3_DIR variable if we found GLFW via the alternative way
            if(NOT glfw3_DIR)
                unset(glfw3_DIR CACHE)
            endif()
        endif()

        if(DEFINED glfw3_VERSION)
            if (${glfw3_VERSION} VERSION_LESS "3.2")
                message(FATAL_ERROR "GLFW 3.2 or higher is required")
            endif()
        endif()
    endif()

    if(TGUI_SHARED_LIBS)
        get_target_property(glfw_target_type glfw TYPE)
        if (glfw_target_type STREQUAL "STATIC_LIBRARY")
            # The user has to link GLFW in his own program, which would conflict with the one already inside the TGUI dll.
            # Note that we might not always detect this, glfw_target_type could be unknown, e.g. when manually specifying the library via GLFW_LIBRARY.
            message(FATAL_ERROR "Linking statically to GLFW isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic GLFW library.")
        endif()
    endif()

    # Link to GLFW and set include and library search directories
    target_link_libraries(tgui PUBLIC glfw)
endmacro()


# Check if we can find SDL_ttf with a config file, or whether we need to use our FindSDL2_ttf.cmake file
function(tgui_try_find_sdl_ttf_config)
    find_package(sdl2-ttf CONFIG QUIET)
    if(SDL2_TTF_FOUND)
        set(TGUI_FOUND_SDL2_TTF_CONFIG TRUE PARENT_SCOPE)
    else()
        set(TGUI_FOUND_SDL2_TTF_CONFIG FALSE PARENT_SCOPE)

        # Don't keep the sdl2-ttf_DIR variable around if we aren't going to use it
        if(NOT sdl2-ttf_DIR)
            unset(sdl2-ttf_DIR CACHE)
        endif()
    endif()
endfunction()

# Find SDL_ttf and add it as a dependency
macro(tgui_add_dependency_sdl_ttf)
    if(NOT TARGET SDL2::SDL2_ttf)
        # First try looking for an SDL2_ttf config file that will probably only be found when using vcpkg
        tgui_try_find_sdl_ttf_config()

        if(TGUI_FOUND_SDL2_TTF_CONFIG)
            find_package(sdl2-ttf CONFIG REQUIRED)
        else()
            if(TGUI_OS_WINDOWS)
                find_package(SDL2_ttf)
                if(NOT SDL2_TTF_FOUND)
                    message(FATAL_ERROR
                            "CMake couldn't find SDL2_ttf.\n"
                            "Set SDL2_TTF_PATH to the root folder of the Development Libraries that you downloaded from https://libsdl.org/projects/SDL_ttf/ \n")
                endif()
            elseif(TGUI_OS_MACOS)
                find_package(SDL2_ttf)

                if(SDL2_TTF_FOUND)
                    # If found automatically (e.g. when installed via brew) then don't keep the SDL2_TTF_PATH variable
                    if(NOT SDL2_TTF_PATH)
                        unset(SDL2_TTF_PATH CACHE)
                    endif()
                else()
                    message(FATAL_ERROR
                            "CMake couldn't find SDL2_ttf.\n"
                            "Set SDL2_TTF_PATH to the folder that contains the 'include' and 'lib' subdirectories, or to the folder that contains the SDL2_ttf.framework file.\n")
                endif()
            else()
                # On Linux we should always find SDL2_ttf automatically when it has been installed
                find_package(SDL2_ttf REQUIRED)
            endif()
        endif()

        if(DEFINED SDL2_TTF_VERSION)
            if (${SDL2_TTF_VERSION} VERSION_LESS "2.0.14")
                message(FATAL_ERROR "SDL_ttf 2.0.14 or higher is required")
            endif()
        endif()
    endif()

    if(TGUI_SHARED_LIBS)
        get_target_property(sdl_ttf_target_type SDL2::SDL2_ttf TYPE)
        if (sdl_ttf_target_type STREQUAL "STATIC_LIBRARY")
            # The user has to link SDL_ttf in his own program, which would conflict with the one already inside the TGUI dll.
            message(FATAL_ERROR "Linking statically to SDL_ttf isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SDL_ttf library.")
        endif()
    endif()

    # Link to SDL_ttf and set include and library search directories.
    # Public dependency because user has to call TTF_Init and TTF_Quit
    target_link_libraries(tgui PUBLIC SDL2::SDL2_ttf)
endmacro()


# Find FreeType and add it as a dependency
macro(tgui_add_dependency_freetype)

    if(NOT TARGET Freetype::Freetype)
        if(TGUI_OS_WINDOWS AND TGUI_COMPILER_MSVC)
            # On windows we will provide some help to find freetype (since it is more difficult on this platform).
            # We only do this for MSVC because FreeType only provides prebuilt binaries for this compiler.
            if(FREETYPE_WINDOWS_BINARIES_PATH AND NOT FREETYPE_INCLUDE_DIR_ft2build)
                set(FREETYPE_INCLUDE_DIR_ft2build "${FREETYPE_WINDOWS_BINARIES_PATH}/include")
            endif()
            if(FREETYPE_WINDOWS_BINARIES_PATH AND NOT FREETYPE_INCLUDE_DIR_freetype2)
                set(FREETYPE_INCLUDE_DIR_freetype2 "${FREETYPE_WINDOWS_BINARIES_PATH}/include")
            endif()
            if(FREETYPE_WINDOWS_BINARIES_PATH AND NOT FREETYPE_LIBRARY_RELEASE AND NOT FREETYPE_LIBRARY)

                # The static prebuilt libraries from FreeType are build with the /MT option, so we can only use them if TGUI_USE_STATIC_STD_LIBS is ON.
                # This is why we will use the dll version by default, even when TGUI is being build statically.
                if(TGUI_USE_STATIC_STD_LIBS)
                    # Note that the 2.11.0 release used "vs2015-2019" while 2.11.1 uses "vs2015-2022". We only check for the latter
                    # because 2.11.0 had a major bug that caused a segmentation fault when trying to use it. So that version won't work anyway.
                    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                        set(expected_freetype_library_location "${FREETYPE_WINDOWS_BINARIES_PATH}/release static/vs2015-2022/win64/freetype.lib")
                    else()
                        set(expected_freetype_library_location "${FREETYPE_WINDOWS_BINARIES_PATH}/release static/vs2015-2022/win32/freetype.lib")
                    endif()
                else()
                    # Search for the file structure used since the 2.11 release
                    if(EXISTS "${FREETYPE_WINDOWS_BINARIES_PATH}/release dll/win64/freetype.lib")
                        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                            set(expected_freetype_library_location "${FREETYPE_WINDOWS_BINARIES_PATH}/release dll/win64/freetype.lib")
                        else()
                            set(expected_freetype_library_location "${FREETYPE_WINDOWS_BINARIES_PATH}/release dll/win32/freetype.lib")
                        endif()

                    # If not found then check if the file structure corresponds to releases prior to 2.11
                    elseif(EXISTS "${FREETYPE_WINDOWS_BINARIES_PATH}/win64/freetype.lib")
                        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                            set(expected_freetype_library_location "${FREETYPE_WINDOWS_BINARIES_PATH}/win64/freetype.lib")
                        else()
                            set(expected_freetype_library_location "${FREETYPE_WINDOWS_BINARIES_PATH}/win32/freetype.lib")
                        endif()
                    endif()
                endif()

                # We only set FREETYPE_LIBRARY_RELEASE if the file exists, because the find module always thinks
                # that freetype is found when this option is set, even if the option is set to an invalid value.
                if(EXISTS ${expected_freetype_library_location})
                    set(FREETYPE_LIBRARY_RELEASE "${expected_freetype_library_location}")
                endif()
            endif()

            # Use CMake's built-in FindFreetype script to find FreeType
            find_package(Freetype)

            if(NOT FREETYPE_FOUND)
                set(FREETYPE_WINDOWS_BINARIES_PATH "" CACHE PATH "Path to FreeType windows binaries (contains 'include', 'release dll' and 'release static' folders)")
                message(FATAL_ERROR
                    "CMake couldn't find FreeType.\n"
                    "If you don't have FreeType installed then you can download binaries from https://github.com/ubawurinna/freetype-windows-binaries/releases/tag/v2.10.4 ('Source code (zip)' link under 'Assets') and set the FREETYPE_WINDOWS_BINARIES_PATH variable to the extracted freetype-windows-binaries folder.\n")
            endif()
        else()
            # Use CMake's built-in FindFreetype script to find FreeType
            find_package(Freetype REQUIRED)
        endif()
    endif()

    target_link_libraries(tgui PRIVATE Freetype::Freetype)
endmacro()


# Find OpenGL and add it as a dependency
macro(tgui_add_dependency_opengl)
    find_package(OpenGL REQUIRED)
    target_link_libraries(tgui PRIVATE OpenGL::GL)
endmacro()


# Find OpenGL ES and add it as a dependency
macro(tgui_add_dependency_gles2)
    if(TGUI_OS_MACOS)
        message(FATAL_ERROR "OpenGL ES isn't supported on macOS")
    endif()

    # Windows technically supports OpenGL ES, but we don't currently support it (the code below would cause linker errors on Windows)
    if(TGUI_OS_WINDOWS)
        message(FATAL_ERROR "TGUI doesn't support OpenGL ES on Windows yet")
    endif()

    target_link_libraries(tgui PRIVATE EGL GLESv2)
endmacro()


# Find X11 and add it as a dependency
macro(tgui_add_dependency_x11)
    if(NOT TGUI_OS_LINUX)
        message(FATAL_ERROR "X11 isn't supported on this platform")
    endif()

    tgui_set_option(TGUI_USE_X11 TRUE BOOL "Set to FALSE if you don't want to link to X11. Note that doing so will prevent some mouse cursors from showing correctly.")
    mark_as_advanced(TGUI_USE_X11)

    if(TGUI_USE_X11)
        find_package(X11 REQUIRED)
        target_include_directories(tgui PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(tgui PRIVATE ${X11_X11_LIB})
    endif()
endmacro()
