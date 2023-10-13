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


#include <TGUI/Backend/Renderer/BackendRenderTarget.hpp>
#include <TGUI/Backend/Renderer/BackendText.hpp>
#include <TGUI/Widget.hpp>

#include <cassert>
#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <array>
    #include <cmath>

    #if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
        #include <numbers>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD static std::vector<Vector2f> drawCircleHelperGetPoints(unsigned int nrPoints, float radius, float offset)
    {
#if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
        const float twoPi = 2.f * std::numbers::pi_v<float>;
#else
        const float twoPi = 2.f * 3.14159265359f;
#endif
        std::vector<Vector2f> points;
        points.reserve(nrPoints);

        for (unsigned int i = 0; i < nrPoints; ++i)
        {
            points.emplace_back(offset + radius + (radius * std::cos(twoPi * i / nrPoints)),
                                offset + radius + (radius * std::sin(twoPi * i / nrPoints)));
        }

        return points;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD static std::vector<Vector2f> drawRoundedRectHelperGetPoints(unsigned int nrCornerPoints, const Vector2f& size, float radius, float offset)
    {
        assert(nrCornerPoints != 0);

#if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
        const float twoPi = 2.f * std::numbers::pi_v<float>;
#else
        const float twoPi = 2.f * 3.14159265359f;
#endif
        std::vector<Vector2f> points;
        points.reserve(nrCornerPoints * 4);

        const unsigned int nrPointsInCircle = 4 * (nrCornerPoints - 1);

        // Top right corner
        for (unsigned int i = 0; i < nrCornerPoints; ++i)
        {
            points.emplace_back(offset + size.x - radius + (radius * std::cos(twoPi * i / nrPointsInCircle)),
                                offset + radius - (radius * std::sin(twoPi * i / nrPointsInCircle)));
        }

        // Top left corner
        for (unsigned int i = 0; i < nrCornerPoints; ++i)
        {
            points.emplace_back(offset + radius + (radius * std::cos(twoPi * ((nrCornerPoints - 1) + i) / nrPointsInCircle)),
                                offset + radius - (radius * std::sin(twoPi * ((nrCornerPoints - 1) + i) / nrPointsInCircle)));
        }

        // Bottom left corner
        for (unsigned int i = 0; i < nrCornerPoints; ++i)
        {
            points.emplace_back(offset + radius + (radius * std::cos(twoPi * (2*(nrCornerPoints - 1) + i) / nrPointsInCircle)),
                                offset + size.y - radius - (radius * std::sin(twoPi * (2*(nrCornerPoints - 1) + i) / nrPointsInCircle)));
        }

        // Bottom right corner
        for (unsigned int i = 0; i < nrCornerPoints; ++i)
        {
            points.emplace_back(offset + size.x - radius + (radius * std::cos(twoPi * (3*(nrCornerPoints - 1) + i) / nrPointsInCircle)),
                                offset + size.y - radius - (radius * std::sin(twoPi * (3*(nrCornerPoints - 1) + i) / nrPointsInCircle)));
        }

        return points;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static void drawBordersAroundShape(BackendRenderTarget* renderTarget, const RenderStates& states,
                                       const std::vector<Vector2f>& outerPoints, const std::vector<Vector2f>& innerPoints, const Color& color)
    {
        TGUI_ASSERT(outerPoints.size() == innerPoints.size(), "Inner and outer ring of cicle border should have the same amount of points");

        // Create the vertices
        std::vector<Vertex> vertices;
        vertices.reserve(outerPoints.size() + innerPoints.size());
        for (const auto& point : outerPoints)
            vertices.emplace_back(point, Vertex::Color(color));
        for (const auto& point : innerPoints)
            vertices.emplace_back(point, Vertex::Color(color));

        // Create the indices
        std::vector<unsigned int> indices;
        indices.reserve(3 * (outerPoints.size() + innerPoints.size()));
        for (std::size_t i = 0; i < outerPoints.size(); ++i)
        {
            indices.push_back(static_cast<unsigned int>(i));
            indices.push_back(static_cast<unsigned int>(i+1));
            indices.push_back(static_cast<unsigned int>(outerPoints.size() + i));

            indices.push_back(static_cast<unsigned int>(outerPoints.size() + i));
            indices.push_back(static_cast<unsigned int>(outerPoints.size() + i+1));
            indices.push_back(static_cast<unsigned int>(i+1));
        }

        // The last two triangles were given wrong indices by the loop (where there are "+1" in the code), and need to be overwitten to close the circle
        indices[indices.size() - 1] = 0;
        indices[indices.size() - 2] = static_cast<unsigned int>(outerPoints.size());
        indices[indices.size() - 5] = 0;

        // Draw the triangles
        renderTarget->drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static void drawInnerShape(BackendRenderTarget* renderTarget, const RenderStates& states, const std::vector<Vector2f>& points, const Vector2f& centerPoint, const Color& color)
    {
        // Create the vertices (one point in the middle of the circle and the others as provided in the 'points' parameter)
        std::vector<Vertex> vertices;
        vertices.reserve(1 + points.size());
        vertices.emplace_back(centerPoint, Vertex::Color(color));
        for (const auto& point : points)
            vertices.emplace_back(point, Vertex::Color(color));

        // Create the indices
        std::vector<unsigned int> indices;
        indices.reserve(3 * points.size());
        for (std::size_t i = 1; i <= points.size(); ++i)
        {
            indices.push_back(0); // Center point
            indices.push_back(static_cast<unsigned int>(i));
            indices.push_back(static_cast<unsigned int>(i+1));
        }
        indices.back() = 1; // Last index was one too far and should use the first point again, to close the circle

        // Draw the triangles
        renderTarget->drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::setView(FloatRect view, FloatRect viewport, Vector2f targetSize)
    {
        TGUI_ASSERT(m_clipLayers.empty(), "You can't change the view of the render target during drawing");

        m_viewRect = view;
        m_viewport = viewport;
        m_targetSize = targetSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::drawWidget(const RenderStates& states, const std::shared_ptr<Widget>& widget)
    {
        // If the widget lies outside of the clip rect then we can skip drawing it
        const FloatRect& clipRect = m_clipLayers.empty() ? m_viewRect : m_clipLayers.back().first;
        const FloatRect& widgetRect = states.transform.transformRect({widget->getWidgetOffset(), widget->getFullSize()});
        if ((widgetRect.left > clipRect.left + clipRect.width) || (widgetRect.top > clipRect.top + clipRect.height)
         || (widgetRect.left + widgetRect.width < clipRect.left) || (widgetRect.top + widgetRect.height < clipRect.top))
            return;

        // Round widget positions to the nearest pixel
        RenderStates statesWithRoundedPos = states;
        statesWithRoundedPos.transform.roundPosition(m_pixelsPerPoint.x, m_pixelsPerPoint.y);

        widget->draw(*this, statesWithRoundedPos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::addClippingLayer(const RenderStates& states, FloatRect rect)
    {
        TGUI_ASSERT(m_targetSize.x > 0 && m_targetSize.y > 0, "Target size must be valid in BackendRenderTarget::addClippingLayer");

        /// TODO: We currently can't clip rotated objects (except for 90°, 180° or 270° rotations)
        const std::array<float, 16>& transformMatrix = states.transform.getMatrix();
        if (((std::abs(transformMatrix[1]) > 0.00001f) || (std::abs(transformMatrix[4]) > 0.00001f)) // 0° or 180°
         && ((std::abs(transformMatrix[1] - 1) > 0.00001f) || (std::abs(transformMatrix[4] + 1) > 0.00001f)) // 90°
         && ((std::abs(transformMatrix[1] + 1) > 0.00001f) || (std::abs(transformMatrix[4] - 1) > 0.00001f))) // -90°
        {
            if (!m_clipLayers.empty())
                m_clipLayers.push_back(m_clipLayers.back());
            else
                m_clipLayers.emplace_back(m_viewRect, m_viewport);
            return;
        }

        const FloatRect& transformedRect = states.transform.transformRect(rect);
        const FloatRect oldClipRect = m_clipLayers.empty() ? m_viewRect : m_clipLayers.back().first;
        const float clipLeft = std::max(transformedRect.left, oldClipRect.left);
        const float clipTop = std::max(transformedRect.top, oldClipRect.top);
        const float clipRight = std::min(transformedRect.left + transformedRect.width, oldClipRect.left + oldClipRect.width);
        const float clipBottom = std::min(transformedRect.top + transformedRect.height, oldClipRect.top + oldClipRect.height);

        if ((clipRight - clipLeft > 0) && (clipBottom - clipTop > 0))
        {
            const FloatRect clipRect = {clipLeft, clipTop, clipRight - clipLeft, clipBottom - clipTop};
            const FloatRect clipViewport = {
                m_viewport.left + (((clipLeft - m_viewRect.left) / m_viewRect.width) * m_viewport.width),
                m_viewport.top + (((clipTop - m_viewRect.top) / m_viewRect.height) * m_viewport.height),
                m_viewport.width * ((clipRight - clipLeft) / m_viewRect.width),
                m_viewport.height * ((clipBottom - clipTop) / m_viewRect.height)
            };
            m_clipLayers.emplace_back(clipRect, clipViewport);
            updateClipping(clipRect, clipViewport);
        }
        else // Entire window is being clipped
        {
            m_clipLayers.emplace_back(FloatRect{}, FloatRect{});
            updateClipping({}, {});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::removeClippingLayer()
    {
        TGUI_ASSERT(!m_clipLayers.empty(), "BackendRenderTarget::removeClippingLayer can't remove layer if there are none left");

        m_clipLayers.pop_back();
        if (m_clipLayers.empty())
            updateClipping(m_viewRect, m_viewport);
        else
            updateClipping(m_clipLayers.back().first, m_clipLayers.back().second);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::drawBorders(const RenderStates& states, const Borders& borders, Vector2f size, Color color)
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
        const std::array<Vertex, 9> vertices = {{
            {{0, 0}, vertexColor},
            {{borders.getLeft(), 0}, vertexColor},
            {{0, size.y}, vertexColor},
            {{borders.getLeft(), size.y - borders.getBottom()}, vertexColor},
            {{size.x, size.y}, vertexColor},
            {{size.x - borders.getRight(), size.y - borders.getBottom()}, vertexColor},
            {{size.x, 0}, vertexColor},
            {{size.x - borders.getRight(), borders.getTop()}, vertexColor},
            {{borders.getLeft(), borders.getTop()}, vertexColor}
        }};
        const std::array<unsigned int, 8*3> indices = {{
            0, 2, 1,
            1, 2, 3,
            2, 4, 3,
            3, 4, 5,
            4, 6, 5,
            5, 6, 7,
            6, 1, 7,
            7, 1, 8
        }};
        drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::drawFilledRect(const RenderStates& states, Vector2f size, Color color)
    {
        const auto vertexColor = Vertex::Color(color);
        const std::array<Vertex, 4> vertices = {{
            {{0, 0}, vertexColor},
            {{0, size.y}, vertexColor},
            {{size.x, 0}, vertexColor},
            {{size.x, size.y}, vertexColor}
        }};
        const std::array<unsigned int, 2*3> indices = {{
            0, 1, 2,
            2, 1, 3
        }};
        drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::drawSprite(const RenderStates& states, const Sprite& sprite)
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

        std::shared_ptr<BackendTexture> backendTexture;
        if (sprite.getTexture().getData()->svgImage)
            backendTexture = sprite.getSvgTexture();
        else
            backendTexture = sprite.getTexture().getData()->backendTexture;

        const auto& vertices = sprite.getVertices();
        const auto& indices = sprite.getIndices();
        drawVertexArray(transformedStates, vertices.data(), vertices.size(), indices.data(), indices.size(), backendTexture);

        if (clippingRequired)
            removeClippingLayer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::drawText(const RenderStates& states, const Text& text)
    {
        RenderStates transformedStates = states;
        transformedStates.transform.translate(text.getPosition());

        // Round the text to the nearest pixel to try to avoid blurry text
        transformedStates.transform.roundPosition(m_pixelsPerPoint.x, m_pixelsPerPoint.y);

        auto vertexData = text.getBackendText()->getVertexData();

        for (const auto& data : vertexData)
        {
            const std::shared_ptr<BackendTexture>& texture = data.first;
            const std::shared_ptr<std::vector<Vertex>>& vertices = data.second;
            drawVertexArray(transformedStates, vertices->data(), vertices->size(), nullptr, 0, texture);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::drawTriangle(const RenderStates& states, const Vertex& point1, const Vertex& point2, const Vertex& point3)
    {
        const std::array<Vertex, 3> vertices = {{ point1, point2, point3 }};
        drawVertexArray(states, vertices.data(), vertices.size(), nullptr, 0, nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::drawCircle(const RenderStates& states, float size, const Color& backgroundColor, float borderThickness, const Color& borderColor)
    {
        const float radius = size / 2.f;
        const unsigned int nrPoints = static_cast<unsigned int>(std::ceil((radius + std::abs(borderThickness)) * 4));
        if (borderThickness > 0)
        {
            const std::vector<Vector2f>& outerPoints = drawCircleHelperGetPoints(nrPoints, radius + borderThickness, -borderThickness);
            const std::vector<Vector2f>& innerPoints = drawCircleHelperGetPoints(nrPoints, radius, 0);

            drawBordersAroundShape(this, states, outerPoints, innerPoints, borderColor);
            drawInnerShape(this, states, innerPoints, {radius, radius}, backgroundColor);
        }
        else if (borderThickness < 0)
        {
            const std::vector<Vector2f>& outerPoints = drawCircleHelperGetPoints(nrPoints, radius, 0);
            const std::vector<Vector2f>& innerPoints = drawCircleHelperGetPoints(nrPoints, radius + borderThickness, -borderThickness);

            drawBordersAroundShape(this, states, outerPoints, innerPoints, borderColor);
            drawInnerShape(this, states, innerPoints, {radius, radius}, backgroundColor);
        }
        else // No outline
        {
            const std::vector<Vector2f>& innerPoints = drawCircleHelperGetPoints(nrPoints, radius, 0);
            drawInnerShape(this, states, innerPoints, {radius, radius}, backgroundColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTarget::drawRoundedRectangle(const RenderStates& states, const Vector2f& size, const Color& backgroundColor,
                                                   float radius, const Borders& borders, const Color& borderColor)
    {
        // Radius can never be larger than half the width or height
        if (radius > size.x / 2)
            radius = size.x / 2;
        if (radius > size.y / 2)
            radius = size.y / 2;

        const unsigned int nrCornerPoints = std::max(1u, static_cast<unsigned int>(std::ceil(radius * 2)));
        const std::vector<Vector2f>& outerPoints = drawRoundedRectHelperGetPoints(nrCornerPoints, size, radius, 0);

        const float borderWidth = borders.getLeft();
        if (borderWidth > 0)
        {
            radius = std::max(0.f, radius - borderWidth);
            const Vector2f innerSize = {std::max(0.f, size.x - 2*borderWidth), std::max(0.f, size.y - 2*borderWidth)};
            if (radius > innerSize.x / 2)
                radius = innerSize.x / 2;
            if (radius > innerSize.y / 2)
                radius = innerSize.y / 2;

            const std::vector<Vector2f>& innerPoints = drawRoundedRectHelperGetPoints(nrCornerPoints, innerSize, radius, borderWidth);

            drawBordersAroundShape(this, states, outerPoints, innerPoints, borderColor);
            drawInnerShape(this, states, innerPoints, size/2.f, backgroundColor);
        }
        else // There are no borders
            drawInnerShape(this, states, outerPoints, size/2.f, backgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
