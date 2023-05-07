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

if(TGUI_BACKEND_UPPERCASE STREQUAL "CUSTOM")
    if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_MACOS OR TGUI_OS_ANDROID OR TGUI_OS_IOS)
        tgui_set_option(TGUI_HAS_BACKEND_SFML_GRAPHICS FALSE BOOL "TRUE to build the SFML_GRAPHICS backend (uses sfml-graphics for everything)")
        tgui_set_option(TGUI_HAS_BACKEND_SDL_RENDERER FALSE BOOL "TRUE to build the SDL_RENDERER backend (SDL2 + SDL2_ttf)")
    endif()
    if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_ANDROID OR TGUI_OS_IOS)
        tgui_set_option(TGUI_HAS_BACKEND_SDL_GLES2 FALSE BOOL "TRUE to build the SDL_GLES2 backend (SDL2 + OpenGL ES + FreeType)")
        tgui_set_option(TGUI_HAS_BACKEND_SDL_TTF_GLES2 FALSE BOOL "TRUE to build the SDL_TTF_GLES2 backend (SDL2 + SDL2_ttf + OpenGL ES)")
        tgui_set_option(TGUI_HAS_BACKEND_GLFW_GLES2 FALSE BOOL "TRUE to build the GLFW_GLES2 backend (GLFW + OpenGL ES + FreeType)")
    endif()
    if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_MACOS)
        tgui_set_option(TGUI_HAS_BACKEND_SFML_OPENGL3 FALSE BOOL "TRUE to build the SFML_OPENGL3 backend (sfml-window + OpenGL + FreeType)")
        tgui_set_option(TGUI_HAS_BACKEND_SDL_OPENGL3 FALSE BOOL "TRUE to build the SDL_OPENGL3 backend (SDL2 + OpenGL + FreeType)")
        tgui_set_option(TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 FALSE BOOL "TRUE to build the SDL_TTF_OPENGL3 backend (SDL2 + SDL2_ttf + OpenGL)")
        tgui_set_option(TGUI_HAS_BACKEND_GLFW_OPENGL3 FALSE BOOL "TRUE to build the GLFW_OPENGL3 backend (GLFW + OpenGL + FreeType)")
    endif()

    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_WINDOW_SFML FALSE BOOL "TRUE to build the SFML window backend even when not needed by any selected backend")
    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_WINDOW_SDL FALSE BOOL "TRUE to build the SDL window backend even when not needed by any selected backend")
    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_WINDOW_GLFW FALSE BOOL "TRUE to build the GLFW window backend even when not needed by any selected backend")

    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_RENDERER_SFML_GRAPHICS FALSE BOOL "TRUE to build the sfml-graphics renderer even when not needed by any selected backend")
    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_RENDERER_SDL_RENDERER FALSE BOOL "TRUE to build the SDL_Renderer renderer even when not needed by any selected backend")
    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_RENDERER_OPENGL3 FALSE BOOL "TRUE to build the modern OpenGL renderer even when not needed by any selected backend")
    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_RENDERER_GLES2 FALSE BOOL "TRUE to build the OpenGL ES renderer even when not needed by any selected backend")

    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_FONT_SFML_GRAPHICS FALSE BOOL "TRUE to build the sf::Font font loader even when not needed by any selected backend")
    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_FONT_SDL_TTF FALSE BOOL "TRUE to build the SDL_ttf font loader even when not needed by any selected backend")
    tgui_set_option(TGUI_CUSTOM_BACKEND_HAS_FONT_FREETYPE FALSE BOOL "TRUE to build the FreeType font loader even when not needed by any selected backend")

    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_WINDOW_SFML)
    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_WINDOW_SDL)
    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_WINDOW_GLFW)

    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_RENDERER_SFML_GRAPHICS)
    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_RENDERER_SDL_RENDERER)
    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_RENDERER_OPENGL3)
    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_RENDERER_GLES2)

    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_FONT_SFML_GRAPHICS)
    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_FONT_SDL_TTF)
    mark_as_advanced(TGUI_CUSTOM_BACKEND_HAS_FONT_FREETYPE)
