/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Renderer/SFML-Graphics/CanvasSFML.hpp>
#include <TGUI/Backend/Renderer/SFML-Graphics/BackendTextureSFML.hpp>
#include <TGUI/Backend/Renderer/SFML-Graphics/BackendRenderTargetSFML.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <array>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char CanvasSFML::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML::CanvasSFML(const char* typeName, bool initRenderer) :
        CanvasBase{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML::CanvasSFML(const CanvasSFML& other) :
        CanvasBase  {other},
        m_customView{other.m_customView}
    {
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML::CanvasSFML(CanvasSFML&& other) noexcept :
        CanvasBase     {std::move(other)},
#if SFML_VERSION_MAJOR >= 3
        m_renderTexture{std::move(other.m_renderTexture)},
#endif
        m_customView   {std::move(other.m_customView)}
    {
#if SFML_VERSION_MAJOR < 3
        setSize(getSize()); // sf::RenderTexture did not support move yet
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML& CanvasSFML::operator= (const CanvasSFML& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            m_customView = right.m_customView;
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML& CanvasSFML::operator= (CanvasSFML&& right) noexcept
    {
        if (this != &right)
        {
            ClickableWidget::operator=(std::move(right));
            m_customView = std::move(right.m_customView);

#if SFML_VERSION_MAJOR >= 3
            m_renderTexture = std::move(right.m_renderTexture);
#else
            setSize(getSize()); // sf::RenderTexture did not support move yet
#endif
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML::Ptr CanvasSFML::create(const Layout2d& size)
    {
        auto canvas = std::make_shared<CanvasSFML>();
        canvas->setSize(size);
        return canvas;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML::Ptr CanvasSFML::copy(const CanvasSFML::ConstPtr& canvas)
    {
        if (canvas)
            return std::static_pointer_cast<CanvasSFML>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::setSize(const Layout2d& size)
    {
        Widget::setSize(size);
        const Vector2f newSize = getSize();

        if ((newSize.x > 0) && (newSize.y > 0))
        {
            const Vector2u newTextureSize{newSize};
            if ((m_renderTexture.getSize().x != newTextureSize.x) || (m_renderTexture.getSize().y != newTextureSize.y))
            {
#if SFML_VERSION_MAJOR >= 3
                (void)m_renderTexture.resize({newTextureSize.x, newTextureSize.y});
#else
                m_renderTexture.create(newTextureSize.x, newTextureSize.y);
#endif
            }
        }

        if (m_customView.has_value())
            m_renderTexture.setView(m_customView.value());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::setView(const sf::View& view)
    {
        m_customView = view;
        m_renderTexture.setView(view);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& CanvasSFML::getView() const
    {
        return m_renderTexture.getView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::resetView()
    {
        m_customView.reset();
        m_renderTexture.setView(getDefaultView());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::View CanvasSFML::getDefaultView() const
    {
        return m_renderTexture.getDefaultView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    IntRect CanvasSFML::getViewport() const
    {
        const sf::IntRect rect = m_renderTexture.getViewport(m_renderTexture.getView());
#if SFML_VERSION_MAJOR >= 3
        return {rect.position.x, rect.position.y, rect.size.x, rect.size.y};
#else
        return {rect.left, rect.top, rect.width, rect.height};
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CanvasSFML::mapPixelToCoords(Vector2f point) const
    {
        const Vector2f size = getSize();
        const sf::View& view = m_renderTexture.getView();
        const sf::FloatRect& viewport = view.getViewport();

        const sf::Vector2f normalized = {
#if SFML_VERSION_MAJOR >= 3
            -1 + (2 * (point.x - (viewport.position.x * size.x)) / (viewport.size.x * size.x)),
            1 + (-2 * (point.y - (viewport.position.y * size.y)) / (viewport.size.y * size.y))
#else
            -1 + (2 * (point.x - (viewport.left * size.x)) / (viewport.width * size.x)),
            1 + (-2 * (point.y - (viewport.top * size.y)) / (viewport.height * size.y))
#endif
        };
        const sf::Vector2f coord = view.getInverseTransform().transformPoint(normalized);
        return {coord.x, coord.y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD Vector2f CanvasSFML::mapCoordsToPixel(Vector2f coord) const
    {
        const Vector2f size = getSize();
        const sf::View& view = m_renderTexture.getView();
        const sf::FloatRect& viewport = view.getViewport();

        const sf::Vector2f normalized = view.getTransform().transformPoint(coord);
#if SFML_VERSION_MAJOR >= 3
        return {(normalized.x + 1) / 2 * (viewport.size.x * size.x) + (viewport.position.x * size.x),
                (-normalized.y + 1) / 2 * (viewport.size.y * size.y) + (viewport.position.y * size.y)};
#else
        return {(normalized.x + 1) / 2 * (viewport.width * size.x) + (viewport.left * size.x),
                (-normalized.y + 1) / 2 * (viewport.height * size.y) + (viewport.top * size.y)};
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::setSmooth(bool smooth)
    {
        m_renderTexture.setSmooth(smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CanvasSFML::isSmooth() const
    {
        return m_renderTexture.isSmooth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::clear(Color color)
    {
        m_renderTexture.clear({color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::draw(const sf::Drawable& drawable, const sf::RenderStates& states)
    {
        m_renderTexture.draw(drawable, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states)
    {
        m_renderTexture.draw(vertices, vertexCount, type, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::draw(const Sprite& sprite, const RenderStates& states)
    {
        // If the sprite is empty then don't try to draw it
        if (!sprite.getTexture().getData())
            return;

        sf::RenderStates statesSFML;
        const std::array<float, 16>& transformMatrix = states.transform.getMatrix();
        statesSFML.transform = sf::Transform(
            transformMatrix[0], transformMatrix[4], transformMatrix[12],
            transformMatrix[1], transformMatrix[5], transformMatrix[13],
            transformMatrix[3], transformMatrix[7], transformMatrix[15]);
        statesSFML.transform.translate({sprite.getPosition().x, sprite.getPosition().y});

        TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture),
                    "CanvasSFML::draw requires sprite to have a backend texture of type BackendTextureSFML");
        statesSFML.texture = std::static_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture)->getInternalTexture();

#if SFML_VERSION_MAJOR >= 3
        statesSFML.coordinateType = sf::CoordinateType::Normalized;
#else
        const sf::Vector2u textureSize{statesSFML.texture->getSize()};
#endif

        const std::vector<Vertex>& vertices = sprite.getVertices();
        const std::vector<unsigned int>& indices = sprite.getIndices();
        auto triangleVertices = MakeUniqueForOverwrite<Vertex[]>(indices.size());
        for (std::size_t i = 0; i < indices.size(); ++i)
        {
            triangleVertices[i] = vertices[indices[i]];
#if SFML_VERSION_MAJOR < 3
            triangleVertices[i].texCoords.x *= textureSize.x;
            triangleVertices[i].texCoords.y *= textureSize.y;
#endif
        }

        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");
        const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(triangleVertices.get());
        m_renderTexture.draw(sfmlVertices, indices.size(), sf::PrimitiveType::Triangles, statesSFML);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::display()
    {
        m_renderTexture.display();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::draw(BackendRenderTarget& target, RenderStates states) const
    {
        TGUI_ASSERT(dynamic_cast<BackendRenderTargetSFML*>(&target), "CanvasSFML requires a render target of type BackendRenderTargetSFML");

        const Vector2f size = getSize();
        const sf::Texture& texture = m_renderTexture.getTexture();
        const Vector2f textureSize = Vector2f{static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y)};
        if ((size.x <= 0) || (size.y <= 0) || (textureSize.x == 0) || (textureSize.y == 0))
            return;

        const std::array<float, 16>& transformMatrix = states.transform.getMatrix();
        const Vertex::Color vertexColor(Color::applyOpacity(Color::White, m_opacityCached));

        sf::RenderStates statesSFML;
        statesSFML.texture = &texture;
        statesSFML.transform = sf::Transform(
            transformMatrix[0], transformMatrix[4], transformMatrix[12],
            transformMatrix[1], transformMatrix[5], transformMatrix[13],
            transformMatrix[3], transformMatrix[7], transformMatrix[15]);

#if SFML_VERSION_MAJOR >= 3
        statesSFML.coordinateType = sf::CoordinateType::Normalized;

        // We use textureSize instead of size for the vertices coordinates to keep rendering stable when the size is changing and
        // the width and height aren't integer values.
        const sf::Color vertexColorSFML{vertexColor.red, vertexColor.green, vertexColor.blue, vertexColor.alpha};
        const std::array<sf::Vertex, 6> verticesSFML = {{
            {{0, 0}, vertexColorSFML, {0, 0}},
            {{0, textureSize.y}, vertexColorSFML, {0, 1}},
            {{textureSize.x, 0}, vertexColorSFML, {1, 0}},
            {{textureSize.x, 0}, vertexColorSFML, {1, 0}},
            {{0, textureSize.y}, vertexColorSFML, {0, 1}},
            {{textureSize.x, textureSize.y}, vertexColorSFML, {1, 1}},
        }};

        static_cast<BackendRenderTargetSFML&>(target).getTarget()->draw(verticesSFML.data(), verticesSFML.size(), sf::PrimitiveType::Triangles, statesSFML);
#else
        // We use textureSize instead of size for the vertices coordinates to keep rendering stable when the size is changing and
        // the width and height aren't integer values.
        const std::array<Vertex, 4> vertices = {{
            {{0, 0}, vertexColor, {0, 0}},
            {{textureSize.x, 0}, vertexColor, {1, 0}},
            {{0, textureSize.y}, vertexColor, {0, 1}},
            {{textureSize.x, textureSize.y}, vertexColor, {1, 1}},
        }};
        const std::array<unsigned int, 6> indices = {{
            0, 2, 1,
            1, 2, 3
        }};

        // Creating an sf::Vertex costs time because its constructor can't be inlined. Since our own Vertex struct has an identical memory layout,
        // we will create an array of our own Vertex objects and then use a reinterpret_cast to turn them into sf::Vertex.
        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");

        auto verticesSFML = MakeUniqueForOverwrite<Vertex[]>(indices.size());
        for (std::size_t i = 0; i < indices.size(); ++i)
        {
            verticesSFML[i].position.x = vertices[indices[i]].position.x;
            verticesSFML[i].position.y = vertices[indices[i]].position.y;
            verticesSFML[i].color.red = vertices[indices[i]].color.red;
            verticesSFML[i].color.green = vertices[indices[i]].color.green;
            verticesSFML[i].color.blue = vertices[indices[i]].color.blue;
            verticesSFML[i].color.alpha = vertices[indices[i]].color.alpha;
            verticesSFML[i].texCoords.x = vertices[indices[i]].texCoords.x * textureSize.x;
            verticesSFML[i].texCoords.y = vertices[indices[i]].texCoords.y * textureSize.y;
        }

        static_cast<BackendRenderTargetSFML&>(target).getTarget()->draw(reinterpret_cast<const sf::Vertex*>(verticesSFML.get()), indices.size(), sf::PrimitiveType::Triangles, statesSFML);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CanvasSFML::clone() const
    {
        return std::make_shared<CanvasSFML>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
