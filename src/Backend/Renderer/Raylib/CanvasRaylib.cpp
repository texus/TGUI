/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Renderer/Raylib/CanvasRaylib.hpp>
#include <TGUI/Backend/Renderer/Raylib/BackendRendererRaylib.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char CanvasRaylib::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasRaylib::CanvasRaylib(const char* typeName, bool initRenderer) :
        CanvasBase{typeName, initRenderer}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererRaylib>(getBackend()->getRenderer()),
                    "CanvasRaylib can only be created when using the Raylib backend renderer");

        m_backendTexture = std::make_shared<BackendTextureRaylib>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasRaylib::~CanvasRaylib()
    {
        if (m_textureTarget.id > 0)
        {
            // The m_backendTexture is using the exact same texture as our render target (due to the call to replaceInternalTexture).
            // To prevent the texture from being freed twice, we shouldn't let UnloadRenderTexture delete the texture.
            m_textureTarget.texture.id = 0;
            UnloadRenderTexture(m_textureTarget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasRaylib::CanvasRaylib(const CanvasRaylib& other) :
        CanvasBase{other}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererRaylib>(getBackend()->getRenderer()),
                    "CanvasRaylib can only be used when using the Raylib backend renderer");

        m_backendTexture = std::make_shared<BackendTextureRaylib>();
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasRaylib& CanvasRaylib::operator= (const CanvasRaylib& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasRaylib::Ptr CanvasRaylib::create(const Layout2d& size)
    {
        auto canvas = std::make_shared<CanvasRaylib>();
        canvas->setSize(size);
        return canvas;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasRaylib::Ptr CanvasRaylib::copy(const CanvasRaylib::ConstPtr& canvas)
    {
        if (canvas)
            return std::static_pointer_cast<CanvasRaylib>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasRaylib::setSize(const Layout2d& size)
    {
        Widget::setSize(size);
        const Vector2f newSize = getSize();

        if ((newSize.x > 0) && (newSize.y > 0))
        {
            const Vector2u newTextureSize{newSize};
            if ((m_textureSize.x < newTextureSize.x) || (m_textureSize.y < newTextureSize.y))
            {
                TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererRaylib>(getBackend()->getRenderer()),
                    "CanvasRaylib can only be used when using the Raylib backend renderer");

                m_textureTarget = LoadRenderTexture(static_cast<int>(newTextureSize.x), static_cast<int>(newTextureSize.y));

                // Move the ownership of the texture to our backend texture
                m_backendTexture->replaceInternalTexture(m_textureTarget.texture);

                if (m_textureTarget.id)
                    m_textureSize = newTextureSize;
                else
                    m_textureSize = {};
            }

            m_usedTextureSize = newTextureSize;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasRaylib::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const Vector2f size = getSize();
        if ((size.x <= 0) || (size.y <= 0) || (m_textureSize.x <= 0) || (m_textureSize.y <= 0))
            return;

        const Vector2f normalizedTextureSize{static_cast<float>(m_usedTextureSize.x) / static_cast<float>(m_textureSize.x),
                                             static_cast<float>(m_usedTextureSize.y) / static_cast<float>(m_textureSize.y)};
        const Vertex::Color vertexColor(Color::applyOpacity(Color::White, m_opacityCached));
        const std::array<Vertex, 4> vertices = {{
            {{0, 0}, vertexColor, {0, normalizedTextureSize.y}},
            {{size.x, 0}, vertexColor, {normalizedTextureSize.x, normalizedTextureSize.y}},
            {{0, size.y}, vertexColor, {0, 0}},
            {{size.x, size.y}, vertexColor, {normalizedTextureSize.x, 0}},
        }};
        const std::array<unsigned int, 6> indices = {{
            0, 2, 1,
            1, 2, 3
        }};
        target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), m_backendTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CanvasRaylib::clone() const
    {
        return std::make_shared<CanvasRaylib>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
