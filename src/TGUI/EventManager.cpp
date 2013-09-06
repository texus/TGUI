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


#include <TGUI/Widget.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/EventManager.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EventManager::EventManager() :
    m_FocusedWidget(0)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EventManager::handleEvent(sf::Event& event)
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
                    if ((m_Widgets[i]->m_DraggableWidget) || (m_Widgets[i]->m_ContainerWidget))
                    {
                        m_Widgets[i]->mouseMoved(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                        return true;
                    }
                }
            }

            // Check if the mouse is on top of an widget
            Widget::Ptr widget = mouseOnWidget(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            if (widget != nullptr)
            {
                // Send the event to the widget
                widget->mouseMoved(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                return true;
            }

            return false;
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
                    }

                    widget->leftMousePressed(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    return true;
                }
                else // The mouse didn't went down on an widget, so unfocus the focused widget
                    unfocusWidgets();
            }

            return false;
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
                    widget->leftMouseReleased(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

                // Tell all the other widgets that the mouse has gone up
                for (std::vector<Widget::Ptr>::iterator it = m_Widgets.begin(); it != m_Widgets.end(); ++it)
                {
                    if (*it != widget)
                        (*it)->mouseNoLongerDown();
                }

                if (widget != nullptr)
                    return true;
            }

            return false;
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

                    return true;
                }
            }

            return false;
        }

        // Check if a key was released
        else if (event.type == sf::Event::KeyReleased)
        {
            // Change the focus to another widget when the tab key was pressed
            if (event.key.code == sf::Keyboard::Tab)
                return tabKeyPressed();
            else
                return false;
        }

        // Also check if text was entered (not a special key)
        else if (event.type == sf::Event::TextEntered)
        {
            // Check if the character that we pressed is allowed
            if ((event.text.unicode >= 30) && (event.text.unicode != 127))
            {
                // Tell the widget that the key was pressed
                if (m_FocusedWidget)
                {
                    m_Widgets[m_FocusedWidget-1]->textEntered(event.text.unicode);
                    return true;
                }
            }

            return false;
        }

        // Check for mouse wheel scrolling
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            // Find the widget under the mouse
            Widget::Ptr widget = mouseOnWidget(static_cast<float>(event.mouseWheel.x), static_cast<float>(event.mouseWheel.y));
            if (widget != nullptr)
            {
                // Send the event to the widget
                widget->mouseWheelMoved(event.mouseWheel.delta, event.mouseWheel.x,  event.mouseWheel.y);
                return true;
            }

            return false;
        }
        else // Event is ignored
            return false;
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

    void EventManager::focusNextWidget()
    {
        // Loop all widgets behind the focused one
        for (unsigned int i = m_FocusedWidget; i < m_Widgets.size(); ++i)
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
            for (unsigned int i = 0; i < m_FocusedWidget - 1; ++i)
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

    void EventManager::focusPreviousWidget()
    {
        // Loop the widgets before the focused one
        if (m_FocusedWidget)
        {
            for (unsigned int i = m_FocusedWidget - 1; i > 0; --i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_Widgets[i-1]->m_AllowFocus == true)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_Widgets[i-1]->m_Visible) && (m_Widgets[i-1]->m_Enabled))
                    {
                        // unfocus the current widget
                        m_Widgets[m_FocusedWidget-1]->m_Focused = false;
                        m_Widgets[m_FocusedWidget-1]->widgetUnfocused();

                        // Focus on the new widget
                        m_FocusedWidget = i;
                        m_Widgets[i-1]->m_Focused = true;
                        m_Widgets[i-1]->widgetFocused();

                        return;
                    }
                }
            }
        }

        // None of the widgets before the focused one could be focused, so loop all widgets behind the focused one
        for (unsigned int i = m_Widgets.size(); i > m_FocusedWidget; --i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_Widgets[i-1]->m_AllowFocus == true)
            {
                // Make sure that the widget is visible and enabled
                if ((m_Widgets[i-1]->m_Visible) && (m_Widgets[i-1]->m_Enabled))
                {
                    if (m_FocusedWidget)
                    {
                        // unfocus the current widget
                        m_Widgets[m_FocusedWidget-1]->m_Focused = false;
                        m_Widgets[m_FocusedWidget-1]->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_FocusedWidget = i;
                    m_Widgets[i-1]->m_Focused = true;
                    m_Widgets[i-1]->widgetFocused();
                    return;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EventManager::unfocusWidgets()
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

    bool EventManager::tabKeyPressed()
    {
        // Don't do anything when the tab key usage is disabled
        if (tabKeyUsageEnabled == false)
            return false;

        // Check if a container is focused
        if (m_FocusedWidget)
        {
            if (m_Widgets[m_FocusedWidget-1]->m_ContainerWidget)
            {
                // Focus the next widget in container
                if (Container::Ptr(m_Widgets[m_FocusedWidget-1])->focusNextWidgetInContainer())
                    return true;
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
                    return true;
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
                        return true;
                    }
                }
            }
        }

        // If the currently focused container widget is the only widget to focus, then focus its next child widget
        if ((m_FocusedWidget) && (m_Widgets[m_FocusedWidget-1]->m_ContainerWidget))
        {
            Container::Ptr(m_Widgets[m_FocusedWidget-1])->m_EventManager.tabKeyPressed();
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EventManager::focusNextWidgetInContainer()
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
        unfocusWidgets();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr EventManager::mouseOnWidget(float x, float y)
    {
        bool widgetFound = false;
        Widget::Ptr widget = nullptr;

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
