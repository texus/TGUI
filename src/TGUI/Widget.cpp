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


#include <TGUI/Widget.hpp>
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/Container.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget() :
    m_Enabled        (true),
    m_Visible        (true),
    m_Loaded         (false),
    m_WidgetPhase    (0),
    m_Parent         (nullptr),
    m_Opacity        (255),
    m_MouseHover     (false),
    m_MouseDown      (false),
    m_Focused        (false),
    m_AllowFocus     (false),
    m_AnimatedWidget (false),
    m_DraggableWidget(false),
    m_ContainerWidget(false)
    {
        m_Callback.widget = nullptr;
        m_Callback.widgetType = Type_Unknown;
        m_Callback.id = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& copy) :
    sf::Drawable     (copy),
    Transformable    (copy),
    CallbackManager  (copy),
    m_Enabled        (copy.m_Enabled),
    m_Visible        (copy.m_Visible),
    m_Loaded         (copy.m_Loaded),
    m_WidgetPhase    (copy.m_WidgetPhase),
    m_Parent         (copy.m_Parent),
    m_Opacity        (copy.m_Opacity),
    m_MouseHover     (false),
    m_MouseDown      (false),
    m_Focused        (false),
    m_AllowFocus     (copy.m_AllowFocus),
    m_AnimatedWidget (copy.m_AnimatedWidget),
    m_DraggableWidget(copy.m_DraggableWidget),
    m_ContainerWidget(copy.m_ContainerWidget)
    {
        m_Callback.widget = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::~Widget()
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

            m_Enabled             = right.m_Enabled;
            m_Visible             = right.m_Visible;
            m_Loaded              = right.m_Loaded;
            m_WidgetPhase         = right.m_WidgetPhase;
            m_Parent              = right.m_Parent;
            m_Opacity             = right.m_Opacity;
            m_MouseHover          = false;
            m_MouseDown           = false;
            m_Focused             = false;
            m_AllowFocus          = right.m_AllowFocus;
            m_AnimatedWidget      = right.m_AnimatedWidget;
            m_DraggableWidget     = right.m_DraggableWidget;
            m_ContainerWidget     = right.m_ContainerWidget;
            m_Callback            = Callback();
            m_Callback.widget     = nullptr;
            m_Callback.widgetType = right.m_Callback.widgetType;
            m_Callback.id         = right.m_Callback.id;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Widget::getAbsolutePosition() const
    {
        if (m_Parent)
            return m_Parent->getAbsolutePosition() + m_Parent->getWidgetsOffset() + getPosition();
        else
            return getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::show()
    {
        m_Visible = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::hide()
    {
        m_Visible = false;

        // If the widget is focused then it must be unfocused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isVisible() const
    {
        return m_Visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::enable()
    {
        m_Enabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::disable()
    {
        m_Enabled = false;

        // Change the mouse button state.
        m_MouseHover = false;
        m_MouseDown = false;

        // If the widget is focused then it must be unfocused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isEnabled() const
    {
        return m_Enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isDisabled() const
    {
        return !m_Enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isLoaded() const
    {
        return m_Loaded;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::focus()
    {
        if (m_Parent)
            m_Parent->focusWidget(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unfocus()
    {
        if (m_Focused)
            m_Parent->unfocusWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isFocused() const
    {
        return m_Focused;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetTypes Widget::getWidgetType() const
    {
        return m_Callback.widgetType;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container* Widget::getParent() const
    {
        return m_Parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setTransparency(unsigned char transparency)
    {
        m_Opacity = transparency;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned char Widget::getTransparency() const
    {
        return m_Opacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToFront()
    {
        m_Parent->moveWidgetToFront(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToBack()
    {
        m_Parent->moveWidgetToBack(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setCallbackId(unsigned int callbackId)
    {
        m_Callback.id = callbackId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Widget::getCallbackId()
    {
        return m_Callback.id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::update()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::addCallback()
    {
        // Loop through all callback functions
        auto& functions = m_CallbackFunctions[m_Callback.trigger];
        for (auto func = functions.cbegin(); func != functions.cend(); ++func)
        {
            // Pass the callback to the correct place
            if (*func != nullptr)
                (*func)();
            else
                m_Parent->addChildCallback(m_Callback);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        if (m_CallbackFunctions[MouseEntered].empty() == false)
        {
            m_Callback.trigger = MouseEntered;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        if (m_CallbackFunctions[MouseLeft].empty() == false)
        {
            m_Callback.trigger = MouseLeft;
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
        if (m_MouseHover == false)
            mouseEnteredWidget();

        m_MouseHover = true;
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
        if (m_CallbackFunctions[Focused].empty() == false)
        {
            m_Callback.trigger = Focused;
            addCallback();
        }

        // Make sure the parent is also focused
        if (m_Parent)
            m_Parent->focus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetUnfocused()
    {
        if (m_CallbackFunctions[Unfocused].empty() == false)
        {
            m_Callback.trigger = Unfocused;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNotOnWidget()
    {
        if (m_MouseHover == true)
            mouseLeftWidget();

        m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerDown()
    {
        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "left")
        {
            setPosition(static_cast<float>(atof(value.c_str())), getPosition().y);
        }
        else if (property == "top")
        {
            setPosition(getPosition().x, static_cast<float>(atof(value.c_str())));
        }
        else if (property == "width")
        {
            setSize(static_cast<float>(atof(value.c_str())), getSize().y);
        }
        else if (property == "height")
        {
            setSize(getSize().x, static_cast<float>(atof(value.c_str())));
        }
        else if (property == "visible")
        {
            if ((value == "true") || (value == "True"))
                m_Visible = true;
            else if ((value == "false") || (value == "False"))
                m_Visible = false;
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Visible' property.");
        }
        else if (property == "enabled")
        {
            if ((value == "true") || (value == "True"))
                m_Enabled = true;
            else if ((value == "false") || (value == "False"))
                m_Enabled = false;
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Enabled' property.");
        }
        else if(property == "focused")
        {
            if ((value == "true") || (value == "True"))
                focus();
            else if ((value == "false") || (value == "False"))
                unfocus();
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Focused' property.");
        }
        else if (property == "transparency")
        {
            setTransparency(static_cast<char>(atoi(value.c_str())));
        }
        else if (property == "callbackid")
        {
            m_Callback.id = static_cast<unsigned int>(std::atoi(value.c_str()));
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
            return false;

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "left")
            value = to_string(getPosition().x);
        else if (property == "top")
            value = to_string(getPosition().y);
        else if (property == "width")
            value = to_string(getSize().x);
        else if (property == "height")
            value = to_string(getSize().y);
        else if (property == "visible")
            value = m_Visible ? "true" : "false";
        else if (property == "enabled")
            value = m_Enabled ? "true" : "false";
        else if (property == "focused")
            value = m_Focused ? "true" : "false";
        else if (property == "transparency")
            value = to_string(int(getTransparency()));
        else if (property == "callbackid")
            value = to_string(m_Callback.id);
        else if (property == "callback")
        {
            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(Focused) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(Focused).size() == 1) && (m_CallbackFunctions.at(Focused).front() == nullptr))
                callbacks.push_back("Focused");
            if ((m_CallbackFunctions.find(Unfocused) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(Unfocused).size() == 1) && (m_CallbackFunctions.at(Unfocused).front() == nullptr))
                callbacks.push_back("Unfocused");
            if ((m_CallbackFunctions.find(MouseEntered) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(MouseEntered).size() == 1) && (m_CallbackFunctions.at(MouseEntered).front() == nullptr))
                callbacks.push_back("MouseEntered");
            if ((m_CallbackFunctions.find(MouseLeft) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(MouseLeft).size() == 1) && (m_CallbackFunctions.at(MouseLeft).front() == nullptr))
                callbacks.push_back("MouseLeft");

            encodeList(callbacks, value);
        }
        else // The property didn't match
            return false;

        // You pass here when one of the properties matched
        return true;
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
        list.push_back(std::pair<std::string, std::string>("Focused", "bool"));
        list.push_back(std::pair<std::string, std::string>("Transparency", "byte"));
        list.push_back(std::pair<std::string, std::string>("Callback", "custom"));
        list.push_back(std::pair<std::string, std::string>("CallbackId", "uint"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::initialize(Container *const parent)
    {
        m_Parent = parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetBorders::WidgetBorders() :
    m_LeftBorder  (0),
    m_TopBorder   (0),
    m_RightBorder (0),
    m_BottomBorder(0)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetBorders::~WidgetBorders()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Borders WidgetBorders::getBorders() const
    {
        return Borders(m_LeftBorder, m_TopBorder, m_RightBorder, m_BottomBorder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
