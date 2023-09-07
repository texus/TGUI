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

# Find SFML, but don't add it as a dependency to TGUI (used for e.g. building the examples and Gui Builder)
macro(tgui_find_dependency_sfml component optional_quiet)
    string(TOLOWER ${component} lowercase_component)
    if(NOT TARGET SFML::${component} AND NOT TARGET sfml-${lowercase_component})
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
        endif()

        # Quietly try to find SFML first, to figure out whether SFML 2 or SFML 3 is being used.
        # Whether found or not, we afterwards search the package again without the QUIET flag,
        # so that it can print information about the found version.
        # Note that find_package resets SFML_DIR on failure, so we need to reset it when we attempt another search.
        # We also need to reset the CACHE entry to prevent seaching sfml-main from the root cmake script to fails later on.
        set(sfml_dir_original ${SFML_DIR})
        find_package(SFML 3 CONFIG QUIET COMPONENTS ${component})
        if(SFML_FOUND)
            find_package(SFML 3 CONFIG ${optional_quiet} COMPONENTS ${component} REQUIRED)
        else()
            set(SFML_DIR ${sfml_dir_original})
            set(SFML_DIR ${SFML_DIR} CACHE PATH "Path to SFMLConfig.cmake" FORCE)
            find_package(SFML 2 CONFIG QUIET COMPONENTS ${lowercase_component})
            if(SFML_FOUND)
                find_package(SFML 2 CONFIG ${optional_quiet} COMPONENTS ${lowercase_component} REQUIRED)
            else()
                set(SFML_DIR ${sfml_dir_original})
                set(SFML_DIR ${SFML_DIR} CACHE PATH "Path to SFMLConfig.cmake" FORCE)

                # Neither SFML 2 or SFML 3 could be found. We need to show information to the user about what fails.
                message(NOTICE "\nSearching for SFML 2...\n")
                find_package(SFML 2 CONFIG COMPONENTS ${lowercase_component})

                message(NOTICE "\nSearching for SFML 3...\n")
                find_package(SFML 3 CONFIG COMPONENTS ${component})
            endif()
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

        if (SFML_VERSION VERSION_LESS "2.5.0")
            message(FATAL_ERROR "SFML 2.5 or higher is required")
        endif()
    endif()
endmacro()

# Find SFML and add it as a dependency (component should be either "graphics", "window" or "main")
macro(tgui_add_dependency_sfml component)
    tgui_find_dependency_sfml(${component} "")

    # Link to SFML and set include and library search directories
    if (SFML_VERSION VERSION_GREATER_EQUAL 3)
        target_link_libraries(tgui PUBLIC SFML::${component})
    else()
        string(TOLOWER ${component} lowercase_component)
        target_link_libraries(tgui PUBLIC sfml-${lowercase_component})
    endif()
endmacro()

# Check if we can find SDL3
function(tgui_try_find_sdl3)
    find_package(SDL3 CONFIG QUIET)

    if(SDL3_FOUND)
        set(TGUI_FOUND_SDL3 TRUE PARENT_SCOPE)
    else()
        set(TGUI_FOUND_SDL3 FALSE PARENT_SCOPE)
    endif()
endfunction()

# Check if we can find SDL2 with a config file, or whether we need to use our FindSDL2.cmake file
function(tgui_try_find_sdl2_config)
    find_package(SDL2 CONFIG QUIET)

    # Not all SDL config file are created equal. On ubuntu 20.04 a config file existed but it didn't create a target yet.
    # If no target exists then fall back to our own FindSDL2.cmake file.
    if(SDL2_FOUND AND (TARGET SDL2::SDL2 OR TARGET SDL2::SDL2-static))
        set(TGUI_FOUND_SDL2_CONFIG TRUE PARENT_SCOPE)
    else()
        set(TGUI_FOUND_SDL2_CONFIG FALSE PARENT_SCOPE)
    endif()
endfunction()

