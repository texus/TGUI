/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGUI_BACKEND_RENDERER_OPENGL3_HPP
#define TGUI_BACKEND_RENDERER_OPENGL3_HPP

#include <TGUI/Backend/Renderer/OpenGL3/BackendTextureOpenGL3.hpp>
#include <TGUI/Backend/Renderer/OpenGL3/BackendRenderTargetOpenGL3.hpp>
#include <TGUI/Backend/Renderer/OpenGL3/CanvasOpenGL3.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Backend/Renderer/BackendRenderer.hpp>
#endif

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <memory>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Backend renderer that uses OpenGL 3 or 4 (depending on the loaded OpenGL context that you have to provide)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendRendererOpenGL3 : public BackendRenderer
    {
    public:

        using ContextLoadFuncReturnType = void (*)();
        using ContextLoadFuncType = ContextLoadFuncReturnType (*)(const char *name);

        using SDLContextLoadFuncType = void* (*)(const char *name);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Initializes the backend renderer. This will initialize GLAD to use (modern) OpenGL
        ///
        /// @param contextLoadFunction  Function that gets the address of an OpenGL function
        ///
        /// Examples of load functions are sf::Context::getFunction, SDL_GL_GetProcAddress and glfwGetProcAddress.
        ///
        /// Example usage:
        /// @code
        /// backend->setRenderer(std::make_shared<BackendRendererOpenGL3>(glfwGetProcAddress));
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendRendererOpenGL3(ContextLoadFuncType contextLoadFunction);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Helper constructor so that you can pass SDL_GL_GetProcAddress without needing to cast the type
        /// @see BackendRendererOpenGL3(ContextLoadFuncType)
        ///
        /// @param contextLoadFunction  Function that gets the address of an OpenGL function
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendRendererOpenGL3(SDLContextLoadFuncType contextLoadFunction);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new empty texture object
        /// @return Shared pointer to a new texture object
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::shared_ptr<BackendTexture> createTexture() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Get the maximum allowed texture size
        ///
        /// @return Maximum width and height that you should try to use in a single texture
        ///
        /// This maximum size is defined by the graphics driver. Most likely this will return 8192 or 16384.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD unsigned int getMaximumTextureSize() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        int m_maxTextureSize = 0;
    };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_RENDERER_OPENGL3_HPP
