/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/RenderTarget.hpp>
#include <TGUI/Container.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>
#include <array>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    void RenderTargetBase::drawBorders(const RenderStates& states, const Borders& borders, Vector2f size, Color color)
    {
        //////////////////////
        // 0---1----------6 //
        // |              | //
        // |   8------7   | //
        // |   |      |   | //
        // |   |      |   | //
        // |   3------5   | //
        // |              | //
        // 2--------------4 //
        //////////////////////
        const auto vertexColor = Vertex::Color(color);
        drawTriangles(states,
            {
                {{0, 0}, vertexColor},
                {{borders.getLeft(), 0}, vertexColor},
                {{0, size.y}, vertexColor},
                {{borders.getLeft(), size.y - borders.getBottom()}, vertexColor},
                {{size.x, size.y}, vertexColor},
                {{size.x - borders.getRight(), size.y - borders.getBottom()}, vertexColor},
                {{size.x, 0}, vertexColor},
                {{size.x - borders.getRight(), borders.getTop()}, vertexColor},
                {{borders.getLeft(), borders.getTop()}, vertexColor}
            },
            {
                0, 2, 1,
                1, 2, 3,
                2, 4, 3,
                3, 4, 5,
                4, 6, 5,
                5, 6, 7,
                6, 1, 7,
                7, 1, 8
            }
        );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTargetBase::drawFilledRect(const RenderStates& states, Vector2f size, Color color)
    {
        const auto vertexColor = Vertex::Color(color);
        drawTriangles(states,
            {
                {{0, 0}, vertexColor},
                {{0, size.y}, vertexColor},
                {{size.x, 0}, vertexColor},
                {{size.x, size.y}, vertexColor}
            },
            {
                0, 1, 2,
                2, 1, 3
            }
        );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTargetBase::drawTriangles(const RenderStates& states, std::initializer_list<Vertex> vertices, std::initializer_list<int> indices)
    {
        if (indices.size() == 0)
            drawTriangles(states, vertices.begin(), vertices.size());
        else
        {
            assert(indices.size() % 3 == 0);
            drawTriangles(states, vertices.begin(), vertices.size(), indices.begin(), indices.size());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::setTarget(sf::RenderTarget& target)
    {
        m_target = &target;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderTarget* RenderTarget::getTarget() const
    {
        return m_target;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::setView(FloatRect view, FloatRect viewport)
    {
        assert(m_clippingLayers.empty()); // You can't change the view of the render target during drawing

        m_view.setViewport({viewport.left / m_target->getSize().x, viewport.top / m_target->getSize().y,
                            viewport.width / m_target->getSize().x, viewport.height / m_target->getSize().y});
        m_view.setSize(view.width, view.height);
        m_view.setCenter(view.left + (view.width / 2.f), view.top + (view.height / 2.f));
        m_viewRect = view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::drawGui(const std::shared_ptr<RootContainer>& root)
    {
        if (!m_target)
            return;

        // Change the view
        const sf::View oldView = m_target->getView();
        m_target->setView(m_view);

        // Draw the widgets
        root->draw(*this, {});

        // Restore the old view
        m_target->setView(oldView);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::drawWidget(const RenderStates& states, const std::shared_ptr<Widget>& widget)
    {
        // If the widget lies outside of the clip rect then we can skip drawing it
        const FloatRect& clipRect = m_clippingLayers.empty() ? m_viewRect : m_clippingLayers.back().first;
        const Vector2f widgetBottomRight{states.transform.transformPoint(widget->getWidgetOffset() + widget->getFullSize())};
        const Vector2f widgetTopLeft = states.transform.transformPoint(widget->getWidgetOffset());
        if ((widgetTopLeft.x > clipRect.left + clipRect.width) || (widgetTopLeft.y > clipRect.top + clipRect.height)
         || (widgetBottomRight.x < clipRect.left) || (widgetBottomRight.y < clipRect.top))
            return;

        widget->draw(*this, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::addClippingLayer(const RenderStates& states, FloatRect rect)
    {
        const FloatRect& oldClipRect = m_clippingLayers.empty() ? m_viewRect : m_clippingLayers.back().first;
        const sf::View& oldView = m_clippingLayers.empty() ? m_view : m_clippingLayers.back().second;

        /// TODO: We currently can't clip rotated objects
        const float* transformMatrix = states.transform.getMatrix();
        if ((std::abs(transformMatrix[1]) > 0.00001f) || (std::abs(transformMatrix[4]) > 0.00001f))
        {
            m_clippingLayers.push_back({oldClipRect, oldView});
            return;
        }

        Vector2f bottomRight{states.transform.transformPoint(rect.getPosition() + rect.getSize())};
        Vector2f topLeft = states.transform.transformPoint(rect.getPosition());
        const FloatRect clipRect = {topLeft, bottomRight - topLeft};

        Vector2f viewTopLeft = topLeft;
        Vector2f size = bottomRight - topLeft;

        topLeft.x -= m_view.getCenter().x - (m_view.getSize().x / 2.f);
        topLeft.y -= m_view.getCenter().y - (m_view.getSize().y / 2.f);
        bottomRight.x -= m_view.getCenter().x - (m_view.getSize().x / 2.f);
        bottomRight.y -= m_view.getCenter().y - (m_view.getSize().y / 2.f);

        topLeft.x *= m_view.getViewport().width / m_view.getSize().x;
        topLeft.y *= m_view.getViewport().height / m_view.getSize().y;
        size.x *= m_view.getViewport().width / m_view.getSize().x;
        size.y *= m_view.getViewport().height / m_view.getSize().y;

        topLeft.x += m_view.getViewport().left;
        topLeft.y += m_view.getViewport().top;

        if (topLeft.x < oldView.getViewport().left)
        {
            size.x -= oldView.getViewport().left - topLeft.x;
            viewTopLeft.x += (oldView.getViewport().left - topLeft.x) * (m_view.getSize().x / m_view.getViewport().width);
            topLeft.x = oldView.getViewport().left;
        }
        if (topLeft.y < oldView.getViewport().top)
        {
            size.y -= oldView.getViewport().top - topLeft.y;
            viewTopLeft.y += (oldView.getViewport().top - topLeft.y) * (m_view.getSize().y / m_view.getViewport().height);
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
                                     std::round(size.x * m_view.getSize().x / m_view.getViewport().width),
                                     std::round(size.y * m_view.getSize().y / m_view.getViewport().height)}};
            clippingView.setViewport({topLeft.x, topLeft.y, size.x, size.y});
        }
        else // The clipping area lies outside the viewport
        {
            clippingView = sf::View{{0, 0, 0, 0}};
            clippingView.setViewport({0, 0, 0, 0});
        }

        m_clippingLayers.push_back({clipRect, clippingView});

        m_target->setView(clippingView);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::removeClippingLayer()
    {
        assert(!m_clippingLayers.empty());

        m_clippingLayers.pop_back();
        if (m_clippingLayers.empty())
            m_target->setView(m_view);
        else
            m_target->setView(m_clippingLayers.back().second);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::drawSprite(const RenderStates& states, const Sprite& sprite)
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

        sf::RenderStates sfStates = transformedStates;
        if (sprite.getTexture().getData()->svgImage)
            sfStates.texture = sprite.getSvgTexture().get();
        else
        {
            sfStates.texture = &sprite.getTexture().getData()->texture.value();
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

    void RenderTarget::drawText(const RenderStates& states, const Text& text)
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

        m_target->draw(text.getSFMLText(), sfStates);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::drawTriangles(const RenderStates& states, const Vertex* vertices, std::size_t vertexCount, const int* indices, std::size_t indexCount)
    {
        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");

        if (indices)
        {
            std::vector<Vertex> triangleVertices(indexCount);
            for (unsigned int i = 0; i < indexCount; ++i)
                triangleVertices[i] = vertices[indices[i]];

            const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(triangleVertices.data());
            m_target->draw(sfmlVertices, indexCount, sf::PrimitiveType::Triangles, states);
        }
        else // There are no indices
        {
            const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(vertices);
            m_target->draw(sfmlVertices, vertexCount, sf::PrimitiveType::Triangles, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RenderTarget::drawCircle(const RenderStates& states, float size, const Color& backgroundColor, unsigned int borderThickness, const Color& borderColor)
    {
        sf::CircleShape bordersShape{size / 2};
        bordersShape.setFillColor(sf::Color(backgroundColor));
        bordersShape.setOutlineColor(sf::Color(borderColor));
        bordersShape.setOutlineThickness(borderThickness);
        m_target->draw(bordersShape, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