else() # TGUI_BACKEND != Custom
    unset(TGUI_HAS_BACKEND_SFML_GRAPHICS CACHE)
    unset(TGUI_HAS_BACKEND_SFML_OPENGL3 CACHE)
    unset(TGUI_HAS_BACKEND_SDL_RENDERER CACHE)
    unset(TGUI_HAS_BACKEND_SDL_OPENGL3 CACHE)
    unset(TGUI_HAS_BACKEND_SDL_GLES2 CACHE)
    unset(TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 CACHE)
    unset(TGUI_HAS_BACKEND_SDL_TTF_GLES2 CACHE)
    unset(TGUI_HAS_BACKEND_GLFW_OPENGL3 CACHE)
    unset(TGUI_HAS_BACKEND_GLFW_GLES2 CACHE)

    unset(TGUI_CUSTOM_BACKEND_HAS_WINDOW_SFML CACHE)
    unset(TGUI_CUSTOM_BACKEND_HAS_WINDOW_SDL CACHE)
    unset(TGUI_CUSTOM_BACKEND_HAS_WINDOW_GLFW CACHE)

    unset(TGUI_CUSTOM_BACKEND_HAS_RENDERER_SFML_GRAPHICS CACHE)
    unset(TGUI_CUSTOM_BACKEND_HAS_RENDERER_SDL_RENDERER CACHE)
    unset(TGUI_CUSTOM_BACKEND_HAS_RENDERER_OPENGL3 CACHE)
    unset(TGUI_CUSTOM_BACKEND_HAS_RENDERER_GLES2 CACHE)

    unset(TGUI_CUSTOM_BACKEND_HAS_FONT_SFML_GRAPHICS CACHE)
    unset(TGUI_CUSTOM_BACKEND_HAS_FONT_SDL_TTF CACHE)
    unset(TGUI_CUSTOM_BACKEND_HAS_FONT_FREETYPE CACHE)

    if(NOT TGUI_BACKEND IN_LIST TGUI_BACKEND_OPTIONS)
        message(FATAL_ERROR "TGUI_BACKEND was set to an unknown backend")
    endif()

    if(TGUI_BACKEND STREQUAL "SFML_GRAPHICS")
        set(TGUI_HAS_BACKEND_SFML_GRAPHICS TRUE)
    elseif(TGUI_BACKEND STREQUAL "SFML_OPENGL3")
        set(TGUI_HAS_BACKEND_SFML_OPENGL3 TRUE)
    elseif(TGUI_BACKEND STREQUAL "SDL_RENDERER")
        set(TGUI_HAS_BACKEND_SDL_RENDERER TRUE)
    elseif(TGUI_BACKEND STREQUAL "SDL_OPENGL3")
        set(TGUI_HAS_BACKEND_SDL_OPENGL3 TRUE)
    elseif(TGUI_BACKEND STREQUAL "SDL_GLES2")
        set(TGUI_HAS_BACKEND_SDL_GLES2 TRUE)
    elseif(TGUI_BACKEND STREQUAL "SDL_TTF_OPENGL3")
        set(TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 TRUE)
    elseif(TGUI_BACKEND STREQUAL "SDL_TTF_GLES2")
        set(TGUI_HAS_BACKEND_SDL_TTF_GLES2 TRUE)
    elseif(TGUI_BACKEND STREQUAL "GLFW_OPENGL3")
        set(TGUI_HAS_BACKEND_GLFW_OPENGL3 TRUE)
    elseif(TGUI_BACKEND STREQUAL "GLFW_GLES2")
        set(TGUI_HAS_BACKEND_GLFW_GLES2 TRUE)
    else()
        message(FATAL_ERROR "FIXME: Backend appeared in TGUI_BACKEND_OPTIONS but isn't being handled here!")
    endif()
endif()

