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

# Macro that helps defining an option.
# This code has a subtle difference compared to just calling "set(${var} ${default} CACHE ${type} ${docstring})":
#   If a normal (non-cache) variable already exists (e.g. for overwriting settings with TGUI in subdirectory),
#   then the cache variable is initialized with the value of the normal variable instead of the default value.
#   When re-running, the cache variable will always be reset to the explicitly set normal value. This is probably
#   better than keep showing the wrong value in the user interface and silently working with another value.
macro(tgui_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default} CACHE ${type} ${docstring} FORCE)
    else()
        set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
    endif()
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
    if(TGUI_COMPILER_MSVC OR TGUI_COMPILER_CLANG_CL)
        target_compile_options(${target} PRIVATE
                               $<$<BOOL:${TGUI_WARNINGS_AS_ERRORS}>:/WX>
                               /W4
                               /permissive-
        )
    else()
        target_compile_options(${target} PRIVATE
                               $<$<BOOL:${TGUI_WARNINGS_AS_ERRORS}>:-Werror>
                               -Wall
                               -Wextra
                               -Wshadow
                               -Wsign-conversion
                               -Wfloat-conversion
                               -Wnon-virtual-dtor
                               -Wold-style-cast
                               -Wcast-align
                               -Wunused
                               -Woverloaded-virtual
                               -Wpedantic
                               -Wdouble-promotion
                               -Wformat=2
                               -Wimplicit-fallthrough
                               -Wnull-dereference
        )

        if(TGUI_COMPILER_GCC)
            target_compile_options(${target} PRIVATE
                                   -Wmisleading-indentation
                                   -Wduplicated-cond
                                   -Wlogical-op
            )
        endif()

        if(TGUI_COMPILER_GCC OR (TGUI_COMPILER_LLVM_CLANG AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11))
            target_compile_options(${target} PRIVATE -Wsuggest-override)
        endif()
    endif()

    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_features(${target} PUBLIC cxx_std_${TGUI_CXX_STANDARD})

    if(TGUI_USE_MULTI_PROCESSOR_COMPILATION)
        target_compile_options(${target} PRIVATE /MP)
    endif()
endfunction()

# Set the appropriate standard library on each platform for the given target
function(tgui_set_stdlib target)
    # Apply the TGUI_USE_STATIC_STD_LIBS option on windows when using GCC.
    if(TGUI_OS_WINDOWS AND TGUI_COMPILER_GCC)
        if(TGUI_USE_STATIC_STD_LIBS AND NOT TGUI_COMPILER_GCC_TDM)
            target_link_libraries(${target} PRIVATE "-static-libgcc" "-static-libstdc++")
        elseif(NOT TGUI_USE_STATIC_STD_LIBS AND TGUI_COMPILER_GCC_TDM)
            target_link_libraries(${target} PRIVATE "-shared-libgcc" "-shared-libstdc++")
        endif()
    endif()

    # Apply the TGUI_USE_STATIC_STD_LIBS option on windows when using Visual Studio.
    if(TGUI_COMPILER_MSVC OR TGUI_COMPILER_CLANG_CL)
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
                                         COMPATIBILITY SameMajorVersion)

    if (TGUI_SHARED_LIBS)
        set(targets_config_filename TGUISharedTargets.cmake)
    else()
        set(targets_config_filename TGUIStaticTargets.cmake)
    endif()

    export(EXPORT ${export_name}
           NAMESPACE TGUI::
           FILE "${PROJECT_BINARY_DIR}/${targets_config_filename}")

    if (TGUI_BUILD_FRAMEWORK)
        set(config_package_location "TGUI.framework/Resources/CMake")
    else()
        set(config_package_location ${CMAKE_INSTALL_LIBDIR}/cmake/TGUI)
    endif()

    configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/TGUIConfig.cmake.in" "${PROJECT_BINARY_DIR}/TGUIConfig.cmake"
        INSTALL_DESTINATION "${config_package_location}")

    install(EXPORT ${export_name}
            NAMESPACE TGUI::
            FILE ${targets_config_filename}
            DESTINATION ${config_package_location})

    install(FILES "${PROJECT_BINARY_DIR}/TGUIConfig.cmake"
                  "${PROJECT_BINARY_DIR}/TGUIConfigVersion.cmake"
            DESTINATION ${config_package_location}
            COMPONENT devel)

    # Install the find modules when they are needed to find our dependencies
    if(TGUI_HAS_WINDOW_BACKEND_GLFW AND NOT TGUI_FOUND_GLFW_CONFIG)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/Findglfw3.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/Findglfw3.cmake" "${PROJECT_BINARY_DIR}/" VERBATIM)
    endif()
    if((TGUI_HAS_WINDOW_BACKEND_SDL OR TGUI_HAS_FONT_BACKEND_SDL_TTF) AND NOT TGUI_FOUND_SDL2_CONFIG)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2.cmake" "${PROJECT_BINARY_DIR}/" VERBATIM)
    endif()
    if(TGUI_HAS_FONT_BACKEND_SDL_TTF AND NOT TGUI_FOUND_SDL2_TTF_CONFIG)
        install(FILES "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2_ttf.cmake" DESTINATION ${config_package_location} COMPONENT devel)
        add_custom_command(TARGET tgui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/cmake/Modules/FindSDL2_ttf.cmake" "${PROJECT_BINARY_DIR}/" VERBATIM)
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
        # We are merely setting triggers here, the actual copying only happens after building or when installing.
        foreach(file_to_copy ${files_to_copy})
            add_custom_command(TARGET ${target} POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E copy "${file_to_copy}" "${post_build_destination}"
                               VERBATIM)

            install(FILES "${file_to_copy}"
                    DESTINATION "${install_destination}"
                    COMPONENT ${target})
        endforeach()
    endif()
endfunction()

# Bundles a set of source files into a c++20 module
function(tgui_create_module_from_sources source_file_list module_name)
    file(READ "TGUI-Module.cppm.in" file_contents)
    string(APPEND file_contents "\nexport module ${module_name};\n")
    string(APPEND file_contents "\nexport import tgui;\n")
    if(${ARGC} GREATER 2)
        foreach(extra_module_to_export ${ARGN})
            string(APPEND file_contents "export import ${extra_module_to_export};\n")
        endforeach()
    endif()
    string(APPEND file_contents "\n")
    foreach(source_file ${source_file_list})
        string(APPEND file_contents "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${source_file}\"\n")
    endforeach()
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/module_${module_name}.cppm" "${file_contents}")
    set(module_source "${CMAKE_CURRENT_BINARY_DIR}/module_${module_name}.cppm" PARENT_SCOPE)
endfunction()
