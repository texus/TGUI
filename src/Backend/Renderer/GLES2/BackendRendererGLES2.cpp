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


#include <TGUI/Backend/Renderer/GLES2/BackendRendererGLES2.hpp>
#include <TGUI/Backend/Renderer/GLES2/CanvasGLES2.hpp>
#include <TGUI/Backend/Renderer/OpenGL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Exception.hpp>
    #include <TGUI/Loading/WidgetFactory.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRendererGLES2::BackendRendererGLES2(ContextLoadFuncType contextLoadFunction)
    {
        const int version = tgui_gladLoadGLES2(reinterpret_cast<GLADloadfunc>(contextLoadFunction));
        if (!TGUI_GLAD_GL_ES_VERSION_2_0)
        {
            if (version == 0)
                throw Exception{U"BackendRendererGLES2 failed to query OpenGL ES version, or requested OpenGL ES version wasn't supported. Has an OpenGL ES context been created yet?"};
            else
            {
                throw Exception{U"BackendRendererGLES2 expects at least OpenGL ES 2.0, found version "
                    + String(GLAD_VERSION_MAJOR(version)) + '.' + String(GLAD_VERSION_MINOR(version))};
            }
        }

        if (!WidgetFactory::getConstructFunction(U"CanvasGLES2"))
            WidgetFactory::setConstructFunction(U"CanvasGLES2", std::make_shared<CanvasGLES2>);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRendererGLES2::BackendRendererGLES2(SDLContextLoadFuncType contextLoadFunction) :
        BackendRendererGLES2(reinterpret_cast<ContextLoadFuncType>(contextLoadFunction))
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendRendererGLES2::createTexture()
    {
        return std::make_shared<BackendTextureGLES2>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int BackendRendererGLES2::getMaximumTextureSize()
    {
        if (m_maxTextureSize == 0)
            TGUI_GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize));

        return static_cast<unsigned int>(m_maxTextureSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
