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


#include <TGUI/extlibs/IncludeSDL.hpp>
#include <TGUI/Backend/Renderer/SDL_Renderer/BackendRendererSDL.hpp>
#include <TGUI/Backend/Renderer/SDL_Renderer/CanvasSDL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Loading/WidgetFactory.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRendererSDL::BackendRendererSDL(SDL_Renderer* renderer) :
        m_renderer{renderer}
    {
        TGUI_ASSERT(m_renderer, "renderer passed to BackendRendererSDL can't be a nullptr");

        if (!WidgetFactory::getConstructFunction(U"CanvasSDL"))
            WidgetFactory::setConstructFunction(U"CanvasSDL", std::make_shared<CanvasSDL>);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendRendererSDL::createTexture()
    {
        return std::make_shared<BackendTextureSDL>(m_renderer);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int BackendRendererSDL::getMaximumTextureSize()
    {
        if ((m_maxTextureSize == 0) && m_renderer)
        {
            SDL_RendererInfo info;
            if (SDL_GetRendererInfo(m_renderer, &info) == 0)
                m_maxTextureSize = std::min(info.max_texture_width, info.max_texture_height);
        }

        return static_cast<unsigned int>(m_maxTextureSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SDL_Renderer* BackendRendererSDL::getInternalRenderer() const
    {
        return m_renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
