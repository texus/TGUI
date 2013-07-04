/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets.hpp>
#include <TGUI/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::ClickableWidget() :
    m_Size(0, 0)
    {
        m_Callback.widgetType = Type_ClickableWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::ClickableWidget(const ClickableWidget& copy) :
    Widget(copy),
    m_Size(copy.m_Size)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::~ClickableWidget()
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

            std::swap(m_Size, temp.m_Size);
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
        m_Size.x = width;
        m_Size.y = height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ClickableWidget::getSize() const
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ClickableWidget::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the widget
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
            return true;
        else
        {
            if (m_MouseHover)
                mouseLeftWidget();

            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag
        m_MouseDown = true;

        // Add the callback (if the user requested it)
        if (m_CallbackFunctions[LeftMousePressed].empty() == false)
        {
            m_Callback.trigger = LeftMousePressed;
            m_Callback.mouse.x = x - getPosition().x;
            m_Callback.mouse.y = y - getPosition().y;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMouseReleased(float x, float y)
    {
        // Add the callback (if the user requested it)
        if (m_CallbackFunctions[LeftMouseReleased].empty() == false)
        {
            m_Callback.trigger = LeftMouseReleased;
            m_Callback.mouse.x = x - getPosition().x;
            m_Callback.mouse.y = y - getPosition().y;
            addCallback();
        }

        // Check if we clicked on the label (not just mouse release)
        if (m_MouseDown == true)
        {
            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[LeftMouseClicked].empty() == false)
            {
                m_Callback.trigger = LeftMouseClicked;
                m_Callback.mouse.x = x - getPosition().x;
                m_Callback.mouse.y = y - getPosition().y;
                addCallback();
            }

            m_MouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::draw(sf::RenderTarget&, sf::RenderStates) const
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
