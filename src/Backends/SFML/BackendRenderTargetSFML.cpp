/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Backends/SFML/BackendRenderTargetSFML.hpp>
#include <TGUI/Backends/SFML/BackendTextSFML.hpp>
#include <TGUI/Backends/SFML/BackendTextureSFML.hpp>
#include <TGUI/Container.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>
#include <array>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_IGNORE_DEPRECATED_WARNINGS_START
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRenderTargetSFML::BackendRenderTargetSFML(sf::RenderTarget& target) :
        m_target(&target)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderTarget* BackendRenderTargetSFML::getTarget() const
    {
        return m_target;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    void BackendRenderTargetSFML::setView(FloatRect view, FloatRect viewport)
    {
        BackendRenderTargetBase::setView(view, viewport, {static_cast<float>(m_target->getSize().x), static_cast<float>(m_target->getSize().y)});

        m_view.setViewport({viewport.left / m_targetSize.x, viewport.top / m_targetSize.y,
                            viewport.width / m_targetSize.x, viewport.height / m_targetSize.y});
        m_view.setSize(view.width, view.height);
        m_view.setCenter(view.left + (view.width / 2.f), view.top + (view.height / 2.f));
    }
#endif

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
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    void BackendRenderTargetSFML::addClippingLayer(const RenderStates& states, FloatRect rect)
    {
        BackendRenderTargetBase::addClippingLayer(states, rect);

        const sf::View& oldView = m_clippingLayers.empty() ? m_view : m_clippingLayers.back().second;

        const float* transformMatrix = states.transform.getMatrix();
        if (((std::abs(transformMatrix[1]) > 0.00001f) || (std::abs(transformMatrix[4]) > 0.00001f)) // 0° or 180°
         && ((std::abs(transformMatrix[1] - 1) > 0.00001f) || (std::abs(transformMatrix[4] + 1) > 0.00001f)) // 90°
         && ((std::abs(transformMatrix[1] + 1) > 0.00001f) || (std::abs(transformMatrix[4] - 1) > 0.00001f))) // -90°
        {
            const FloatRect& oldClippingRect = m_clippingLayers.empty() ? m_viewRect : m_clippingLayers.back().first;
            m_clippingLayers.push_back({oldClippingRect, oldView});
            return;
        }

        Vector2f bottomRight{states.transform.transformPoint(rect.getPosition() + rect.getSize())};
        Vector2f topLeft = states.transform.transformPoint(rect.getPosition());
        const FloatRect clipRect = {topLeft, bottomRight - topLeft};

        Vector2f viewTopLeft = topLeft;
        Vector2f size = bottomRight - topLeft;

        topLeft.x -= m_viewRect.left;
        topLeft.y -= m_viewRect.top;
        bottomRight.x -= m_viewRect.left;
        bottomRight.y -= m_viewRect.top;

        topLeft.x *= m_viewport.width / m_targetSize.x / m_viewRect.width;
        topLeft.y *= m_viewport.height / m_targetSize.y / m_viewRect.height;
        size.x *= m_viewport.width / m_targetSize.x / m_viewRect.width;
        size.y *= m_viewport.height / m_targetSize.y / m_viewRect.height;

        topLeft.x += m_viewport.left / m_targetSize.x;
        topLeft.y += m_viewport.top / m_targetSize.y;

        if (topLeft.x < oldView.getViewport().left)
        {
            size.x -= oldView.getViewport().left - topLeft.x;
            viewTopLeft.x += (oldView.getViewport().left - topLeft.x) * (m_viewRect.width / (m_viewport.width / m_targetSize.x));
            topLeft.x = oldView.getViewport().left;
        }
        if (topLeft.y < oldView.getViewport().top)
        {
            size.y -= oldView.getViewport().top - topLeft.y;
            viewTopLeft.y += (oldView.getViewport().top - topLeft.y) * (m_viewRect.height / (m_viewport.height / m_targetSize.y));
            topLeft.y = oldView.getViewport().top;
        }

        if (size.x > oldView.getViewport().left + oldView.getViewport().width - topLeft.x)
            size.x = oldView.getViewport().left + oldView.getViewport().width - topLeft.x;
        if (size.y > oldView.getViewport().top + oldView.getViewport().height - topLeft.y)
            size.y = oldView.getViewport().top + oldView.getViewport().height - topLeft.y;

        sf::View clippingView;
        if ((size.x >= 0) && (size.y >= 0))
        {
            clippingView = sf::View{{std::round(viewTopLeft.x),
                                     std::round(viewTopLeft.y),
                                     std::round(size.x * m_viewRect.width / (m_viewport.width / m_targetSize.x)),
                                     std::round(size.y * m_viewRect.height / (m_viewport.height / m_targetSize.y))}};
            clippingView.setViewport({topLeft.x, topLeft.y, size.x, size.y});
        }
        else // The clipping area lies outside the viewport
        {
            clippingView = sf::View{{0, 0, 0, 0}};
            clippingView.setViewport({0, 0, 0, 0});
        }

        m_clippingLayers.push_back({clipRect, clippingView});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::removeClippingLayer()
    {
        BackendRenderTargetBase::removeClippingLayer();

        TGUI_ASSERT(!m_clippingLayers.empty(), "BackendRenderTargetSFML::removeClippingLayer can't remove layer if there are none left");
        m_clippingLayers.pop_back();
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::drawSprite(const RenderStates& states, const Sprite& sprite)
    {
        if (!sprite.isSet())
            return;

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

        sf::RenderStates sfStates = convertRenderStates(transformedStates);
        if (sprite.getTexture().getData()->svgImage)
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSFML>(sprite.getSvgTexture()), "BackendRenderTargetSFML::drawSprite requires SVG texture of type BackendTextureSFML");
            sfStates.texture = &std::static_pointer_cast<BackendTextureSFML>(sprite.getSvgTexture())->getInternalTexture();
        }
        else
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture), "BackendRenderTargetSFML::drawSprite requires backend texture of type BackendTextureSFML");
            sfStates.texture = &std::static_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture)->getInternalTexture();
            sfStates.shader = sprite.getTexture().getShader();
        }

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

    void BackendRenderTargetSFML::drawText(const RenderStates& states, const Text& text)
    {
        RenderStates movedStates = states;
        movedStates.transform.translate(text.getPosition());

        // Round the position to avoid blurry text.
        // The top position is floored instead of rounded because it often results in the text looking more centered. A small
        // number is added before flooring to prevent 0.99 to be "rounded" to 0.
        sf::RenderStates sfStates;
        const float* matrix = movedStates.transform.getMatrix();
        sfStates.transform = sf::Transform{matrix[0], matrix[4], std::round(matrix[12]),
                                           matrix[1], matrix[5], std::floor(matrix[13] + 0.1f),
                                           matrix[3], matrix[7], matrix[15]};

        TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextSFML>(text.getBackendText()), "BackendRenderTargetSFML::drawText requires backend text of type BackendTextSFML");
        m_target->draw(std::static_pointer_cast<BackendTextSFML>(text.getBackendText())->getInternalText(), sfStates);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSFML::drawTriangles(const RenderStates& states, const Vertex* vertices, std::size_t vertexCount, const int* indices, std::size_t indexCount)
    {
        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");

        if (indices)
        {
            std::vector<Vertex> triangleVertices(indexCount);
            for (unsigned int i = 0; i < indexCount; ++i)
                triangleVertices[i] = vertices[indices[i]];

            const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(triangleVertices.data());
            m_target->draw(sfmlVertices, indexCount, sf::PrimitiveType::Triangles, convertRenderStates(states));
        }
        else // There are no indices
        {
            const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(vertices);
            m_target->draw(sfmlVertices, vertexCount, sf::PrimitiveType::Triangles, convertRenderStates(states));
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

    sf::RenderStates BackendRenderTargetSFML::convertRenderStates(const RenderStates& states)
    {
        const float *transformMatrix = states.transform.getMatrix();

        sf::RenderStates statesSFML;
        statesSFML.transform = sf::Transform(
            transformMatrix[0], transformMatrix[4], std::round(transformMatrix[12]),
            transformMatrix[1], transformMatrix[5], std::round(transformMatrix[13]),
            transformMatrix[3], transformMatrix[7], transformMatrix[15]);

        return statesSFML;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
TGUI_IGNORE_DEPRECATED_WARNINGS_END

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
