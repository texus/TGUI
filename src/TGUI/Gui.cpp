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


#include <TGUI/Clipboard.hpp>
#include <TGUI/Tooltip.hpp>
#include <TGUI/Gui.hpp>

#include <SFML/OpenGL.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui() :
        m_window        (nullptr),
        m_accessToWindow(false)
    {
        m_container->bindGlobalCallback(&Gui::addChildCallback, this);

        m_container->m_focused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderWindow& window) :
        m_window        (&window),
        m_accessToWindow(true)
    {
        m_container->m_window = &window;
        m_container->bindGlobalCallback(&Gui::addChildCallback, this);

        m_container->m_focused = true;

        TGUI_Clipboard.setWindowHandle(window.getSystemHandle());

        setView(window.getDefaultView());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderTarget& window) :
        m_window        (&window),
        m_accessToWindow(false)
    {
        m_container->m_window = &window;
        m_container->bindGlobalCallback(&Gui::addChildCallback, this);

        m_container->m_focused = true;

        setView(window.getDefaultView());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setWindow(sf::RenderWindow& window)
    {
        m_accessToWindow = true;

        m_window = &window;
        m_container->m_window = &window;

        TGUI_Clipboard.setWindowHandle(window.getSystemHandle());

        setView(window.getDefaultView());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setWindow(sf::RenderTarget& window)
    {
        m_accessToWindow = false;

        m_window = &window;
        m_container->m_window = &window;

        setView(window.getDefaultView());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setView(const sf::View& view)
    {
        m_view = view;
        m_container->m_size = view.getSize();
        m_container->m_position = view.getCenter() - (view.getSize() / 2.0f);

        if (m_container->m_callbackFunctions[Widget::SizeChanged].empty() == false)
        {
            m_container->m_callback.trigger = Widget::SizeChanged;
            m_container->m_callback.size    = m_container->getSize();
            m_container->addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::handleEvent(sf::Event event)
    {
        assert(m_window != nullptr);

        // Check if the event has something to do with the mouse
        if ((event.type == sf::Event::MouseMoved) || (event.type == sf::Event::MouseButtonPressed)
         || (event.type == sf::Event::MouseButtonReleased) || (event.type == sf::Event::MouseWheelMoved))
        {
            sf::Vector2f mouseCoords;

            switch (event.type)
            {
                case sf::Event::MouseMoved:
                {
                    mouseCoords = m_window->mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}, m_view);
                    event.mouseMove.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseMove.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                case sf::Event::MouseButtonPressed:
                case sf::Event::MouseButtonReleased:
                {
                    mouseCoords = m_window->mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}, m_view);
                    event.mouseButton.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseButton.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                case sf::Event::MouseWheelMoved:
                {
                    mouseCoords = m_window->mapPixelToCoords({event.mouseWheel.x, event.mouseWheel.y}, m_view);
                    event.mouseWheel.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseWheel.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                default:
                    break;
            }

            // If a tooltip is visible then hide it now
            if (m_visibleTooltip != nullptr)
                remove(m_visibleTooltip);

            // Reset the data for the tooltip since the mouse has moved
            m_tooltipTime = {};
            m_tooltipPossible = true;
            m_lastMousePos = mouseCoords;
        }

        // Keep track of whether the window is focused or not
        else if (event.type == sf::Event::LostFocus)
        {
            m_container->m_focused = false;
        }
        else if (event.type == sf::Event::GainedFocus)
        {
            m_container->m_focused = true;

            if (m_accessToWindow)
                TGUI_Clipboard.setWindowHandle(static_cast<sf::RenderWindow*>(m_window)->getSystemHandle());
        }

        // Let the event manager handle the event
        return m_container->handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::draw()
    {
        assert(m_window != nullptr);

        // Update the time
        if (m_container->m_focused)
            updateTime(m_clock.restart());
        else
            m_clock.restart();

        // Check if clipping is enabled
        GLboolean clippingEnabled = glIsEnabled(GL_SCISSOR_TEST);
        GLint scissor[4];

        if (clippingEnabled)
        {
            // Remember the old clipping area
            glGetIntegerv(GL_SCISSOR_BOX, scissor);
        }
        else // Clipping was disabled
        {
            // Enable clipping
            glEnable(GL_SCISSOR_TEST);
            glScissor(0, 0, m_window->getSize().x, m_window->getSize().y);
        }

        // Change the view
        sf::View oldView = m_window->getView();
        m_window->setView(sf::View{{m_container->getPosition().x, m_container->getPosition().y, m_container->getSize().x, m_container->getSize().y}});

        // Draw the window with all widgets inside it
        m_container->drawWidgetContainer(m_window, sf::RenderStates::Default);

        // Restore the old view
        m_window->setView(oldView);

        // Reset clipping to its original state
        if (clippingEnabled)
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        else
            glDisable(GL_SCISSOR_TEST);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::pollCallback(Callback& callback)
    {
        // Check if the callback queue is empty
        if (m_callback.empty())
            return false;
        else // The queue is not empty
        {
            // Get the next callback
            callback = m_callback.front();

            // Remove the callback from the queue
            m_callback.pop();

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        m_container->add(widgetPtr, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Gui::get(const sf::String& widgetName, bool recursive) const
    {
        return m_container->get(widgetName, recursive);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::remove(const Widget::Ptr& widget)
    {
        m_container->remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::removeAllWidgets()
    {
        m_container->removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::setWidgetName(const Widget::Ptr& widget, const std::string& name)
    {
        return m_container->setWidgetName(widget, name);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::getWidgetName(const Widget::Ptr& widget, std::string& name) const
    {
        return m_container->getWidgetName(widget, name);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusWidget(Widget::Ptr& widget)
    {
        m_container->focusWidget(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusNextWidget()
    {
        m_container->focusNextWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusPreviousWidget()
    {
        m_container->focusPreviousWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unfocusWidgets()
    {
        m_container->unfocusWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::uncheckRadioButtons()
    {
        m_container->uncheckRadioButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::moveWidgetToFront(Widget::Ptr& widget)
    {
        m_container->moveWidgetToFront(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::moveWidgetToBack(Widget::Ptr& widget)
    {
        m_container->moveWidgetToBack(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::bindGlobalCallback(std::function<void(const Callback&)> func)
    {
        m_container->bindGlobalCallback(func);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unbindGlobalCallback()
    {
        m_container->unbindGlobalCallback();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::updateTime(const sf::Time& elapsedTime)
    {
        m_container->m_animationTimeElapsed = elapsedTime;
        m_container->update();

        if (m_tooltipPossible)
        {
            m_tooltipTime += elapsedTime;
            if (m_tooltipTime >= sf::milliseconds(500))
            {
                Tooltip::Ptr tooltip = m_container->askTooltip(m_lastMousePos);
                if (tooltip)
                {
                    m_visibleTooltip = tooltip;
                    add(tooltip, "#TGUI_INTERNAL$Tooltip#");

                    tooltip->setPosition({m_lastMousePos.x + 5, m_lastMousePos.y + 20});
                    tooltip->moveToFront();
                }

                m_tooltipPossible = false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::addChildCallback(const Callback& callback)
    {
        // Add the callback to the queue
        m_callback.push(callback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
