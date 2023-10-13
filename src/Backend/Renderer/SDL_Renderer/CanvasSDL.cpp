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


#include <TGUI/Backend/Renderer/SDL_Renderer/CanvasSDL.hpp>
#include <TGUI/Backend/Renderer/SDL_Renderer/BackendRendererSDL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char CanvasSDL::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDL::CanvasSDL(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, initRenderer}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererSDL>(getBackend()->getRenderer()),
                    "CanvasSDL can only be created when using the SDL_Renderer backend renderer");

        SDL_Renderer* sdlRenderer = std::static_pointer_cast<BackendRendererSDL>(getBackend()->getRenderer())->getInternalRenderer();
        m_backendTexture = std::make_shared<BackendTextureSDL>(sdlRenderer);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDL::CanvasSDL(const CanvasSDL& other) :
        ClickableWidget{other}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererSDL>(getBackend()->getRenderer()),
                    "CanvasSDL can only be used when using the SDL_Renderer backend renderer");

        SDL_Renderer* sdlRenderer = std::static_pointer_cast<BackendRendererSDL>(getBackend()->getRenderer())->getInternalRenderer();
        m_backendTexture = std::make_shared<BackendTextureSDL>(sdlRenderer);

        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDL& CanvasSDL::operator= (const CanvasSDL& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDL::Ptr CanvasSDL::create(const Layout2d& size)
    {
        auto canvas = std::make_shared<CanvasSDL>();
        canvas->setSize(size);
        return canvas;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDL::Ptr CanvasSDL::copy(const CanvasSDL::ConstPtr& canvas)
    {
        if (canvas)
            return std::static_pointer_cast<CanvasSDL>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSDL::setSize(const Layout2d& size)
    {
        Widget::setSize(size);
        const Vector2f newSize = getSize();

        if ((newSize.x > 0) && (newSize.y > 0))
        {
            const Vector2u newTextureSize{newSize};
            if ((m_textureSize.x < newTextureSize.x) || (m_textureSize.y < newTextureSize.y))
            {
                TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererSDL>(getBackend()->getRenderer()),
                    "CanvasSDL can only be used when using the SDL_Renderer backend renderer");
                SDL_Renderer* sdlRenderer = std::static_pointer_cast<BackendRendererSDL>(getBackend()->getRenderer())->getInternalRenderer();

                m_textureTarget = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
                                                    static_cast<int>(newTextureSize.x), static_cast<int>(newTextureSize.y));

                // Move the ownership of the texture to our backend texture (we thus don't need to call SDL_DestroyTexture ourselves)
                m_backendTexture->replaceInternalTexture(m_textureTarget);

                if (m_textureTarget)
                    m_textureSize = newTextureSize;
                else
                    m_textureSize = {};
            }

            m_usedTextureSize = newTextureSize;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSDL::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const Vector2f size = getSize();
        if ((size.x <= 0) || (size.y <= 0) || (m_textureSize.x <= 0) || (m_textureSize.y <= 0))
            return;

        const Vector2f normalizedTextureSize{static_cast<float>(m_usedTextureSize.x) / static_cast<float>(m_textureSize.x),
                                             static_cast<float>(m_usedTextureSize.y) / static_cast<float>(m_textureSize.y)};
        const Vertex::Color vertexColor(Color::applyOpacity(Color::White, m_opacityCached));
        const std::array<Vertex, 4> vertices = {{
            {{0, 0}, vertexColor, {0, 0}},
            {{size.x, 0}, vertexColor, {normalizedTextureSize.x, 0}},
            {{0, size.y}, vertexColor, {0, normalizedTextureSize.y}},
            {{size.x, size.y}, vertexColor, {normalizedTextureSize.x, normalizedTextureSize.y}},
        }};
        const std::array<unsigned int, 6> indices = {{
            0, 2, 1,
            1, 2, 3
        }};
        target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), m_backendTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CanvasSDL::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CanvasSDL::clone() const
    {
        return std::make_shared<CanvasSDL>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
