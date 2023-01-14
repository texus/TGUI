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


#include <TGUI/Backend/GLFW-GLES2.hpp>

#define GLFW_INCLUDE_NONE // Don't let GLFW include an OpenGL extention loader
#include <GLFW/glfw3.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    inline namespace GLFW_GLES2
    {
        void Gui::setWindow(GLFWwindow* window)
        {
            if (!isBackendSet())
            {
                auto backend = std::make_shared<BackendGLFW>();
                backend->setFontBackend(std::make_shared<BackendFontFactoryImpl<BackendFontFreetype>>());
                backend->setRenderer(std::make_shared<BackendRendererGLES2>(glfwGetProcAddress));
                backend->setDestroyOnLastGuiDetatch(true);
                setBackend(backend);
            }

            m_backendRenderTarget = std::make_shared<BackendRenderTargetGLES2>();
            setGuiWindow(window);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