# Figure out which backend components are needed
tgui_assign_bool(TGUI_HAS_WINDOW_BACKEND_SFML
    TGUI_HAS_BACKEND_SFML_GRAPHICS OR TGUI_HAS_BACKEND_SFML_OPENGL3 OR TGUI_CUSTOM_BACKEND_HAS_WINDOW_SFML)

tgui_assign_bool(TGUI_HAS_WINDOW_BACKEND_SDL
    TGUI_HAS_BACKEND_SDL_RENDERER OR TGUI_HAS_BACKEND_SDL_OPENGL3 OR TGUI_HAS_BACKEND_SDL_GLES2
    OR TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 OR TGUI_HAS_BACKEND_SDL_TTF_GLES2
    OR TGUI_CUSTOM_BACKEND_HAS_WINDOW_SDL OR TGUI_CUSTOM_BACKEND_HAS_RENDERER_SDL_RENDERER)

tgui_assign_bool(TGUI_HAS_WINDOW_BACKEND_GLFW
    TGUI_HAS_BACKEND_GLFW_OPENGL3 OR TGUI_HAS_BACKEND_GLFW_GLES2 OR TGUI_CUSTOM_BACKEND_HAS_WINDOW_GLFW)

tgui_assign_bool(TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
    TGUI_HAS_BACKEND_SFML_GRAPHICS OR TGUI_CUSTOM_BACKEND_HAS_RENDERER_SFML_GRAPHICS)

tgui_assign_bool(TGUI_HAS_RENDERER_BACKEND_SDL_RENDERER
    TGUI_HAS_BACKEND_SDL_RENDERER OR TGUI_CUSTOM_BACKEND_HAS_RENDERER_SDL_RENDERER)

tgui_assign_bool(TGUI_HAS_RENDERER_BACKEND_OPENGL3
    TGUI_HAS_BACKEND_SFML_OPENGL3 OR TGUI_HAS_BACKEND_SDL_OPENGL3 OR TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
    OR TGUI_HAS_BACKEND_GLFW_OPENGL3 OR TGUI_CUSTOM_BACKEND_HAS_RENDERER_OPENGL3)

tgui_assign_bool(TGUI_HAS_RENDERER_BACKEND_GLES2
    TGUI_HAS_BACKEND_SDL_GLES2 OR TGUI_HAS_BACKEND_SDL_TTF_GLES2 OR TGUI_HAS_BACKEND_GLFW_GLES2 OR TGUI_CUSTOM_BACKEND_HAS_RENDERER_GLES2)

tgui_assign_bool(TGUI_HAS_FONT_BACKEND_SFML_GRAPHICS
    TGUI_HAS_BACKEND_SFML_GRAPHICS OR TGUI_CUSTOM_BACKEND_HAS_FONT_SFML_GRAPHICS)

tgui_assign_bool(TGUI_HAS_FONT_BACKEND_SDL_TTF
    TGUI_HAS_BACKEND_SDL_RENDERER OR TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 OR TGUI_HAS_BACKEND_SDL_TTF_GLES2 OR TGUI_CUSTOM_BACKEND_HAS_FONT_SDL_TTF)

tgui_assign_bool(TGUI_HAS_FONT_BACKEND_FREETYPE
    TGUI_HAS_BACKEND_SFML_OPENGL3 OR TGUI_HAS_BACKEND_SDL_OPENGL3 OR TGUI_HAS_BACKEND_SDL_GLES2
    OR TGUI_HAS_BACKEND_GLFW_OPENGL3 OR TGUI_HAS_BACKEND_GLFW_GLES2 OR TGUI_CUSTOM_BACKEND_HAS_FONT_FREETYPE)

# Find and add dependencies
if(TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS OR TGUI_HAS_FONT_BACKEND_SFML_GRAPHICS)
    tgui_add_dependency_sfml(Graphics)
elseif(TGUI_HAS_WINDOW_BACKEND_SFML)
    tgui_add_dependency_sfml(Window)
endif()
if(TGUI_HAS_WINDOW_BACKEND_SDL OR TGUI_HAS_FONT_BACKEND_SDL_TTF)
    tgui_add_dependency_sdl()
