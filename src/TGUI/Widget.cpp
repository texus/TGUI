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
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/Container.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& copy) :
        sf::Drawable     (copy),
        Transformable    (copy),
        CallbackManager  (copy),
        m_enabled        (copy.m_enabled),
        m_visible        (copy.m_visible),
        m_parent         (copy.m_parent),
        m_opacity        (copy.m_opacity),
        m_mouseHover     (false),
        m_mouseDown      (false),
        m_focused        (false),
        m_allowFocus     (copy.m_allowFocus),
        m_animatedWidget (copy.m_animatedWidget),
        m_draggableWidget(copy.m_draggableWidget),
        m_containerWidget(copy.m_containerWidget)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator= (const Widget& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            this->sf::Drawable::operator=(right);
            this->Transformable::operator=(right);
            this->CallbackManager::operator=(right);

            m_enabled             = right.m_enabled;
            m_visible             = right.m_visible;
            m_parent              = right.m_parent;
            m_opacity             = right.m_opacity;
            m_mouseHover          = false;
            m_mouseDown           = false;
            m_focused             = false;
            m_allowFocus          = right.m_allowFocus;
            m_animatedWidget      = right.m_animatedWidget;
            m_draggableWidget     = right.m_draggableWidget;
            m_containerWidget     = right.m_containerWidget;
            m_callback            = Callback();
        }

        return *this;
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

    bool Widget::isVisible() const
    {
        return m_visible;
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

    bool Widget::isEnabled() const
    {
        return m_enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isDisabled() const
    {
        return !m_enabled;
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

    bool Widget::isFocused() const
    {
        return m_focused;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetTypes Widget::getWidgetType() const
    {
        return m_callback.widgetType;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container* Widget::getParent() const
    {
        return m_parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setTransparency(unsigned char transparency)
    {
        m_opacity = transparency;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned char Widget::getTransparency() const
    {
        return m_opacity;
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

    void Widget::setCallbackId(unsigned int callbackId)
    {
        m_callback.id = callbackId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Widget::getCallbackId()
    {
        return m_callback.id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::update()
    {
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

    void Widget::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "left")
        {
            setPosition(tgui::stof(value), getPosition().y);
        }
        else if (property == "top")
        {
            setPosition(getPosition().x, tgui::stof(value));
        }
        else if (property == "width")
        {
            setSize(tgui::stof(value), getSize().y);
        }
        else if (property == "height")
        {
            setSize(getSize().x, tgui::stof(value));
        }
        else if (property == "visible")
        {
            if ((value == "true") || (value == "True"))
                m_visible = true;
            else if ((value == "false") || (value == "False"))
                m_visible = false;
            else
                throw Exception("Failed to parse 'Visible' property.");
        }
        else if (property == "enabled")
        {
            if ((value == "true") || (value == "True"))
                m_enabled = true;
            else if ((value == "false") || (value == "False"))
                m_enabled = false;
            else
                throw Exception("Failed to parse 'Enabled' property.");
        }
        else if (property == "transparency")
        {
            setTransparency(static_cast<char>(tgui::stoi(value)));
        }
        else if (property == "callbackid")
        {
            m_callback.id = tgui::stoul(value);
        }
        else if (property == "callback")
        {
            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "Focused") || (*it == "focused"))
                    bindCallback(Focused);
                else if ((*it == "Unfocused") || (*it == "unfocused"))
                    bindCallback(Unfocused);
                else if ((*it == "MouseEntered") || (*it == "mouseentered"))
                    bindCallback(MouseEntered);
                else if ((*it == "MouseLeft") || (*it == "mouseleft"))
                    bindCallback(MouseLeft);
            }
        }
        else // The property didn't match
            throw Exception("Failed to set unknown property '" + property + "'.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "left")
            value = tgui::to_string(getPosition().x);
        else if (property == "top")
            value = tgui::to_string(getPosition().y);
        else if (property == "width")
            value = tgui::to_string(getSize().x);
        else if (property == "height")
            value = tgui::to_string(getSize().y);
        else if (property == "visible")
            value = m_visible ? "true" : "false";
        else if (property == "enabled")
            value = m_enabled ? "true" : "false";
        else if (property == "transparency")
            value = tgui::to_string(int(getTransparency()));
        else if (property == "callbackid")
            value = tgui::to_string(m_callback.id);
        else if (property == "callback")
        {
            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(Focused) != m_callbackFunctions.end()) && (m_callbackFunctions.at(Focused).size() == 1) && (m_callbackFunctions.at(Focused).front() == nullptr))
                callbacks.push_back("Focused");
            if ((m_callbackFunctions.find(Unfocused) != m_callbackFunctions.end()) && (m_callbackFunctions.at(Unfocused).size() == 1) && (m_callbackFunctions.at(Unfocused).front() == nullptr))
                callbacks.push_back("Unfocused");
            if ((m_callbackFunctions.find(MouseEntered) != m_callbackFunctions.end()) && (m_callbackFunctions.at(MouseEntered).size() == 1) && (m_callbackFunctions.at(MouseEntered).front() == nullptr))
                callbacks.push_back("MouseEntered");
            if ((m_callbackFunctions.find(MouseLeft) != m_callbackFunctions.end()) && (m_callbackFunctions.at(MouseLeft).size() == 1) && (m_callbackFunctions.at(MouseLeft).front() == nullptr))
                callbacks.push_back("MouseLeft");

            encodeList(callbacks, value);
        }
        else // The property didn't match
            throw Exception("Failed to get unknown property '" + property + "'.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Widget::getPropertyList() const
    {
        std::list< std::pair<std::string, std::string> > list;
        list.push_back(std::pair<std::string, std::string>("Left", "int"));
        list.push_back(std::pair<std::string, std::string>("Top", "int"));
        list.push_back(std::pair<std::string, std::string>("Width", "uint"));
        list.push_back(std::pair<std::string, std::string>("Height", "uint"));
        list.push_back(std::pair<std::string, std::string>("Visible", "bool"));
        list.push_back(std::pair<std::string, std::string>("Enabled", "bool"));
        list.push_back(std::pair<std::string, std::string>("Transparency", "byte"));
        list.push_back(std::pair<std::string, std::string>("Callback", "custom"));
        list.push_back(std::pair<std::string, std::string>("CallbackId", "uint"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::initialize(Container *const parent)
    {
        assert(parent);

        m_parent = parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
