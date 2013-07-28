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
#include <TGUI/ContainerWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContainerWidget::ContainerWidget()
    {
        m_ContainerWidget = true;
        m_AnimatedWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContainerWidget::ContainerWidget(const ContainerWidget& containerWidgetToCopy) :
    Widget   (containerWidgetToCopy),
    Container(containerWidgetToCopy)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContainerWidget::~ContainerWidget()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContainerWidget& ContainerWidget::operator= (const ContainerWidget& right)
    {
        if (this != &right)
        {
            this->Widget::operator=(right);
            this->Container::operator=(right);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContainerWidget::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        for (unsigned int i = 0; i < m_EventManager.m_Widgets.size(); ++i)
            m_EventManager.m_Widgets[i]->setTransparency(transparency);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContainerWidget::addChildCallback(Callback& callback)
    {
        // If there is no global callback function then send the callback to the parent
        if (m_GlobalCallbackFunctions.empty())
            m_Parent->addChildCallback(callback);
        else
        {
            // Loop through all callback functions and call them
            for (std::list< std::function<void(const Callback&)> >::const_iterator it = m_GlobalCallbackFunctions.begin(); it != m_GlobalCallbackFunctions.end(); ++it)
                (*it)(callback);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContainerWidget::initialize(tgui::Container *const parent)
    {
        m_Parent = parent;
        setGlobalFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContainerWidget::update()
    {
        m_EventManager.updateTime(m_AnimationTimeElapsed);
        m_AnimationTimeElapsed = sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContainerWidget::handleEvent(sf::Event& event, float mouseX, float mouseY)
    {
        // Adjust the mouse position of the event when the event is about the mouse
        if (event.type == sf::Event::MouseMoved)
        {
            event.mouseMove.x = static_cast<int>(mouseX - getPosition().x);
            event.mouseMove.y = static_cast<int>(mouseY - getPosition().y);
        }
        else if ((event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::MouseButtonReleased))
        {
            event.mouseButton.x = static_cast<int>(mouseX - getPosition().x);
            event.mouseButton.y = static_cast<int>(mouseY - getPosition().y);
        }
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            event.mouseWheel.x = static_cast<int>(mouseX - getPosition().x);
            event.mouseWheel.y = static_cast<int>(mouseY - getPosition().y);
        }

        // Let the event manager handle the event
        m_EventManager.handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContainerWidget::mouseNotOnWidget()
    {
        if (m_MouseHover == true)
        {
            mouseLeftWidget();

            m_EventManager.mouseNotOnWidget();

            m_MouseHover = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContainerWidget::focusNextWidgetInContainer()
    {
        return m_EventManager.focusNextWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
