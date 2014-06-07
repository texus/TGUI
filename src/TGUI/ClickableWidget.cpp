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


#include <TGUI/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::ClickableWidget() :
    m_size(0, 0)
    {
        m_callback.widgetType = Type_ClickableWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::ClickableWidget(const ClickableWidget& copy) :
    Widget(copy),
    m_size(copy.m_size)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget& ClickableWidget::operator= (const ClickableWidget& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            ClickableWidget temp(right);
            this->Widget::operator=(right);

            std::swap(m_size, temp.m_size);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget* ClickableWidget::clone()
    {
        return new ClickableWidget(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::setSize(float width, float height)
    {
        m_size.x = width;
        m_size.y = height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ClickableWidget::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ClickableWidget::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the widget
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
            return true;
        else
        {
            if (m_mouseHover)
                mouseLeftWidget();

            m_mouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag
        m_mouseDown = true;

        // Add the callback (if the user requested it)
        if (m_callbackFunctions[LeftMousePressed].empty() == false)
        {
            m_callback.trigger = LeftMousePressed;
            m_callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMouseReleased(float x, float y)
    {
        // Add the callback (if the user requested it)
        if (m_callbackFunctions[LeftMouseReleased].empty() == false)
        {
            m_callback.trigger = LeftMouseReleased;
            m_callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }

        // Check if we clicked on the label (not just mouse release)
        if (m_mouseDown == true)
        {
            // Add the callback (if the user requested it)
            if (m_callbackFunctions[LeftMouseClicked].empty() == false)
            {
                m_callback.trigger = LeftMouseClicked;
                m_callback.mouse.x = static_cast<int>(x - getPosition().x);
                m_callback.mouse.y = static_cast<int>(y - getPosition().y);
                addCallback();
            }

            m_mouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "callback")
        {
            Widget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "LeftMousePressed") || (*it == "leftmousepressed"))
                    bindCallback(LeftMousePressed);
                else if ((*it == "LeftMouseReleased") || (*it == "leftmousereleased"))
                    bindCallback(LeftMouseReleased);
                else if ((*it == "LeftMouseClicked") || (*it == "leftmouseclicked"))
                    bindCallback(LeftMouseClicked);
            }
        }
        else // The property didn't match
            Widget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(LeftMousePressed) != m_callbackFunctions.end()) && (m_callbackFunctions.at(LeftMousePressed).size() == 1) && (m_callbackFunctions.at(LeftMousePressed).front() == nullptr))
                callbacks.push_back("LeftMousePressed");
            if ((m_callbackFunctions.find(LeftMouseReleased) != m_callbackFunctions.end()) && (m_callbackFunctions.at(LeftMouseReleased).size() == 1) && (m_callbackFunctions.at(LeftMouseReleased).front() == nullptr))
                callbacks.push_back("LeftMouseReleased");
            if ((m_callbackFunctions.find(LeftMouseClicked) != m_callbackFunctions.end()) && (m_callbackFunctions.at(LeftMouseClicked).size() == 1) && (m_callbackFunctions.at(LeftMouseClicked).front() == nullptr))
                callbacks.push_back("LeftMouseClicked");

            encodeList(callbacks, value);

            if (value.empty() || tempValue.empty())
                value += tempValue;
            else
                value += "," + tempValue;
        }
        else // The property didn't match
            Widget::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::draw(sf::RenderTarget&, sf::RenderStates) const
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