# Check if we can find SDL2 with our FindSDL2.cmake file
function(tgui_try_find_sdl2_module)
    find_package(SDL2 MODULE QUIET)

    if(SDL2_FOUND)
        set(TGUI_FOUND_SDL2_MODULE TRUE PARENT_SCOPE)
    else()
        set(TGUI_FOUND_SDL2_MODULE FALSE PARENT_SCOPE)
    endif()
endfunction()

# Find SDL, but don't add it as a dependency to TGUI (used for e.g. building the examples and Gui Builder)
macro(tgui_find_dependency_sdl)
    set(TGUI_FOUND_SDL3 FALSE)
    set(TGUI_FOUND_SDL2_CONFIG FALSE)
    set(TGUI_FOUND_SDL2_MODULE FALSE)
    if(NOT DEFINED TGUI_USE_SDL3 OR TGUI_USE_SDL3)
        if(TARGET SDL3::SDL3)
            set(TGUI_FOUND_SDL3 TRUE)
        else()
            tgui_try_find_sdl3()
        endif()
    endif()
    if(NOT DEFINED TGUI_USE_SDL3 OR NOT TGUI_USE_SDL3)
        if(TARGET SDL2::SDL2 OR TARGET SDL2::SDL2-static)
            set(TGUI_FOUND_SDL2_CONFIG TRUE)
        else()
            if(NOT TGUI_SKIP_SDL_CONFIG) # e.g. to skip macOS config file when building for iOS
                tgui_try_find_sdl2_config()
            endif()
            if(NOT TGUI_FOUND_SDL2_CONFIG)
                tgui_try_find_sdl2_module()
            endif()
        endif()
    endif()

    # If either (or both) SDL2 or SDL3 was found, but no decision was made yet about which version to search, then make the decision now.
    # An option is added to explicitly search for the other version, in case the default decision is unwanted.
    if(NOT DEFINED TGUI_USE_SDL3)
        set(description "Determines whether TGUI looks for SDL2 or SDL3")
        if(TGUI_FOUND_SDL3)
            option(TGUI_USE_SDL3 "${description}" TRUE)
        elseif(TGUI_FOUND_SDL2_CONFIG OR TGUI_FOUND_SDL2_MODULE)
            option(TGUI_USE_SDL3 "${description}" FALSE)
        else()
            message(FATAL_ERROR
                    "CMake couldn't find SDL2 or SDL3.\n"
                    "For SDL3, set SDL3_DIR to the directory containing either SDL3Config.cmake or sdl3-config.cmake\n"
                    "For SDL2, set SDL2_DIR to the directory containing either SDL2Config.cmake or sdl2-config.cmake if such files exist\n"
                    "For SDL2 < 2.24 on Windows or macOS, for SDL files downloaded from github.com/libsdl-org/SDL/releases, set SDL2_PATH to the folder containing the 'include' and 'lib' directories, or the SDL2.framework file.\n")
        endif()
    endif()

    if(TGUI_USE_SDL3)
        if(NOT TARGET SDL3::SDL3) # Only search if the target wasn't defined yet
            if(TGUI_FOUND_SDL3)
                find_package(SDL3 CONFIG REQUIRED)
            else()
                message(FATAL_ERROR
                        "CMake couldn't find SDL3.\n"
                        "Set SDL3_DIR to the directory containing either SDL3Config.cmake or sdl3-config.cmake\n"
                        "If you wish to use SDL2 instead of SDL3 then set TGUI_USE_SDL3 = FALSE.\n")
            endif()
        endif()

        # We don't need to keep options that were created for SDL2
        unset(SDL2_DIR CACHE)
        unset(SDL2_PATH CACHE)
        unset(SDL2_LIBRARY CACHE)
        unset(SDL2MAIN_LIBRARY CACHE)
        unset(SDL2_INCLUDE_DIR CACHE)
        unset(SDL2_NO_DEFAULT_PATH CACHE)
    else() # Using SDL2
        if(NOT TARGET SDL2::SDL2 AND NOT TARGET SDL2::SDL2-static) # Only search if the target wasn't defined yet
            if(TGUI_FOUND_SDL2_CONFIG)
                find_package(SDL2 CONFIG REQUIRED)

                # We don't need to keep options that were created in our FindSDL2.cmake (since we didn't use it)
                unset(SDL2_PATH CACHE)
                unset(SDL2_LIBRARY CACHE)
                unset(SDL2MAIN_LIBRARY CACHE)
                unset(SDL2_INCLUDE_DIR CACHE)
                unset(SDL2_NO_DEFAULT_PATH CACHE)
            elseif(TGUI_FOUND_SDL2_MODULE)
                find_package(SDL2 MODULE REQUIRED)

                # We don't need to keep options that were created for the SDL2 config file (since we used our FindSDL2.cmake file instead)
                unset(SDL2_DIR CACHE)
            else()
                if(TGUI_OS_WINDOWS)
                    message(FATAL_ERROR
                            "CMake couldn't find SDL2.\n"
                            "For SDL2 >= 2.24, set SDL2_DIR to the directory containing either SDL2Config.cmake or sdl2-config.cmake\n"
                            "For older SDL versions, if you downloaded SDL2-devel-2.0.XX-VC.zip from github.com/libsdl-org/SDL/releases then set SDL2_PATH to the root directory.\n"
                            "If you wish to use SDL3 instead of SDL2 then set TGUI_USE_SDL3 = TRUE.\n")
                elseif(TGUI_OS_MACOS)
                    message(FATAL_ERROR
                            "CMake couldn't find SDL2.\n"
                            "For SDL2 >= 2.24, set SDL2_DIR to the directory containing either SDL2Config.cmake or sdl2-config.cmake.\n"
                            "For older versions, set SDL2_PATH to the folder that contains the 'include' and 'lib' subdirectories, or to the folder that contains the SDL2.framework file (found in SDL2-2.0.XX.dmg from github.com/libsdl-org/SDL/releases).\n"
                            "If you wish to use SDL3 instead of SDL2 then set TGUI_USE_SDL3 = TRUE.\n")
                else()
                    message(FATAL_ERROR
                            "CMake couldn't find SDL2.\n"
                            "Set SDL2_DIR to the directory containing either SDL2Config.cmake or sdl2-config.cmake.\n"
                            "If you wish to use SDL3 instead of SDL2 then set TGUI_USE_SDL3 = TRUE.\n")
                endif()
            endif()

            # The version doesn't seem to always be defined (e.g. Ubuntu doesn't seem to have it while Arch Linux
            # and the fallback FindSDL2.cmake module do provide the variable). So only check the version if we can.
            if(DEFINED SDL2_VERSION)
                # The minimum SDL version for using the SDL_RENDERER backend renderer lies a lot higher than what
                # the rest of the SDL code requires.
                if (TGUI_HAS_BACKEND_SDL_RENDERER OR TGUI_CUSTOM_BACKEND_HAS_RENDERER_SDL_RENDERER)
                    if (SDL2_VERSION VERSION_LESS 2.0.18)
                        message(FATAL_ERROR "SDL 2.0.18 or higher is required for SDL_RENDERER backend")
                    endif()
                else()
                    if (SDL2_VERSION VERSION_LESS 2.0.6)
                        message(FATAL_ERROR "SDL 2.0.6 or higher is required")
                    endif()
                endif()
            endif()

            # SDL2main should appear before SDL2 in the linker options, at least for MinGW
            if (TARGET SDL2::SDL2main)
                if(TGUI_USE_STATIC_SDL OR (NOT DEFINED TGUI_USE_STATIC_SDL AND NOT TGUI_SHARED_LIBS) AND TARGET SDL2::SDL2-static)
                    target_link_libraries(SDL2::SDL2main INTERFACE SDL2::SDL2-static)
                else()
                    target_link_libraries(SDL2::SDL2main INTERFACE SDL2::SDL2)
                endif()
             endif()
        endif()

        # We don't need to keep options that were created for SDL3
        unset(SDL3_DIR CACHE)
    endif()
