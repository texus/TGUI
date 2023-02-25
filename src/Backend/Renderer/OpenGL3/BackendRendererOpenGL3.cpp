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


#include <TGUI/Backend/Renderer/OpenGL3/BackendRendererOpenGL3.hpp>
#include <TGUI/Backend/Renderer/OpenGL3/CanvasOpenGL3.hpp>
#include <TGUI/Backend/Renderer/OpenGL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
    import tgui.opengl;
#else
    #include <TGUI/Exception.hpp>
    #include <TGUI/Loading/WidgetFactory.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRendererOpenGL3::BackendRendererOpenGL3(ContextLoadFuncType contextLoadFunction)
    {
        const int version = tgui_gladLoadGL(reinterpret_cast<GLADloadfunc>(contextLoadFunction));
        if ((GLAD_VERSION_MAJOR(version) < 3) || ((GLAD_VERSION_MAJOR(version) == 3) && GLAD_VERSION_MINOR(version) < 3))
        {
            if (version == 0)
                throw Exception{U"BackendRendererOpenGL failed to query OpenGL version, or requested OpenGL version wasn't supported. Has an OpenGL context been created yet?"};
            else
            {
                throw Exception{U"BackendRendererOpenGL expects at least OpenGL 3.3, found version "
                    + String(GLAD_VERSION_MAJOR(version)) + '.' + String(GLAD_VERSION_MINOR(version))};
            }
        }

        if (!WidgetFactory::getConstructFunction(U"CanvasOpenGL3"))
            WidgetFactory::setConstructFunction(U"CanvasOpenGL3", std::make_shared<CanvasOpenGL3>);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRendererOpenGL3::BackendRendererOpenGL3(SDLContextLoadFuncType contextLoadFunction) :
        BackendRendererOpenGL3(reinterpret_cast<ContextLoadFuncType>(contextLoadFunction))
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendRendererOpenGL3::createTexture()
    {
        return std::make_shared<BackendTextureOpenGL3>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int BackendRendererOpenGL3::getMaximumTextureSize()
    {
        if (m_maxTextureSize == 0)
            TGUI_GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize));

        return static_cast<unsigned int>(m_maxTextureSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
