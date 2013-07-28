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

    EventManager::EventManager() :
    m_FocusedWidget(0)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::handleEvent(sf::Event& event)
    {
        // Check if a mouse button has moved
        if (event.type == sf::Event::MouseMoved)
        {
            // Loop through all widgets
            for (unsigned int i=0; i<m_Widgets.size(); ++i)
            {
                // Check if the mouse went down on the widget
                if (m_Widgets[i]->m_MouseDown)
                {
                    // Some widgets should always receive mouse move events while dragging them, even if the mouse is no longer on top of them.
                    if (m_Widgets[i]->m_DraggableWidget)
                    {
                        m_Widgets[i]->mouseMoved(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                        return;
                    }

                    // Containers also need a different treatment
                    else if (m_Widgets[i]->m_ContainerWidget)
                    {
                        // Make the event handler of the container do the rest
                        static_cast<ContainerWidget::Ptr>(m_Widgets[i])->handleEvent(event, static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                        return;
                    }
                }
            }

            // Check if the mouse is on top of an widget
            Widget::Ptr widget = mouseOnWidget(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            if (widget != nullptr)
            {
                // Check if the widget is a container
                if (widget->m_ContainerWidget)
                {
                    // Make the event handler of the container do the rest
                    static_cast<ContainerWidget::Ptr>(widget)->handleEvent(event, static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                }
                else // Send the event to the widget
                    widget->mouseMoved(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            }
        }

        // Check if a mouse button was pressed
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            // Check if the left mouse was pressed
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse is on top of an widget
                Widget::Ptr widget = mouseOnWidget(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                if (widget != nullptr)
                {
                    // Focus the widget
                    focusWidget(widget.get());

                    // Check if the widget is a container
                    if (widget->m_ContainerWidget)
                    {
                        // If another widget was focused then unfocus it now
                        if ((m_FocusedWidget) && (m_Widgets[m_FocusedWidget-1] != widget))
                        {
                            m_Widgets[m_FocusedWidget-1]->m_Focused = false;
                            m_Widgets[m_FocusedWidget-1]->widgetUnfocused();
                            m_FocusedWidget = 0;
                        }

                        // Make the event handler of the container do the rest
                        static_cast<ContainerWidget::Ptr>(widget)->handleEvent(event, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    }
                    else // The event has to be sent to an widget
                        widget->leftMousePressed(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                }
                else // The mouse didn't went down on an widget, so unfocus the focused widget
                    unfocusAllWidgets();
            }
        }

        // Check if a mouse button was released
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            // Check if the left mouse was released
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse is on top of an widget
                Widget::Ptr widget = mouseOnWidget(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                if (widget != nullptr)
                {
                    // Check if the widget is a container
                    if (widget->m_ContainerWidget)
                    {
                        // Make the event handler of the container do the rest
                        static_cast<ContainerWidget::Ptr>(widget)->handleEvent(event, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    }
                    else // Send the event to the widget
                        widget->leftMouseReleased(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

                    // Tell all the other widgets that the mouse has gone up
                    for (std::vector<Widget::Ptr>::iterator it = m_Widgets.begin(); it != m_Widgets.end(); ++it)
                    {
                        if (*it != widget)
                        {
                            if ((*it)->m_ContainerWidget)
                                static_cast<ContainerWidget::Ptr>(*it)->handleEvent(event, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                            else
                                (*it)->mouseNoLongerDown();
                        }
                    }
                }
                else
                {
                    // Tell all the widgets that the mouse has gone up
                    for (unsigned int i=0; i<m_Widgets.size(); ++i)
                    {
                        if (m_Widgets[i]->m_ContainerWidget)
                            static_cast<ContainerWidget::Ptr>(m_Widgets[i])->handleEvent(event, static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                        else
                            m_Widgets[i]->mouseNoLongerDown();
                    }
                }
            }
        }

        // Check if a key was pressed
        else if (event.type == sf::Event::KeyPressed)
        {
            // Only continue when the character was recognised
            if (event.key.code != sf::Keyboard::Unknown)
            {
                // Check if there is a focused widget
                if (m_FocusedWidget)
                {
                    // Check if the widget is a container
                    if (m_Widgets[m_FocusedWidget-1]->m_ContainerWidget)
                    {
                        // Make the event handler of the container do the rest
                        static_cast<ContainerWidget::Ptr>(m_Widgets[m_FocusedWidget-1])->handleEvent(event);
                    }
                    else // The event has to be send to a normal widget
                    {
                        // Check the pressed key
                        if ((event.key.code == sf::Keyboard::Left)
                         || (event.key.code == sf::Keyboard::Right)
                         || (event.key.code == sf::Keyboard::Up)
                         || (event.key.code == sf::Keyboard::Down)
                         || (event.key.code == sf::Keyboard::BackSpace)
                         || (event.key.code == sf::Keyboard::Delete)
                         || (event.key.code == sf::Keyboard::Space)
                         || (event.key.code == sf::Keyboard::Return))
                        {
                            // Tell the widget that the key was pressed
                            m_Widgets[m_FocusedWidget-1]->keyPressed(event.key.code);
                        }
                    }
                }
            }
        }

        // Check if a key was released
        else if (event.type == sf::Event::KeyReleased)
        {
            // Change the focus to another widget when the tab key was pressed
            if (event.key.code == sf::Keyboard::Tab)
                tabKeyPressed();
        }

        // Also check if text was entered (not a special key)
        else if (event.type == sf::Event::TextEntered)
        {
            // Check if the character that we pressed is allowed
            if ((event.text.unicode >= 30) && (event.text.unicode != 127))
            {
                // Check if there is a focused widget
                if (m_FocusedWidget)
                {
                    // Check if the widget is a container
                    if (m_Widgets[m_FocusedWidget-1]->m_ContainerWidget)
                    {
                        // Make the event handler of the container do the rest
                        static_cast<ContainerWidget::Ptr>(m_Widgets[m_FocusedWidget-1])->handleEvent(event);
                    }
                    else // Tell the widget that the key was pressed
                        m_Widgets[m_FocusedWidget-1]->textEntered(event.text.unicode);
                }
            }
        }

        // Check for mouse wheel scrolling
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            // Find the widget under the mouse
            Widget::Ptr widget = mouseOnWidget(static_cast<float>(event.mouseWheel.x), static_cast<float>(event.mouseWheel.y));
            if (widget != nullptr)
            {
                // Check if the widget is a container
                if (widget->m_ContainerWidget)
                {
                    // Make the event handler of the container do the rest
                    static_cast<ContainerWidget::Ptr>(widget)->handleEvent(event, static_cast<float>(event.mouseWheel.x), static_cast<float>(event.mouseWheel.y));
                }
                else // Send the event to the widget
                    widget->mouseWheelMoved(event.mouseWheel.delta);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::focusWidget(Widget *const widget)
    {
        // Loop all the widgets
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            // Search for the widget that has to be focused
            if (m_Widgets[i].get() == widget)
            {
                // Only continue when the widget wasn't already focused
                if (m_FocusedWidget != i+1)
                {
                    // Unfocus the currently focused widget
                    if (m_FocusedWidget)
                    {
                        m_Widgets[m_FocusedWidget-1]->m_Focused = false;
                        m_Widgets[m_FocusedWidget-1]->widgetUnfocused();
                    }

                    // Focus the new widget
                    m_FocusedWidget = i+1;
                    widget->m_Focused = true;
                    widget->widgetFocused();
                }

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::unfocusWidget(Widget *const widget)
    {
        // Check if the widget is focused
        if (widget->m_Focused)
        {
            // Focus the next widget
            tabKeyPressed();

            // Make sure that the widget gets unfocused
            if (widget->m_Focused)
            {
                widget->m_Focused = false;
                widget->widgetUnfocused();
                m_FocusedWidget = 0;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::unfocusAllWidgets()
    {
        if (m_FocusedWidget)
        {
            m_Widgets[m_FocusedWidget-1]->m_Focused = false;
            m_Widgets[m_FocusedWidget-1]->widgetUnfocused();
            m_FocusedWidget = 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::updateTime(const sf::Time& elapsedTime)
    {
        // Loop through all widgets
        for (unsigned int i=0; i<m_Widgets.size(); ++i)
        {
            // Check if the widget is a container or an widget that uses the time
            if (m_Widgets[i]->m_AnimatedWidget)
            {
                // Update the elapsed time
                m_Widgets[i]->m_AnimationTimeElapsed += elapsedTime;
                m_Widgets[i]->update();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::tabKeyPressed()
    {
        // Don't do anything when the tab key usage is disabled
        if (tabKeyUsageEnabled == false)
            return;

        // Check if a container is focused
        if (m_FocusedWidget)
        {
            if (m_Widgets[m_FocusedWidget-1]->m_ContainerWidget)
            {
                // Focus the next widget in container
                if (static_cast<ContainerWidget::Ptr>(m_Widgets[m_FocusedWidget-1])->focusNextWidgetInContainer())
                    return;
            }
        }

        // Loop all widgets behind the focused one
        for (unsigned int i=m_FocusedWidget; i<m_Widgets.size(); ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_Widgets[i]->m_AllowFocus == true)
            {
                // Make sure that the widget is visible and enabled
                if ((m_Widgets[i]->m_Visible) && (m_Widgets[i]->m_Enabled))
                {
                    if (m_FocusedWidget)
                    {
                        // unfocus the current widget
                        m_Widgets[m_FocusedWidget-1]->m_Focused = false;
                        m_Widgets[m_FocusedWidget-1]->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_FocusedWidget = i+1;
                    m_Widgets[i]->m_Focused = true;
                    m_Widgets[i]->widgetFocused();
                    return;
                }
            }
        }

        // None of the widgets behind the focused one could be focused, so loop the ones before it
        if (m_FocusedWidget)
        {
            for (unsigned int i=0; i<m_FocusedWidget-1; ++i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_Widgets[i]->m_AllowFocus == true)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_Widgets[i]->m_Visible) && (m_Widgets[i]->m_Enabled))
                    {
                        // unfocus the current widget
                        m_Widgets[m_FocusedWidget-1]->m_Focused = false;
                        m_Widgets[m_FocusedWidget-1]->widgetUnfocused();

                        // Focus on the new widget
                        m_FocusedWidget = i+1;
                        m_Widgets[i]->m_Focused = true;
                        m_Widgets[i]->widgetFocused();
                        return;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EventManager::focusNextWidget()
    {
        // Don't do anything when the tab key usage is disabled
        if (tabKeyUsageEnabled == false)
            return false;

        // Loop through all widgets
        for (unsigned int i=m_FocusedWidget; i<m_Widgets.size(); ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_Widgets[i]->m_AllowFocus == true)
            {
                // Make sure that the widget is visible and enabled
                if ((m_Widgets[i]->m_Visible) && (m_Widgets[i]->m_Enabled))
                {
                    if (m_FocusedWidget > 0)
                    {
                        // Unfocus the current widget
                        m_Widgets[m_FocusedWidget-1]->m_Focused = false;
                        m_Widgets[m_FocusedWidget-1]->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_FocusedWidget = i+1;
                    m_Widgets[i]->m_Focused = true;
                    m_Widgets[i]->widgetFocused();

                    return true;
                }
            }
        }

        // We have the highest id
        unfocusAllWidgets();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr EventManager::mouseOnWidget(float x, float y)
    {
        bool widgetFound = false;
        Widget::Ptr widget;

        // Loop through all widgets
        for (std::vector<Widget::Ptr>::reverse_iterator it = m_Widgets.rbegin(); it != m_Widgets.rend(); ++it)
        {
            // Check if the widget is visible and enabled
            if (((*it)->m_Visible) && ((*it)->m_Enabled))
            {
                if (widgetFound == false)
                {
                    // Return the widget if the mouse is on top of it
                    if ((*it)->mouseOnWidget(x, y))
                    {
                        widget = *it;
                        widgetFound = true;
                    }
                }
                else // The widget was already found, so tell the other widgets that the mouse can't be on them
                    (*it)->mouseNotOnWidget();
            }
        }

        // The mouse isn't on any widget
        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::mouseNotOnWidget()
    {
        // Loop through all widgets
        for (unsigned int i=0; i<m_Widgets.size(); ++i)
        {
            // Tell the widget that the mouse is no longer on top of it
            m_Widgets[i]->mouseNotOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::mouseNoLongerDown()
    {
        // Loop through all widgets
        for (unsigned int i=0; i<m_Widgets.size(); ++i)
        {
            // Tell the widget that the mouse is no longer down
            m_Widgets[i]->mouseNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
