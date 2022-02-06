####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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

# Macro that helps defining an option
macro(tgui_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# Macro to set a variable based on a boolean expression
# Usage: tgui_assign_bool(var cond1 AND cond2)
macro(tgui_assign_bool var)
    if(${ARGN})
        set(${var} ON)
    else()
        set(${var} OFF)
    endif()
endmacro()

# Set the compile options used by all targets
function(tgui_set_global_compile_flags target)
    if(TGUI_COMPILER_MSVC OR (TGUI_OS_WINDOWS AND TGUI_COMPILER_CLANG AND NOT MINGW))
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target}
                               PRIVATE
                               -Wall
                               -Wextra
                               -Wshadow
                               -Wnon-virtual-dtor
                               -Wold-style-cast
                               -Wcast-align
                               -Wunused
                               -Woverloaded-virtual
                               -Wpedantic
                               -Wdouble-promotion
                               -Wformat=2)
    endif()

    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_features(${target} PUBLIC cxx_std_${TGUI_CXX_STANDARD})

    if(TGUI_USE_MULTI_PROCESSOR_COMPILATION)
        target_compile_options(${target} PRIVATE /MP)
    endif()
endfunction()

# Set the appropriate standard library on each platform for the given target
function(tgui_set_stdlib target)
    # Use libc++ on macOS
    if(TGUI_OS_MACOS)
        if(${CMAKE_GENERATOR} MATCHES "Xcode")
            set_property(TARGET ${target} PROPERTY XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        else()
            target_compile_options(${target} PRIVATE "-stdlib=libc++")
            target_link_libraries(${target} PRIVATE "-stdlib=libc++")
        endif()
    endif()

    # Apply the TGUI_USE_STATIC_STD_LIBS option on windows when using GCC.
    if(TGUI_OS_WINDOWS AND TGUI_COMPILER_GCC)
        if(TGUI_USE_STATIC_STD_LIBS AND NOT TGUI_COMPILER_GCC_TDM)
            target_link_libraries(${target} PRIVATE "-static-libgcc" "-static-libstdc++")
        elseif(NOT TGUI_USE_STATIC_STD_LIBS AND TGUI_COMPILER_GCC_TDM)
            target_link_libraries(${target} PRIVATE "-shared-libgcc" "-shared-libstdc++")
        endif()
    endif()

    # Apply the TGUI_USE_STATIC_STD_LIBS option on windows when using Visual Studio.
    if((TGUI_COMPILER_MSVC OR (TGUI_OS_WINDOWS AND TGUI_COMPILER_CLANG AND NOT MINGW)))
        if(TGUI_USE_STATIC_STD_LIBS)
            set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        else()
            set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        endif()
    endif()
endfunction()

# Generate a TGUIConfig.cmake file (and associated files)
function(tgui_export_target export_name)
    include(CMakePackageConfigHelpers)
        write_basic_package_version_file("${PROJECT_BINARY_DIR}/TGUIConfigVersion.cmake"
                                         VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}
                                         COMPATIBILITY SameMinorVersion)

    if (TGUI_SHARED_LIBS)
        set(targets_config_filename TGUISharedTargets.cmake)
    else()
        set(targets_config_filename TGUIStaticTargets.cmake)
    endif()

    export(EXPORT ${export_name}
           FILE "${PROJECT_BINARY_DIR}/${targets_config_filename}")

    if (TGUI_BUILD_FRAMEWORK)
        set(config_package_location "TGUI.framework/Resources/CMake")
    else()
        set(config_package_location ${CMAKE_INSTALL_LIBDIR}/cmake/TGUI)
    endif()

    configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/TGUIConfig.cmake.in" "${PROJECT_BINARY_DIR}/TGUIConfig.cmake"
        INSTALL_DESTINATION "${config_package_location}")

    install(EXPORT ${export_name}
            FILE ${targets_config_filename}
            DESTINATION ${config_package_location})

    install(FILES "${PROJECT_BINARY_DIR}/TGUIConfig.cmake"
                  "${PROJECT_BINARY_DIR}/TGUIConfigVersion.cmake"
            DESTINATION ${config_package_location}
            COMPONENT devel)

    # Install the find modules when they are needed to find our dependencies
    if(TGUI_HAS_WINDOW_BACKEND_GLFW AND NOT TGUI_FOUND_GLFW_CONFIG)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/Findglfw3.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/Findglfw3.cmake" "${PROJECT_BINARY_DIR}/")
    endif()
    if((TGUI_HAS_WINDOW_BACKEND_SDL OR TGUI_HAS_FONT_BACKEND_SDL_TTF) AND NOT TGUI_FOUND_SDL2_CONFIG)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2.cmake" "${PROJECT_BINARY_DIR}/")
    endif()
    if(TGUI_HAS_FONT_BACKEND_SDL_TTF AND NOT TGUI_FOUND_SDL2_TTF_CONFIG)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2_ttf.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2_ttf.cmake" "${PROJECT_BINARY_DIR}/")
    endif()
endfunction()


# Install the dlls next to the executables (both immediately after building and when installing them somewhere)
function(copy_dlls_to_exe post_build_destination install_destination target)
    if(TGUI_OS_WINDOWS)
        set(files_to_copy "")

        # Copy the TGUI dll if we built one
        if(TGUI_SHARED_LIBS)
            list(APPEND files_to_copy "$<TARGET_FILE:tgui>")
        endif()

        # Copy the FreeType dll if needed and when we know where it is
        if(TGUI_HAS_FONT_BACKEND_FREETYPE AND FREETYPE_WINDOWS_BINARIES_PATH AND NOT TGUI_USE_STATIC_STD_LIBS)
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(freetype_dll "${FREETYPE_WINDOWS_BINARIES_PATH}/release dll/win64/freetype.dll")
            else()
                set(freetype_dll "${FREETYPE_WINDOWS_BINARIES_PATH}/release dll/win32/freetype.dll")
            endif()
            if(EXISTS "${freetype_dll}")
                list(APPEND files_to_copy "${freetype_dll}")
            else() # Look for file structure that was used prior to FreeType 2.11
                if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                    set(freetype_dll "${FREETYPE_WINDOWS_BINARIES_PATH}/win64/freetype.dll")
                else()
                    set(freetype_dll "${FREETYPE_WINDOWS_BINARIES_PATH}/win32/freetype.dll")
                endif()

                if(EXISTS "${freetype_dll}")
                    list(APPEND files_to_copy "${freetype_dll}")
                endif()
            endif()
        endif()

        # TODO: SFML, SDL, SDL_ttf and GLFW

        # Previously we were just listing the files to copy, now we will actually give the commands for the copying.
        # We are merely setting triggers here, the actual copying only happens after building and when installing.
        foreach(file_to_copy ${files_to_copy})
            add_custom_command(TARGET ${target} POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E copy "${file_to_copy}" "${post_build_destination}")

            install(FILES "${file_to_copy}"
                    DESTINATION "${install_destination}"
                    COMPONENT ${target})
        endforeach()
    endif()
endfunction()
