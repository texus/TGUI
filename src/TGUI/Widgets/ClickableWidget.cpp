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


#include <TGUI/Widgets/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::ClickableWidget(const Layout2d& size)
    {
        m_type = "ClickableWidget";
        m_callback.widgetType = "ClickableWidget";

        addSignal<sf::Vector2f>("MousePressed");
        addSignal<sf::Vector2f>("MouseReleased");
        addSignal<sf::Vector2f>("Clicked");

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::ClickableWidget(const Layout& width, const Layout& height) :
        ClickableWidget{Layout2d{width, height}}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::Ptr ClickableWidget::copy(ClickableWidget::ConstPtr widget)
    {
        if (widget)
            return std::static_pointer_cast<ClickableWidget>(widget->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ClickableWidget::mouseOnWidget(sf::Vector2f pos) const
    {
        return sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMousePressed(sf::Vector2f pos)
    {
        m_mouseDown = true; /// TODO: Is there any widget for which this can't be in Widget base class?

        m_callback.mouse.x = static_cast<int>(pos.x);
        m_callback.mouse.y = static_cast<int>(pos.y);
        sendSignal("MousePressed", pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMouseReleased(sf::Vector2f pos)
    {
        m_callback.mouse.x = static_cast<int>(pos.x);
        m_callback.mouse.y = static_cast<int>(pos.y);
        sendSignal("MouseReleased", pos);

        if (m_mouseDown)
            sendSignal("Clicked", pos);

        m_mouseDown = false; /// TODO: Is there any widget for which this can't be in Widget base class?
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::draw(sf::RenderTarget&, sf::RenderStates) const
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
