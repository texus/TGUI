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


#include <TGUI/Widgets/Canvas.hpp>

#if TGUI_BUILD_WITH_SFML
#include <TGUI/Backends/SFML/BackendTextureSFML.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(const Canvas& other) :
        ClickableWidget{other}
    {
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(Canvas&& other) :
        ClickableWidget{std::move(other)}
    {
        // sf::RenderTexture does not support move yet
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas& Canvas::operator= (const Canvas& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas& Canvas::operator= (Canvas&& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(std::move(right));

            // sf::RenderTexture does not support move yet
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Ptr Canvas::create(Layout2d size)
    {
        auto canvas = std::make_shared<Canvas>();
        canvas->setSize(size);
        return canvas;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Ptr Canvas::copy(Canvas::ConstPtr canvas)
    {
        if (canvas)
            return std::static_pointer_cast<Canvas>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::setSize(const Layout2d& size)
    {
        Vector2f newSize = size.getValue();

        if ((newSize.x > 0) && (newSize.y > 0))
        {
            if ((m_renderTexture.getSize().x < static_cast<unsigned int>(newSize.x)) || (m_renderTexture.getSize().y < static_cast<unsigned int>(newSize.y)))
                m_renderTexture.create(static_cast<unsigned int>(newSize.x), static_cast<unsigned int>(newSize.y));

            m_sprite.setSize(newSize);
            clear();
            display();
        }

        Widget::setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::setView(const sf::View& view)
    {
        m_renderTexture.setView(view);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& Canvas::getView() const
    {
        return m_renderTexture.getView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& Canvas::getDefaultView() const
    {
        return m_renderTexture.getDefaultView();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    IntRect Canvas::getViewport() const
    {
        return IntRect(m_renderTexture.getViewport(m_renderTexture.getView()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::clear(Color color)
    {
        m_renderTexture.clear(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(const sf::Drawable& drawable, const sf::RenderStates& states)
    {
        m_renderTexture.draw(drawable, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states)
    {
        m_renderTexture.draw(vertices, vertexCount, type, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(const tgui::Sprite& sprite, const RenderStates& states)
    {
        const std::vector<Vertex>& vertices = sprite.getVertices();
        const std::vector<int>& indices = sprite.getIndices();
        std::vector<Vertex> triangleVertices(indices.size());
        for (unsigned int i = 0; i < indices.size(); ++i)
            triangleVertices[i] = vertices[indices[i]];

        sf::RenderStates statesSFML;
        const float *transformMatrix = states.transform.getMatrix();
        statesSFML.transform = sf::Transform(
            transformMatrix[0], transformMatrix[4], transformMatrix[12],
            transformMatrix[1], transformMatrix[5], transformMatrix[13],
            transformMatrix[3], transformMatrix[7], transformMatrix[15]);

        TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture),
                    "Canvas::draw requires sprite to have a backend texture of type BackendTextureSFML");
        statesSFML.texture = &std::static_pointer_cast<BackendTextureSFML>(sprite.getTexture().getData()->backendTexture)->getInternalTexture();

        static_assert(sizeof(Vertex) == sizeof(sf::Vertex), "Size of sf::Vertex has to match with tgui::Vertex for optimization to work");
        const sf::Vertex* sfmlVertices = reinterpret_cast<const sf::Vertex*>(triangleVertices.data());
        m_renderTexture.draw(sfmlVertices, indices.size(), sf::PrimitiveType::Triangles, statesSFML);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::display()
    {
        m_renderTexture.display();

        const Vector2f& size = getSize();
        const sf::Texture& texture = m_renderTexture.getTexture();
        m_sprite.setTexture({texture, {0, 0, static_cast<unsigned int>(std::max(0.f, size.x)), static_cast<unsigned int>(std::max(0.f, size.y))}});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::rendererChanged(const String& property)
    {
        Widget::rendererChanged(property);

        if ((property == "Opacity") || (property == "OpacityDisabled"))
            m_sprite.setOpacity(m_opacityCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(BackendRenderTargetBase& target, RenderStates states) const
    {
        if ((getSize().x <= 0) || (getSize().y <= 0))
            return;

        target.drawSprite(states, m_sprite);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Canvas::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif // TGUI_BUILD_WITH_SFML

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
