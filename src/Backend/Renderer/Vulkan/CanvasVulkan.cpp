////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Backend/Renderer/Vulkan/BackendRendererVulkan.hpp>
#include <TGUI/Backend/Renderer/Vulkan/CanvasVulkan.hpp>
#include <TGUI/Backend/Window/Backend.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    CanvasVulkan::CanvasVulkan(const char* typeName, bool initRenderer) :
        CanvasBase{typeName, initRenderer}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer()
                        && std::dynamic_pointer_cast<BackendRendererVulkan>(getBackend()->getRenderer()),
                    "CanvasVulkan can only be created when using the Vulkan backend renderer");

        m_backendTexture = std::make_shared<BackendTextureVulkan>(std::static_pointer_cast<BackendRendererVulkan>(
                                                                      getBackend()->getRenderer()),
                                                                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasVulkan::CanvasVulkan(const CanvasVulkan& other) :
        CanvasBase{other}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer()
                        && std::dynamic_pointer_cast<BackendRendererVulkan>(getBackend()->getRenderer()),
                    "CanvasVulkan can only be used when using the Vulkan backend renderer");

        m_backendTexture = std::make_shared<BackendTextureVulkan>(std::static_pointer_cast<BackendRendererVulkan>(
                                                                      getBackend()->getRenderer()),
                                                                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

        setSize(other.getSize());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasVulkan& CanvasVulkan::operator=(const CanvasVulkan& right)
    {
        if (this != &right)
        {
            CanvasBase::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasVulkan::Ptr CanvasVulkan::create(const Layout2d& size)
    {
        auto canvas = std::make_shared<CanvasVulkan>();
        canvas->setSize(size);
        return canvas;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasVulkan::Ptr CanvasVulkan::copy(const CanvasVulkan::ConstPtr& canvas)
    {
        if (canvas)
            return std::static_pointer_cast<CanvasVulkan>(canvas->clone());
        else
            return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasVulkan::setSize(const Layout2d& size)
    {
        CanvasBase::setSize(size);

        // An image needs to be at least 1 pixel wide and high, so a size between 0 and 1 is treated the same as 0
        const Vector2f newSize = getSize();
        if ((newSize.x >= 1) && (newSize.y >= 1))
        {
            const Vector2u newTextureSize{newSize};
            if ((m_textureSize.x != newTextureSize.x) || (m_textureSize.y != newTextureSize.y))
            {
                // Passing a nullptr as pixels creates a new image that is cleared to be fully transparent
                if (m_backendTexture->loadTextureOnly(newTextureSize, nullptr, m_backendTexture->isSmooth()))
                    m_textureSize = newTextureSize;
                else
                    m_textureSize = {};
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasVulkan::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const Vector2f size = getSize();
        if ((size.x <= 0) || (size.y <= 0) || (m_textureSize.x <= 0) || (m_textureSize.y <= 0))
            return;

        const Vertex::Color vertexColor(Color::applyOpacity(Color::White, m_opacityCached));
        const std::array<Vertex, 4> vertices = {{
            {{0, 0}, vertexColor, {0, 0}},
            {{size.x, 0}, vertexColor, {1, 0}},
            {{0, size.y}, vertexColor, {0, 1}},
            {{size.x, size.y}, vertexColor, {1, 1}},
        }};
        const std::array<unsigned int, 6> indices = {{0, 2, 1, 1, 2, 3}};
        target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), m_backendTexture);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CanvasVulkan::clone() const
    {
        return std::make_shared<CanvasVulkan>(*this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
