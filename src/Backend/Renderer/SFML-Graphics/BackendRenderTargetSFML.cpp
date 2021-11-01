/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Backend/Renderer/SFML-Graphics/BackendRenderTargetSFML.hpp>
#include <TGUI/Backend/Renderer/SFML-Graphics/BackendTextureSFML.hpp>
#include <TGUI/Container.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>
#include <array>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TGUI_IGNORE_DEPRECATED_WARNINGS_START
    BackendRenderTargetSFML::BackendRenderTargetSFML(sf::RenderTarget& target) :
        m_target(&target)
    {
    }
TGUI_IGNORE_DEPRECATED_WARNINGS_END
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderTarget* BackendRenderTargetSFML::getTarget() const
    {
        return m_target;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::setClearColor(const Color& color)
    {
        m_clearColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::clearScreen()
    {
        m_target->clear({m_clearColor.getRed(), m_clearColor.getGreen(), m_clearColor.getBlue(), m_clearColor.getAlpha()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::drawGui(const std::shared_ptr<RootContainer>& root)
    {
        if (!m_target || (m_targetSize.x == 0) || (m_targetSize.y == 0) || (m_viewRect.width <= 0) || (m_viewRect.height <= 0))
            return;

        // Change the view
        const sf::View oldView = m_target->getView();
        updateClipping(m_viewRect, m_viewport);

        // Draw the widgets
        root->draw(*this, {});

        // Restore the old view
        m_target->setView(oldView);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::drawSprite(const RenderStates& states, const Sprite& sprite)
    {
        if (!sprite.isSet())
            return;

        // We can use the drawVertexArray function (called from the base class) if the sprite doesn't have a shader
        if (sprite.getTexture().getData()->svgImage || !sprite.getTexture().getShader())
            return BackendRenderTarget::drawSprite(states, sprite);

        RenderStates transformedStates = states;
        if (sprite.getRotation() != 0)
        {
            // A rotation can cause the image to be shifted, so we move it upfront so that it ends at the correct location
            transformedStates.transform.translate(-Transform().rotate(sprite.getRotation()).transformRect({{}, sprite.getSize()}).getPosition());
            transformedStates.transform.rotate(sprite.getRotation());
        }

        transformedStates.transform.translate(sprite.getPosition());

        const FloatRect& visibleRect = sprite.getVisibleRect();
        const bool clippingRequired = (visibleRect != FloatRect{});
        if (clippingRequired)
            addClippingLayer(transformedStates, {{visibleRect.left, visibleRect.top}, {visibleRect.width, visibleRect.height}});

        std::shared_ptr<BackendTexture> texture = sprite.getTexture().getData()->backendTexture;

        sf::RenderStates sfStates = convertRenderStates(transformedStates, texture);
        TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture), "BackendRenderTargetSFML::drawSprite requires backend texture of type BackendTextureSFML");
        sfStates.texture = &std::static_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture)->getInternalTexture();
        sfStates.shader = sprite.getTexture().getShader();

        const std::vector<Vertex>& vertices = sprite.getVertices();
        const std::vector<int>& indices = sprite.getIndices();
        std::vector<Vertex> triangleVertices(indices.size());
        for (unsigned int i = 0; i < indices.size(); ++i)
            triangleVertices[i] = vertices[indices[i]];

        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");
        const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(triangleVertices.data());
        m_target->draw(sfmlVertices, indices.size(), sf::PrimitiveType::Triangles, sfStates);

        if (clippingRequired)
            removeClippingLayer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::drawVertexArray(const RenderStates& states, const Vertex* vertices, std::size_t vertexCount, const int* indices, std::size_t indexCount, const std::shared_ptr<BackendTexture>& texture)
    {
        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");

        if (indices)
        {
            std::vector<Vertex> triangleVertices(indexCount);
            for (unsigned int i = 0; i < indexCount; ++i)
                triangleVertices[i] = vertices[indices[i]];

            const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(triangleVertices.data());
            m_target->draw(sfmlVertices, indexCount, sf::PrimitiveType::Triangles, convertRenderStates(states, texture));
        }
        else // There are no indices
        {
            const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(vertices);
            m_target->draw(sfmlVertices, vertexCount, sf::PrimitiveType::Triangles, convertRenderStates(states, texture));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::updateClipping(FloatRect clipRect, FloatRect clipViewport)
    {
        if ((clipViewport.width > 0) && (clipViewport.height > 0))
        {
            sf::View newView{{std::round(clipRect.left), std::round(clipRect.top),
                              std::round(clipRect.width), std::round(clipRect.height)}};
            newView.setViewport({clipViewport.left / m_targetSize.x, clipViewport.top / m_targetSize.y,
                                 clipViewport.width / m_targetSize.x, clipViewport.height / m_targetSize.y});
            m_target->setView(newView);
        }
        else // Clip the entire window
        {
            sf::View clippingView{{0, 0, 0, 0}};
            clippingView.setViewport({0, 0, 0, 0});
            m_target->setView(clippingView);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderStates BackendRenderTargetSFML::convertRenderStates(const RenderStates& states, const std::shared_ptr<BackendTexture>& texture)
    {
        const float *transformMatrix = states.transform.getMatrix();

        sf::RenderStates statesSFML;
        statesSFML.transform = sf::Transform(
            transformMatrix[0], transformMatrix[4], std::round(transformMatrix[12]),
            transformMatrix[1], transformMatrix[5], std::round(transformMatrix[13]),
            transformMatrix[3], transformMatrix[7], transformMatrix[15]);

        if (texture)
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSFML>(texture), "BackendRenderTargetSFML requires textures of type BackendTextureSFML");
            statesSFML.texture = &std::static_pointer_cast<BackendTextureSFML>(texture)->getInternalTexture();
        }

        return statesSFML;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
