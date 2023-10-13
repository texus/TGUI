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


#include <TGUI/Backend/Renderer/OpenGL3/CanvasOpenGL3.hpp>
#include <TGUI/Backend/Renderer/OpenGL3/BackendRendererOpenGL3.hpp>
#include <TGUI/Backend/Renderer/OpenGL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
    import tgui.opengl;
#else
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char CanvasOpenGL3::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasOpenGL3::CanvasOpenGL3(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, initRenderer}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererOpenGL3>(getBackend()->getRenderer()),
                    "CanvasOpenGL3 can only be created when using the OpenGL3 backend renderer");

        glGenFramebuffers(1, &m_frameBuffer);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasOpenGL3::CanvasOpenGL3(const CanvasOpenGL3& other) :
        ClickableWidget{other}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererOpenGL3>(getBackend()->getRenderer()),
                    "CanvasOpenGL3 can only be used when using the OpenGL3 backend renderer");

        glGenFramebuffers(1, &m_frameBuffer);

        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasOpenGL3::~CanvasOpenGL3()
    {
        if (m_frameBuffer)
            glDeleteFramebuffers(1, &m_frameBuffer);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasOpenGL3& CanvasOpenGL3::operator= (const CanvasOpenGL3& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasOpenGL3::Ptr CanvasOpenGL3::create(const Layout2d& size)
    {
        auto canvas = std::make_shared<CanvasOpenGL3>();
        canvas->setSize(size);
        return canvas;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasOpenGL3::Ptr CanvasOpenGL3::copy(const CanvasOpenGL3::ConstPtr& canvas)
    {
        if (canvas)
            return std::static_pointer_cast<CanvasOpenGL3>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasOpenGL3::setSize(const Layout2d& size)
    {
        Widget::setSize(size);
        const Vector2f newSize = getSize();

        if ((newSize.x > 0) && (newSize.y > 0))
        {
            const Vector2u newTextureSize{newSize};
            if ((m_textureSize.x < newTextureSize.x) || (m_textureSize.y < newTextureSize.y))
            {
                TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererOpenGL3>(getBackend()->getRenderer()),
                    "CanvasOpenGL3 can only be used when using the OpenGL3 backend renderer");

                m_backendTexture->loadTextureOnly(newTextureSize, nullptr, false);
                m_textureSize = newTextureSize;
            }

            m_usedTextureSize = newTextureSize;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasOpenGL3::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const Vector2f size = getSize();
        if ((size.x <= 0) || (size.y <= 0) || (m_textureSize.x == 0) || (m_textureSize.y == 0))
            return;

        const Vector2f normalizedTextureSize{static_cast<float>(m_usedTextureSize.x) / static_cast<float>(m_textureSize.x),
                                             static_cast<float>(m_usedTextureSize.y) / static_cast<float>(m_textureSize.y)};
        const Vertex::Color vertexColor(Color::applyOpacity(Color::White, m_opacityCached));
        const std::array<Vertex, 4> vertices = {{
            {{0, 0}, vertexColor, {0, 0}},
            {{size.x, 0}, vertexColor, {normalizedTextureSize.x, 0}},
            {{0, size.y}, vertexColor, {0, -normalizedTextureSize.y}},
            {{size.x, size.y}, vertexColor, {normalizedTextureSize.x, -normalizedTextureSize.y}},
        }};
        const std::array<unsigned int, 6> indices = {{
            0, 2, 1,
            1, 2, 3
        }};
        target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), m_backendTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int CanvasOpenGL3::bindFramebuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_backendTexture->getInternalTexture(), 0);
        return m_frameBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CanvasOpenGL3::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CanvasOpenGL3::clone() const
    {
        return std::make_shared<CanvasOpenGL3>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