endif()
if(TGUI_HAS_WINDOW_BACKEND_GLFW)
    tgui_add_dependency_glfw()
endif()
if(TGUI_HAS_FONT_BACKEND_SDL_TTF)
    tgui_add_dependency_sdl_ttf()
endif()
if(TGUI_HAS_RENDERER_BACKEND_OPENGL3)
    tgui_add_dependency_opengl()
endif()
if(TGUI_HAS_RENDERER_BACKEND_GLES2)
    tgui_add_dependency_gles2()
endif()
if(TGUI_HAS_FONT_BACKEND_FREETYPE)
    tgui_add_dependency_freetype()

    if(FREETYPE_WINDOWS_BINARIES_PATH AND (TGUI_HAS_FONT_BACKEND_SDL_TTF OR TGUI_HAS_FONT_BACKEND_SFML_GRAPHICS OR TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS))
        message(WARNING "Warning: FREETYPE_WINDOWS_BINARIES_PATH was set to find FreeType while also linking to SDL_ttf or sfml-graphics. "
            "Only one FreeType version is allowed to exist, so double check to make certain that the library pointed to with FREETYPE_WINDOWS_BINARIES_PATH is also the one that was used to build SDL_ttf or sfml-graphics. "
            "If the library is the same then you can ignore this warning.")
    endif()
endif()
if(TGUI_OS_LINUX OR TGUI_OS_BSD)
    # We need X11 to display diagonal arrow mouse cursors if we use a window from SFML < 2.6 or SDL
    if((TGUI_HAS_WINDOW_BACKEND_SFML AND SFML_VERSION VERSION_LESS "2.6") OR TGUI_HAS_WINDOW_BACKEND_SDL)
        tgui_add_dependency_x11()
    endif()
endif()

