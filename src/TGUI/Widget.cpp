/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widget.hpp>
#include <TGUI/Container.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& copy) :
        sf::Drawable     {copy},
        Transformable    {copy},
        CallbackManager  {copy},
        m_enabled        {copy.m_enabled},
        m_visible        {copy.m_visible},
        m_parent         {copy.m_parent},
        m_opacity        {copy.m_opacity},
        m_mouseHover     {false},
        m_mouseDown      {false},
        m_focused        {false},
        m_allowFocus     {copy.m_allowFocus},
        m_animatedWidget {copy.m_animatedWidget},
        m_draggableWidget{copy.m_draggableWidget},
        m_containerWidget{copy.m_containerWidget}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator= (const Widget& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            sf::Drawable::operator=(right);
            Transformable::operator=(right);
            CallbackManager::operator=(right);

            m_enabled         = right.m_enabled;
            m_visible         = right.m_visible;
            m_parent          = right.m_parent;
            m_opacity         = right.m_opacity;
            m_mouseHover      = false;
            m_mouseDown       = false;
            m_focused         = false;
            m_allowFocus      = right.m_allowFocus;
            m_animatedWidget  = right.m_animatedWidget;
            m_draggableWidget = right.m_draggableWidget;
            m_containerWidget = right.m_containerWidget;
            m_callback        = Callback();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout& position)
    {
        Transformable::setPosition(position);

        if (m_callbackFunctions[Widget::PositionChanged].empty() == false)
        {
            m_callback.trigger  = Widget::PositionChanged;
            m_callback.position = position.getValue();
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout& size)
    {
        Transformable::setSize(size);

        if (m_callbackFunctions[Widget::SizeChanged].empty() == false)
        {
            m_callback.trigger = Widget::SizeChanged;
            m_callback.size    = size.getValue();
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Widget::getAbsolutePosition() const
    {
        if (m_parent)
            return m_parent->getAbsolutePosition() + m_parent->getWidgetsOffset() + getPosition();
        else
            return getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::show()
    {
        m_visible = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::hide()
    {
        m_visible = false;

        // If the widget is focused then it must be unfocused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::enable()
    {
        m_enabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::disable()
    {
        m_enabled = false;

        // Change the mouse button state.
        m_mouseHover = false;
        m_mouseDown = false;

        // If the widget is focused then it must be unfocused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::focus()
    {
        if (m_parent)
            m_parent->focusWidget(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unfocus()
    {
        if (m_focused)
            m_parent->unfocusWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToFront()
    {
        m_parent->moveWidgetToFront(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToBack()
    {
        m_parent->moveWidgetToBack(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::update()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        if (m_callbackFunctions[MouseEntered].empty() == false)
        {
            m_callback.trigger = MouseEntered;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        if (m_callbackFunctions[MouseLeft].empty() == false)
        {
            m_callback.trigger = MouseLeft;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMousePressed(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseReleased(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseMoved(float, float)
    {
        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::keyPressed(const sf::Event::KeyEvent&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::textEntered(sf::Uint32)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseWheelMoved(int, int, int)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetFocused()
    {
        if (m_callbackFunctions[Focused].empty() == false)
        {
            m_callback.trigger = Focused;
            addCallback();
        }

        // Make sure the parent is also focused
        if (m_parent)
            m_parent->focus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetUnfocused()
    {
        if (m_callbackFunctions[Unfocused].empty() == false)
        {
            m_callback.trigger = Unfocused;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNotOnWidget()
    {
        if (m_mouseHover == true)
            mouseLeftWidget();

        m_mouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::initialize(Container *const parent)
    {
        assert(parent);

        m_parent = parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::addCallback()
    {
        // Loop through all callback functions
        auto& functions = m_callbackFunctions[m_callback.trigger];
        for (auto func = functions.cbegin(); func != functions.cend(); ++func)
        {
            // Pass the callback to the correct place
            if (*func != nullptr)
                (*func)();
            else
                m_parent->addChildCallback(m_callback);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
