/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Layout2d& size)
    {
        m_callback.widgetType = "Panel";
        m_type = "Panel";

        addSignal<sf::Vector2f>("MousePressed");
        addSignal<sf::Vector2f>("MouseReleased");
        addSignal<sf::Vector2f>("Clicked");

        m_renderer = aurora::makeCopied<PanelRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorderColor({220, 220, 220});

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::create(Layout2d size)
    {
        return std::make_shared<Panel>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::copy(Panel::ConstPtr panel)
    {
        if (panel)
            return std::static_pointer_cast<Panel>(panel->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Panel::getChildWidgetsOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnWidget(sf::Vector2f pos) const
    {
        return sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMousePressed(sf::Vector2f pos)
    {
        m_mouseDown = true;

        m_callback.mouse.x = static_cast<int>(pos.x);
        m_callback.mouse.y = static_cast<int>(pos.y);
        sendSignal("MousePressed", pos);

        Container::leftMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMouseReleased(sf::Vector2f pos)
    {
        m_callback.mouse.x = static_cast<int>(pos.x);
        m_callback.mouse.y = static_cast<int>(pos.y);
        sendSignal("MouseReleased", pos);

        if (m_mouseDown)
            sendSignal("Clicked", pos);

        m_mouseDown = false;

        Container::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if ((property != "borders") && (property != "bordercolor") && (property != "backgroundcolor"))
            Container::rendererChanged(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());
            states.transform.translate({borders.left, borders.top});
        }

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        sf::Vector2f innerSize = {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
        Clipping clipping{target, states, {borders.left, borders.top}, innerSize};

        // Draw the background
        if (getRenderer()->getBackgroundColor() != sf::Color::Transparent)
            drawRectangleShape(target, states, innerSize, getRenderer()->getBackgroundColor());

        // Draw the child widgets
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