# Build the required backends
if(TGUI_HAS_WINDOW_BACKEND_SFML)
    set(new_tgui_backend_sources
        Backend/Window/SFML/BackendGuiSFML.cpp
        Backend/Window/SFML/BackendSFML.cpp
    )
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.window.sfml")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
        set(new_tgui_backend_sources "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
    set_source_files_properties(${new_tgui_backend_sources} PROPERTIES UNITY_GROUP "WINDOW_BACKEND_SFML")
endif()
if(TGUI_HAS_WINDOW_BACKEND_SDL)
    set(new_tgui_backend_sources
        Backend/Window/SDL/BackendGuiSDL.cpp
        Backend/Window/SDL/BackendSDL.cpp
    )
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.window.sdl")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
        set(new_tgui_backend_sources "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
    set_source_files_properties(${new_tgui_backend_sources} PROPERTIES UNITY_GROUP "WINDOW_BACKEND_SDL")
endif()
if(TGUI_HAS_WINDOW_BACKEND_GLFW)
    set(new_tgui_backend_sources
        Backend/Window/GLFW/BackendGuiGLFW.cpp
        Backend/Window/GLFW/BackendGLFW.cpp
    )
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.window.glfw")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
        set(new_tgui_backend_sources "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
    set_source_files_properties(${new_tgui_backend_sources} PROPERTIES UNITY_GROUP "WINDOW_BACKEND_GLFW")
endif()

if(TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS)
    set(new_tgui_backend_sources
        Backend/Renderer/SFML-Graphics/BackendRendererSFML.cpp
        Backend/Renderer/SFML-Graphics/BackendRenderTargetSFML.cpp
        Backend/Renderer/SFML-Graphics/BackendTextureSFML.cpp
        Backend/Renderer/SFML-Graphics/CanvasSFML.cpp
    )
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.renderer.sfml_graphics")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
        set(new_tgui_backend_sources "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
    set_source_files_properties(${new_tgui_backend_sources} PROPERTIES UNITY_GROUP "RENDERER_BACKEND_SFML_GRAPHICS")
endif()
if(TGUI_HAS_RENDERER_BACKEND_SDL_RENDERER)
    set(new_tgui_backend_sources
        Backend/Renderer/SDL_Renderer/BackendRendererSDL.cpp
        Backend/Renderer/SDL_Renderer/BackendRenderTargetSDL.cpp
        Backend/Renderer/SDL_Renderer/BackendTextureSDL.cpp
        Backend/Renderer/SDL_Renderer/CanvasSDL.cpp
    )
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.renderer.sdl_renderer")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
        set(new_tgui_backend_sources "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
    set_source_files_properties(${new_tgui_backend_sources} PROPERTIES UNITY_GROUP "RENDERER_BACKEND_SDL_RENDERER")
endif()
if(TGUI_HAS_RENDERER_BACKEND_OPENGL3)
    set(new_tgui_backend_sources
        Backend/Renderer/OpenGL3/BackendRendererOpenGL3.cpp
        Backend/Renderer/OpenGL3/BackendRenderTargetOpenGL3.cpp
        Backend/Renderer/OpenGL3/BackendTextureOpenGL3.cpp
        Backend/Renderer/OpenGL3/CanvasOpenGL3.cpp
    )
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.renderer.opengl3")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
        set(new_tgui_backend_sources "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
    set_source_files_properties(${new_tgui_backend_sources} PROPERTIES UNITY_GROUP "RENDERER_BACKEND_OPENGL3")
endif()
if(TGUI_HAS_RENDERER_BACKEND_GLES2)
    set(new_tgui_backend_sources
            Backend/Renderer/GLES2/BackendRendererGLES2.cpp
            Backend/Renderer/GLES2/BackendRenderTargetGLES2.cpp
            Backend/Renderer/GLES2/BackendTextureGLES2.cpp
            Backend/Renderer/GLES2/CanvasGLES2.cpp
    )
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.renderer.gles2")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
        set(new_tgui_backend_sources "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
    set_source_files_properties(${new_tgui_backend_sources} PROPERTIES UNITY_GROUP "RENDERER_BACKEND_GLES2")
endif()

if(TGUI_HAS_RENDERER_BACKEND_OPENGL3 OR TGUI_HAS_RENDERER_BACKEND_GLES2)
    if (TGUI_BUILD_AS_CXX_MODULE)
        set(new_tgui_backend_sources Backend/Renderer/OpenGL.cppm)
        target_sources(tgui
                       PRIVATE
                       FILE_SET tgui_cxx_module_files
                       TYPE CXX_MODULES
                       BASE_DIRS "Backend/Renderer/"
                       FILES "${new_tgui_backend_sources}")
    else()
        set(new_tgui_backend_sources Backend/Renderer/OpenGL.cpp)
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
    set_source_files_properties(${new_tgui_backend_sources} PROPERTIES UNITY_GROUP "OPENGL")
endif()

if(TGUI_HAS_FONT_BACKEND_SFML_GRAPHICS)
    set(new_tgui_backend_sources Backend/Font/SFML-Graphics/BackendFontSFML.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.font.sfml_graphics")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
endif()
if(TGUI_HAS_FONT_BACKEND_SDL_TTF)
    set(new_tgui_backend_sources Backend/Font/SDL_ttf/BackendFontSDLttf.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.font.sdl_ttf")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
endif()
if(TGUI_HAS_FONT_BACKEND_FREETYPE)
    set(new_tgui_backend_sources Backend/Font/FreeType/BackendFontFreeType.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${new_tgui_backend_sources}" "tgui.backend.font.freetype")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${new_tgui_backend_sources})
    endif()
endif()

if(TGUI_HAS_BACKEND_SFML_GRAPHICS)
    message(STATUS "Activating backend SFML_GRAPHICS (TGUI/Backend/SFML-Graphics.hpp)")
    set(backend_source Backend/SFML-Graphics.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.sfml_graphics" "tgui.backend.window.sfml;tgui.backend.font.sfml_graphics;tgui.backend.renderer.sfml_graphics")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()
if(TGUI_HAS_BACKEND_SFML_OPENGL3)
    message(STATUS "Activating backend SFML_OPENGL3 (TGUI/Backend/SFML-OpenGL3.hpp)")
    set(backend_source Backend/SFML-OpenGL3.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.sfml_opengl3" "tgui.backend.window.sfml;tgui.backend.font.freetype;tgui.backend.renderer.opengl3")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()
if(TGUI_HAS_BACKEND_SDL_RENDERER)
    message(STATUS "Activating backend SDL_RENDERER (TGUI/Backend/SDL-Renderer.hpp)")
    set(backend_source Backend/SDL-Renderer.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.sdl_renderer" "tgui.backend.window.sdl;tgui.backend.font.sdl_ttf;tgui.backend.renderer.sdl_renderer")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()
if(TGUI_HAS_BACKEND_SDL_OPENGL3)
    message(STATUS "Activating backend SDL_OPENGL3 (TGUI/Backend/SDL-OpenGL3.hpp)")
    set(backend_source Backend/SDL-OpenGL3.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.sdl_opengl3" "tgui.backend.window.sdl;tgui.backend.font.freetype;tgui.backend.renderer.opengl3")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()
if(TGUI_HAS_BACKEND_SDL_GLES2)
    message(STATUS "Activating backend SDL_GLES2 (TGUI/Backend/SDL-GLES2.hpp)")
    set(backend_source Backend/SDL-GLES2.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.sdl_gles2" "tgui.backend.window.sdl;tgui.backend.font.freetype;tgui.backend.renderer.gles2")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()
if(TGUI_HAS_BACKEND_SDL_TTF_OPENGL3)
    message(STATUS "Activating backend SDL_TTF_OPENGL3 (TGUI/Backend/SDL-TTF-OpenGL3.hpp)")
    set(backend_source Backend/SDL-TTF-OpenGL3.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.sdl_ttf_opengl3" "tgui.backend.window.sdl;tgui.backend.font.sdl_ttf;tgui.backend.renderer.opengl3")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()
if(TGUI_HAS_BACKEND_SDL_TTF_GLES2)
    message(STATUS "Activating backend SDL_TTF_GLES2 (TGUI/Backend/SDL-TTF-GLES2.hpp)")
    set(backend_source Backend/SDL-TTF-GLES2.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.sdl_ttf_gles2" "tgui.backend.window.sdl;tgui.backend.font.sdl_ttf;tgui.backend.renderer.gles2")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()
if(TGUI_HAS_BACKEND_GLFW_OPENGL3)
    message(STATUS "Activating backend GLFW_OPENGL3 (TGUI/Backend/GLFW-OpenGL3.hpp)")
    set(backend_source Backend/GLFW-OpenGL3.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.glfw_opengl3" "tgui.backend.window.glfw;tgui.backend.font.freetype;tgui.backend.renderer.opengl3")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()
if(TGUI_HAS_BACKEND_GLFW_GLES2)
    message(STATUS "Activating backend GLFW_GLES2 (TGUI/Backend/GLFW-GLES2.hpp)")
    set(backend_source Backend/GLFW-GLES2.cpp)
    if (TGUI_BUILD_AS_CXX_MODULE)
        tgui_create_module_from_sources("${backend_source}" "tgui.backend.glfw_gles2" "tgui.backend.window.glfw;tgui.backend.font.freetype;tgui.backend.renderer.gles2")
        target_sources(tgui PRIVATE FILE_SET tgui_cxx_module_files TYPE CXX_MODULES FILES "${module_source}")
    else()
        target_sources(tgui PRIVATE ${backend_source})
    endif()
endif()

# Select the default backend that is available for the Gui Builder, examples and tests.
# This order is fixed and has to match with e.g. DefaultBackendWindow.
if(TGUI_HAS_BACKEND_SFML_GRAPHICS)
    set(TGUI_DEFAULT_BACKEND "SFML_GRAPHICS" PARENT_SCOPE)
elseif(TGUI_HAS_BACKEND_SFML_OPENGL3)
    set(TGUI_DEFAULT_BACKEND "SFML_OPENGL3" PARENT_SCOPE)
elseif(TGUI_HAS_BACKEND_SDL_RENDERER)
    set(TGUI_DEFAULT_BACKEND "SDL_RENDERER" PARENT_SCOPE)
elseif(TGUI_HAS_BACKEND_SDL_TTF_OPENGL3)
    set(TGUI_DEFAULT_BACKEND "SDL_TTF_OPENGL3" PARENT_SCOPE)
elseif(TGUI_HAS_BACKEND_SDL_TTF_GLES2)
    set(TGUI_DEFAULT_BACKEND "SDL_TTF_GLES2" PARENT_SCOPE)
elseif(TGUI_HAS_BACKEND_SDL_OPENGL3)
    set(TGUI_DEFAULT_BACKEND "SDL_OPENGL3" PARENT_SCOPE)
elseif(TGUI_HAS_BACKEND_SDL_GLES2)
    set(TGUI_DEFAULT_BACKEND "SDL_GLES2" PARENT_SCOPE)
elseif(TGUI_HAS_BACKEND_GLFW_OPENGL3)
    set(TGUI_DEFAULT_BACKEND "GLFW_OPENGL3" PARENT_SCOPE)
elseif(TGUI_HAS_BACKEND_GLFW_GLES2)
    set(TGUI_DEFAULT_BACKEND "GLFW_GLES2" PARENT_SCOPE)
endif()

# Also pass backend components to parent scope so that the Gui Builder, examples and tests can check them.
# This is e.g. needed to copy the right dll files next to the executables
set(TGUI_HAS_WINDOW_BACKEND_SFML ${TGUI_HAS_WINDOW_BACKEND_SFML} PARENT_SCOPE)
set(TGUI_HAS_WINDOW_BACKEND_SDL ${TGUI_HAS_WINDOW_BACKEND_SDL} PARENT_SCOPE)
set(TGUI_HAS_WINDOW_BACKEND_GLFW ${TGUI_HAS_WINDOW_BACKEND_GLFW} PARENT_SCOPE)
set(TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS ${TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS} PARENT_SCOPE)
set(TGUI_HAS_RENDERER_BACKEND_SDL_RENDERER ${TGUI_HAS_RENDERER_BACKEND_SDL_RENDERER} PARENT_SCOPE)
set(TGUI_HAS_RENDERER_BACKEND_OPENGL3 ${TGUI_HAS_RENDERER_BACKEND_OPENGL3} PARENT_SCOPE)
set(TGUI_HAS_RENDERER_BACKEND_GLES2 ${TGUI_HAS_RENDERER_BACKEND_GLES2} PARENT_SCOPE)
set(TGUI_HAS_FONT_BACKEND_SFML_GRAPHICS ${TGUI_HAS_FONT_BACKEND_SFML_GRAPHICS} PARENT_SCOPE)
set(TGUI_HAS_FONT_BACKEND_SDL_TTF ${TGUI_HAS_FONT_BACKEND_SDL_TTF} PARENT_SCOPE)
set(TGUI_HAS_FONT_BACKEND_FREETYPE ${TGUI_HAS_FONT_BACKEND_FREETYPE} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_SFML_GRAPHICS ${TGUI_HAS_BACKEND_SFML_GRAPHICS} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_SFML_OPENGL3 ${TGUI_HAS_BACKEND_SFML_OPENGL3} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_SDL_RENDERER ${TGUI_HAS_BACKEND_SDL_RENDERER} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_SDL_OPENGL3 ${TGUI_HAS_BACKEND_SDL_OPENGL3} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_SDL_GLES2 ${TGUI_HAS_BACKEND_SDL_GLES2} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 ${TGUI_HAS_BACKEND_SDL_TTF_OPENGL3} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_SDL_TTF_GLES2 ${TGUI_HAS_BACKEND_SDL_TTF_GLES2} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_GLFW_OPENGL3 ${TGUI_HAS_BACKEND_GLFW_OPENGL3} PARENT_SCOPE)
set(TGUI_HAS_BACKEND_GLFW_GLES2 ${TGUI_HAS_BACKEND_GLFW_GLES2} PARENT_SCOPE)
