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


#include <TGUI/Backend/Renderer/GLES2/CanvasGLES2.hpp>
#include <TGUI/Backend/Renderer/GLES2/BackendRendererGLES2.hpp>
#include <TGUI/Backend/Renderer/OpenGL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char CanvasGLES2::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasGLES2::CanvasGLES2(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, initRenderer}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererGLES2>(getBackend()->getRenderer()),
                    "CanvasGLES2 can only be created when using the GLES2 backend renderer");

        glGenFramebuffers(1, &m_frameBuffer);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasGLES2::CanvasGLES2(const CanvasGLES2& other) :
        ClickableWidget{other}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererGLES2>(getBackend()->getRenderer()),
                    "CanvasGLES2 can only be used when using the GLES2 backend renderer");

        glGenFramebuffers(1, &m_frameBuffer);

        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasGLES2::~CanvasGLES2()
    {
        if (m_frameBuffer)
            glDeleteFramebuffers(1, &m_frameBuffer);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasGLES2& CanvasGLES2::operator= (const CanvasGLES2& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasGLES2::Ptr CanvasGLES2::create(const Layout2d& size)
    {
        auto canvas = std::make_shared<CanvasGLES2>();
        canvas->setSize(size);
        return canvas;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasGLES2::Ptr CanvasGLES2::copy(const CanvasGLES2::ConstPtr& canvas)
    {
        if (canvas)
            return std::static_pointer_cast<CanvasGLES2>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasGLES2::setSize(const Layout2d& size)
    {
        Widget::setSize(size);
        const Vector2f newSize = getSize();

        if ((newSize.x > 0) && (newSize.y > 0))
        {
            const Vector2u newTextureSize{newSize};
            if ((m_textureSize.x < newTextureSize.x) || (m_textureSize.y < newTextureSize.y))
            {
                TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererGLES2>(getBackend()->getRenderer()),
                    "CanvasGLES2 can only be used when using the GLES2 backend renderer");

                m_backendTexture->loadTextureOnly(newTextureSize, nullptr, false);
                m_textureSize = newTextureSize;
            }

            m_usedTextureSize = newTextureSize;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasGLES2::draw(BackendRenderTarget& target, RenderStates states) const
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

    unsigned int CanvasGLES2::bindFramebuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_backendTexture->getInternalTexture(), 0);
        return m_frameBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CanvasGLES2::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CanvasGLES2::clone() const
    {
        return std::make_shared<CanvasGLES2>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
