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
#include <TGUI/Tooltip.hpp>
#include <TGUI/Container.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget()
    {
        addSignal<SignalVector2f>("PositionChanged");
        addSignal<SignalVector2f>("SizeChanged");
        addSignal("Focused");
        addSignal("Unfocused");
        addSignal("MouseEntered");
        addSignal("MouseLeft");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& copy) :
        sf::Drawable     {copy},
        Transformable    {copy},
        SignalWidgetBase {copy},
        enable_shared_from_this<Widget>{copy},
        m_widgetType     {copy.m_widgetType},
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
        m_containerWidget{copy.m_containerWidget},
        m_tooltip        {Tooltip::copy(copy.m_tooltip)},
        m_font           {copy.m_font}
    {
        if (copy.m_renderer != nullptr)
            m_renderer = copy.m_renderer->clone(this);
        else
            m_renderer = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator= (const Widget& right)
    {
        if (this != &right)
        {
            sf::Drawable::operator=(right);
            Transformable::operator=(right);
            SignalWidgetBase::operator=(right);
            enable_shared_from_this::operator=(right);

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
            m_tooltip         = Tooltip::copy(right.m_tooltip);
            m_font            = right.m_font;

            if (right.m_renderer != nullptr)
                m_renderer = right.m_renderer->clone(this);
            else
                m_renderer = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout& position)
    {
        Transformable::setPosition(position);
        sendSignal("PositionChanged", getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout& size)
    {
        Transformable::setSize(size);
        sendSignal("SizeChanged", getSize());
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

    void Widget::setTooltip(Tooltip::Ptr tooltip)
    {
        m_tooltip = tooltip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tooltip::Ptr Widget::getTooltip()
    {
        if (!m_tooltip)
            m_tooltip = Tooltip::create();

        return m_tooltip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::update()
    {
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
        if (!m_mouseHover)
            mouseEnteredWidget();
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
        sendSignal("Focused");

        // Make sure the parent is also focused
        if (m_parent)
            m_parent->focus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetUnfocused()
    {
        sendSignal("Unfocused");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNotOnWidget()
    {
        if (m_mouseHover == true)
            mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tooltip::Ptr Widget::askTooltip(sf::Vector2f mousePos)
    {
        if (mouseOnWidget(mousePos.x, mousePos.y) && getTooltip()->getText() != "")
            return getTooltip();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::initialize(Container *const parent)
    {
        assert(parent);

        m_parent = parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        m_mouseHover = true;
        sendSignal("MouseEntered");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        m_mouseHover = false;
        sendSignal("MouseLeft");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
