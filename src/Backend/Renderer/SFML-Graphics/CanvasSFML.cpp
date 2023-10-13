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
        ClickableWidget{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML::CanvasSFML(const CanvasSFML& other) :
        ClickableWidget  {other},
        m_usedTextureSize{other.m_usedTextureSize}
    {
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML::CanvasSFML(CanvasSFML&& other) noexcept :
        ClickableWidget  {std::move(other)},
        m_usedTextureSize{std::move(other.m_usedTextureSize)}
    {
        // sf::RenderTexture does not support move yet
        setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML& CanvasSFML::operator= (const CanvasSFML& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            m_usedTextureSize = right.m_usedTextureSize;
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSFML& CanvasSFML::operator= (CanvasSFML&& right) noexcept
    {
        if (this != &right)
        {
            m_usedTextureSize = std::move(right.m_usedTextureSize);
            ClickableWidget::operator=(std::move(right));

            // sf::RenderTexture does not support move yet
            setSize(getSize());
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
            if ((m_renderTexture.getSize().x < newTextureSize.x) || (m_renderTexture.getSize().y < newTextureSize.y))
#if SFML_VERSION_MAJOR >= 3
                (void)m_renderTexture.create({newTextureSize.x, newTextureSize.y});
#else
                m_renderTexture.create(newTextureSize.x, newTextureSize.y);
#endif

            m_usedTextureSize = newTextureSize;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSFML::setView(const sf::View& view)
    {
        m_renderTexture.setView(view);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& CanvasSFML::getView() const
    {
        return m_renderTexture.getView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& CanvasSFML::getDefaultView() const
    {
        return m_renderTexture.getDefaultView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    IntRect CanvasSFML::getViewport() const
    {
        const sf::IntRect rect = m_renderTexture.getViewport(m_renderTexture.getView());
        return {rect.left, rect.top, rect.width, rect.height};
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
        statesSFML.texture = &std::static_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture)->getInternalTexture();

        const sf::Vector2u textureSize{statesSFML.texture->getSize()};
        const std::vector<Vertex>& vertices = sprite.getVertices();
        const std::vector<unsigned int>& indices = sprite.getIndices();
        std::vector<Vertex> triangleVertices(indices.size());
        for (std::size_t i = 0; i < indices.size(); ++i)
        {
            triangleVertices[i] = vertices[indices[i]];
            triangleVertices[i].texCoords.x *= textureSize.x;
            triangleVertices[i].texCoords.y *= textureSize.y;
        }

        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");
        const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(triangleVertices.data());
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
        const Vector2f size = getSize();
        if ((size.x <= 0) || (size.y <= 0) || (m_usedTextureSize.x == 0) || (m_usedTextureSize.y == 0))
            return;

        const Vector2f normalizedTextureSize{static_cast<float>(m_usedTextureSize.x) / static_cast<float>(m_renderTexture.getSize().x),
                                             static_cast<float>(m_usedTextureSize.y) / static_cast<float>(m_renderTexture.getSize().y)};
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

        // Creating an sf::Vertex costs time because its constructor can't be inlined. Since our own Vertex struct has an identical memory layout,
        // we will create an array of our own Vertex objects and then use a reinterpret_cast to turn them into sf::Vertex.
        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");

        const sf::Texture& texture = m_renderTexture.getTexture();
        const Vector2f textureSize = Vector2f{static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y)};
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

        sf::RenderStates statesSFML;
        const std::array<float, 16>& transformMatrix = states.transform.getMatrix();
        statesSFML.texture = &texture;
        statesSFML.transform = sf::Transform(
            transformMatrix[0], transformMatrix[4], transformMatrix[12],
            transformMatrix[1], transformMatrix[5], transformMatrix[13],
            transformMatrix[3], transformMatrix[7], transformMatrix[15]);

        TGUI_ASSERT(dynamic_cast<BackendRenderTargetSFML*>(&target), "CanvasSFML requires a render target of type BackendRenderTargetSFML");
        static_cast<BackendRenderTargetSFML&>(target).getTarget()->draw(reinterpret_cast<const sf::Vertex*>(verticesSFML.get()), indices.size(), sf::PrimitiveType::Triangles, statesSFML);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CanvasSFML::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CanvasSFML::clone() const
    {
        return std::make_shared<CanvasSFML>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