endmacro()

# Find SDL and add it as a dependency
macro(tgui_add_dependency_sdl)
    tgui_find_dependency_sdl()

    # Link to SDL and set include and library search directories.
    # TGUI_USE_STATIC_SDL allows explicitly chosing how SDL is linked. By default it is undefined and a static lib is preferred (but not required) when linking statically.
    if(TGUI_USE_SDL3)
        if(TGUI_USE_STATIC_SDL OR (NOT DEFINED TGUI_USE_STATIC_SDL AND NOT TGUI_SHARED_LIBS AND TARGET SDL3::SDL3-static))
            set(link_static_sdl_libs TRUE)
        else()
            set(link_static_sdl_libs FALSE)
        endif()

        if(link_static_sdl_libs)
            if(TGUI_SHARED_LIBS)
                # The user has to link SDL in his own program, which would conflict with the one already inside the TGUI dll.
                message(FATAL_ERROR "Linking statically to SDL isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SDL library by setting TGUI_USE_STATIC_SDL to FALSE.")
            endif()
            if(NOT TARGET SDL3::SDL3-static)
                message(FATAL_ERROR "Couldn't link to SDL3::SDL3-static, no such target exists")
            endif()
            target_link_libraries(tgui PUBLIC SDL3::SDL3-static)
        else()
            if(NOT TARGET SDL3::SDL3-shared)
                message(FATAL_ERROR "Couldn't link to SDL3::SDL3-shared, no such target exists")
            endif()
            target_link_libraries(tgui PUBLIC SDL3::SDL3-shared)
        endif()
    else() # Using SDL2
        if(TGUI_USE_STATIC_SDL AND NOT TARGET SDL2::SDL2-static)
            # If the user explicitly asks for a static target then it must exist
            message(FATAL_ERROR "Couldn't link to SDL2::SDL2-static, no such target exists")
        endif()
        if(TGUI_USE_STATIC_SDL OR (NOT DEFINED TGUI_USE_STATIC_SDL AND NOT TGUI_SHARED_LIBS))
            if(TGUI_SHARED_LIBS AND TGUI_USE_STATIC_SDL)
                # The user has to link SDL in his own program, which would conflict with the one already inside the TGUI dll.
                message(FATAL_ERROR "Linking statically to SDL isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SDL library by setting TGUI_USE_STATIC_SDL to FALSE.")
            endif()
            if(TARGET SDL2::SDL2-static)
                target_link_libraries(tgui PUBLIC SDL2::SDL2-static)
            else()
                # If no static version was found then fall back to a shared library
                message(STATUS "Using shared SDL2 lib because static target didn't exist")
                target_link_libraries(tgui PUBLIC SDL2::SDL2)
            endif()
        else() # Linking dynamically
            if(TGUI_SHARED_LIBS)
                # When possible, verify that the library really isn't a static library. The SDL2::SDL2 target may be a static library if SDL was only build statically.
                get_target_property(sdl_target_type SDL2::SDL2 TYPE)
                if (TGUI_USE_STATIC_SDL OR sdl_target_type STREQUAL "STATIC_LIBRARY")
                    # The user has to link SDL in his own program, which would conflict with the one already inside the TGUI dll.
                    message(FATAL_ERROR "Linking statically to SDL isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SDL library.")
                endif()
            endif()

            target_link_libraries(tgui PUBLIC SDL2::SDL2)
        endif()
    endif()
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


