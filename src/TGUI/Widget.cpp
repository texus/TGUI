/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/ToolTip.hpp>
#include <TGUI/Container.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget()
    {
        m_callback.widget = this;

        addSignal<sf::Vector2f>("PositionChanged");
        addSignal<sf::Vector2f>("SizeChanged");
        addSignal("Focused");
        addSignal("Unfocused");
        addSignal("MouseEntered");
        addSignal("MouseLeft");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::~Widget()
    {
        detachTheme();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& copy) :
        sf::Drawable     {copy},
        Transformable    {copy},
        SignalWidgetBase {copy},
        enable_shared_from_this<Widget>{copy},
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
        m_tooltip        {ToolTip::copy(copy.m_tooltip)},
        m_font           {copy.m_font}
    {
        m_callback.widget = this;

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
            m_tooltip         = ToolTip::copy(right.m_tooltip);
            m_font            = right.m_font;
            m_callback.widget = this;

            if (right.m_renderer != nullptr)
                m_renderer = right.m_renderer->clone(this);
            else
                m_renderer = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout2d& position)
    {
        if (position.x.getImpl()->parentWidget != this)
        {
            position.x.getImpl()->parentWidget = this;
            position.x.getImpl()->recalculate();
        }
        if (position.y.getImpl()->parentWidget != this)
        {
            position.y.getImpl()->parentWidget = this;
            position.y.getImpl()->recalculate();
        }

        Transformable::setPosition(position);

        m_callback.position = getPosition();
        sendSignal("PositionChanged", getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout2d& size)
    {
        if (size.x.getImpl()->parentWidget != this)
        {
            size.x.getImpl()->parentWidget = this;
            size.x.getImpl()->recalculate();
        }
        if (size.y.getImpl()->parentWidget != this)
        {
            size.y.getImpl()->parentWidget = this;
            size.y.getImpl()->recalculate();
        }

        Transformable::setSize(size);

        m_callback.size = getSize();
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

    void Widget::setToolTip(ToolTip::Ptr tooltip)
    {
        sf::String oldToolTipText;
        if (m_tooltip)
            oldToolTipText = getToolTip()->getText();

        m_tooltip = tooltip;

        if ((oldToolTipText != "") && (tooltip->getText() == ""))
            m_tooltip->setText(oldToolTipText);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ToolTip::Ptr Widget::getToolTip()
    {
        if (!m_tooltip)
            m_tooltip = std::make_shared<ToolTip>();

        return m_tooltip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::detachTheme()
    {
        if (m_theme)
        {
            m_theme->widgetDetached(this);
            m_theme = nullptr;
        }
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

    ToolTip::Ptr Widget::askToolTip(sf::Vector2f mousePos)
    {
        if (m_tooltip && mouseOnWidget(mousePos.x, mousePos.y) && getToolTip()->getText() != "")
            return getToolTip();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::initialize(Container *const parent)
    {
        assert(parent);

        m_parent = parent;

        m_position.x.getImpl()->recalculate();
        m_position.y.getImpl()->recalculate();
        m_size.x.getImpl()->recalculate();
        m_size.y.getImpl()->recalculate();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::attachTheme(std::shared_ptr<BaseTheme> theme)
    {
        detachTheme();
        m_theme = theme;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::reload(const std::string& primary, const std::string& secondary, bool)
    {
        m_primaryLoadingParameter = primary;
        m_secondaryLoadingParameter = secondary;

        if (m_theme && primary != "")
            m_theme->initWidget(this, primary, secondary);
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setProperty(std::string property, const std::string&)
    {
        /// TODO: Font in every widget
        throw Exception{"Could not set property '" + property + "', widget does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setProperty(std::string property, ObjectConverter&&)
    {
        /// TODO: Font in every widget
        /**
        if (value.getType() == ObjectConverter::Type::Font)
        {
            if (property == "font")
                setTextFont(value.getFont());
            else
                return false;
        }
        */
        throw Exception{"Could not set property '" + property + "', widget does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter WidgetRenderer::getProperty(std::string) const
    {
        /// TODO: Font in every widget
        return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> WidgetRenderer::getPropertyValuePairs() const
    {
        /// TODO: Font in every widget
        return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
