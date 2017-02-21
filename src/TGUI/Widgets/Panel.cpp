/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"bordercolor", sf::Color::Black},
                {"backgroundcolor", sf::Color::Transparent}
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Layout2d& size)
    {
        m_callback.widgetType = "Panel";
        m_type = "Panel";

        addSignal<sf::Vector2f>("MousePressed");
        addSignal<sf::Vector2f>("MouseReleased");
        addSignal<sf::Vector2f>("Clicked");

        m_renderer = aurora::makeCopied<PanelRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));

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
        return {m_paddingCached.left + m_bordersCached.left, m_paddingCached.top + m_bordersCached.top};
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

    void Panel::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else
            Group::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.left, m_bordersCached.top});
        }

        // Draw the background
        sf::Vector2f innerSize = {getSize().x - m_bordersCached.left - m_bordersCached.right, getSize().y - m_bordersCached.top - m_bordersCached.bottom};
        drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        states.transform.translate(m_paddingCached.left, m_paddingCached.top);
        innerSize.x -= m_paddingCached.left + m_paddingCached.right;
        innerSize.y -= m_paddingCached.top + m_paddingCached.bottom;

        // Draw the child widgets
        Clipping clipping{target, states, {}, innerSize};
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