# Check if we can find SDL2_ttf with a config file (2.20.0 or newer), or whether we need to use our FindSDL2_ttf.cmake file
function(tgui_try_find_sdl2_ttf_config)
    find_package(SDL2_ttf CONFIG QUIET)
    if(SDL2_ttf_FOUND AND (TARGET SDL2_ttf::SDL2_ttf OR TARGET SDL2_ttf::SDL2_ttf-static))
        set(TGUI_FOUND_SDL2_TTF_CONFIG TRUE PARENT_SCOPE)
    else()
        set(TGUI_FOUND_SDL2_TTF_CONFIG FALSE PARENT_SCOPE)
    endif()
endfunction()

# Find SDL_ttf and add it as a dependency
macro(tgui_add_dependency_sdl_ttf)
    if(TGUI_USE_SDL3)
        if(NOT TARGET SDL3_ttf::SDL3_ttf)
            find_package(SDL3_ttf CONFIG)
            if(NOT SDL3_ttf_FOUND)
                message(FATAL_ERROR
                        "CMake couldn't find SDL3_ttf.\n"
                        "Set SDL3_ttf_DIR to the directory containing either SDL3_ttfConfig.cmake or sdl3_ttf-config.cmake\n"
                        "If you wish to use SDL2 instead of SDL3 then set TGUI_USE_SDL3 = FALSE.\n")
            endif()
        endif()

        # We don't need to keep options that were created for SDL2
        unset(SDL2_ttf_DIR CACHE)
        unset(SDL2_TTF_PATH CACHE)
        unset(SDL2_TTF_LIBRARY CACHE)
        unset(SDL2_TTF_INCLUDE_DIR CACHE)
        unset(SDL2_TTF_NO_DEFAULT_PATH CACHE)

        if(TGUI_USE_STATIC_SDL OR (NOT DEFINED TGUI_USE_STATIC_SDL AND NOT TGUI_SHARED_LIBS AND TARGET SDL3_ttf::SDL3_ttf-static))
            set(link_static_sdl_libs TRUE)
        else()
            set(link_static_sdl_libs FALSE)
        endif()

        if(link_static_sdl_libs)
            if(TGUI_SHARED_LIBS)
                # The user has to link SDL_ttf in his own program, which would conflict with the one already inside the TGUI dll.
                message(FATAL_ERROR "Linking statically to SDL_ttf isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SDL_ttf library by setting TGUI_USE_STATIC_SDL to FALSE.")
            endif()
            if(NOT TARGET SDL3_ttf::SDL3_ttf-static)
                message(FATAL_ERROR "Couldn't link to SDL3_ttf::SDL3_ttf-static, no such target exists")
            endif()
            target_link_libraries(tgui PUBLIC SDL3_ttf::SDL3_ttf-static)
        else()
            if(NOT TARGET SDL3_ttf::SDL3_ttf-shared)
                message(FATAL_ERROR "Couldn't link to SDL3_ttf::SDL3_ttf-shared, no such target exists")
            endif()
            target_link_libraries(tgui PUBLIC SDL3_ttf::SDL3_ttf-shared)
        endif()
    else() # Using SDL2
        if(NOT TARGET SDL2_ttf::SDL2_ttf AND NOT TARGET SDL2_ttf::SDL2_ttf-static)
            if(NOT TGUI_SKIP_SDL_CONFIG) # e.g. to skip macOS config file when building for iOS
                # First try looking for an SDL2_ttf config file
                tgui_try_find_sdl2_ttf_config()
            endif()
            if(TGUI_FOUND_SDL2_TTF_CONFIG)
                find_package(SDL2_ttf CONFIG REQUIRED)
            else()
                if(TGUI_OS_WINDOWS)
                    find_package(SDL2_ttf)
                    if(NOT SDL2_ttf_FOUND)
                        message(FATAL_ERROR
                                "CMake couldn't find SDL2_ttf.\n"
                                "For SDL2_ttf >= 2.20, set SDL2_ttf_DIR to the directory containing either SDL2_ttfConfig.cmake or sdl2_ttf-config.cmake.\n"
                                "For older versions, if you downloaded SDL2_ttf-devel-2.0.XX-VC.zip from github.com/libsdl-org/SDL_ttf/releases then set SDL2_TTF_PATH to the root directory.\n")
                    endif()
                elseif(TGUI_OS_MACOS)
                    find_package(SDL2_ttf)

                    if(NOT SDL2_ttf_FOUND)
                        # TODO: For SDL2_ttf >= 2.20.0 we should suggest setting SDL2_ttf_DIR instead of using our own SDL2_TTF_PATH.
                        #       The config file is located inside the framework, so it needs to be tested what path needs to be given to SDL2_ttf_DIR exactly.
                        message(FATAL_ERROR
                                "CMake couldn't find SDL2_ttf.\n"
                                "Set SDL2_TTF_PATH to the folder that contains the 'include' and 'lib' subdirectories, or to the folder that contains the SDL2_ttf.framework file.\n")
                    endif()
                else()
                    # On Linux we should always find SDL2_ttf automatically when it has been installed
                    find_package(SDL2_ttf REQUIRED)
                endif()
            endif()

            # Remove the empty SDL2_ttf_DIR variable if we found SDL_ttf via SDL2_TTF_PATH and vice versa
            if(NOT SDL2_ttf_DIR)
                unset(SDL2_ttf_DIR CACHE)
            endif()
            if(SDL2_ttf_DIR AND NOT SDL2_TTF_PATH AND NOT SDL2_TTF_LIBRARY)
                unset(SDL2_TTF_PATH CACHE)
                unset(SDL2_TTF_LIBRARY CACHE)
                unset(SDL2_TTF_INCLUDE_DIR CACHE)
                unset(SDL2_TTF_NO_DEFAULT_PATH CACHE)
            endif()
            unset(SDL3_ttf_DIR CACHE)

            if(DEFINED sdl2_ttf_VERSION)
                if (${sdl2_ttf_VERSION} VERSION_LESS "2.0.14")
                    message(FATAL_ERROR "SDL_ttf 2.0.14 or higher is required")
                endif()
            endif()
        endif()

        # Link to SDL_ttf and set include and library search directories. The dependency is PUBLIC because the user has to call TTF_Init and TTF_Quit.
        # TGUI_USE_STATIC_SDL_TTF allows explicitly chosing how SDL_ttf is linked. By default it is undefined and a static lib is preferred (but not required) when linking statically.
        if(TGUI_USE_STATIC_SDL_TTF AND NOT TARGET SDL2_ttf::SDL2_ttf-static)
            # If the user explicitly asks for a static target then it must exist
            message(FATAL_ERROR "Couldn't link to SDL2_ttf::SDL2_ttf-static, no such target exists")
        endif()
        if(TGUI_USE_STATIC_SDL_TTF OR (NOT DEFINED TGUI_USE_STATIC_SDL_TTF AND NOT TGUI_SHARED_LIBS))
            if(TGUI_SHARED_LIBS AND TGUI_USE_STATIC_SDL_TTF)
                # The user has to link SDL_ttf in his own program, which would conflict with the one already inside the TGUI dll.
                message(FATAL_ERROR "Linking statically to SDL_ttf isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SDL_ttf library by setting TGUI_USE_STATIC_SDL_TTF to FALSE.")
            endif()
            if(TARGET SDL2_ttf::SDL2_ttf-static)
                target_link_libraries(tgui PUBLIC SDL2_ttf::SDL2_ttf-static)
            else()
                # If no static version was found then fall back to a shared library
                message(STATUS "Using shared SDL2_ttf lib because static target didn't exist")
                target_link_libraries(tgui PUBLIC SDL2_ttf::SDL2_ttf)
            endif()
        else() # Linking dynamically
            if(TGUI_SHARED_LIBS)
                # When possible, verify that the library really isn't a static library. The SDL2_ttf::SDL2_ttf target may be a static library if SDL_ttf was only build statically.
                get_target_property(sdl_target_type SDL2_ttf::SDL2_ttf TYPE)
                if (TGUI_USE_STATIC_SDL_TTF OR sdl_target_type STREQUAL "STATIC_LIBRARY")
                    # The user has to link SDL_ttf in his own program, which would conflict with the one already inside the TGUI dll.
                    message(FATAL_ERROR "Linking statically to SDL_ttf isn't allowed when linking TGUI dynamically. Either set TGUI_SHARED_LIBS to FALSE to link TGUI statically or use a dynamic SDL_ttf library.")
                endif()
            endif()

            target_link_libraries(tgui PUBLIC SDL2_ttf::SDL2_ttf)
        endif()
    endif()
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
                    "If you don't have FreeType installed then you can download binaries from github.com/ubawurinna/freetype-windows-binaries/releases/ ('Source code (zip)' link under 'Assets') and set the FREETYPE_WINDOWS_BINARIES_PATH variable to the extracted freetype-windows-binaries folder.\n")
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
        target_link_libraries(tgui PRIVATE X11::X11)
    endif()
endmacro()
