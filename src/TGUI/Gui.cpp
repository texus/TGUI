/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/ToolTip.hpp>
#include <TGUI/Gui.hpp>
#include <TGUI/DefaultFont.hpp>

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
        m_container->m_focused = true;

        sf::Font defaultFont;
        if (defaultFont.loadFromMemory(defaultFontBytes, sizeof(defaultFontBytes)))
            setFont(defaultFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderWindow& window) :
        m_window        (&window),
        m_accessToWindow(true)
    {
        m_container->m_window = &window;
        m_container->m_focused = true;

        Clipboard::setWindowHandle(window.getSystemHandle());

        setView(window.getDefaultView());

        sf::Font defaultFont;
        if (defaultFont.loadFromMemory(defaultFontBytes, sizeof(defaultFontBytes)))
            setFont(defaultFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderTarget& window) :
        m_window        (&window),
        m_accessToWindow(false)
    {
        m_container->m_window = &window;
        m_container->m_focused = true;

        setView(window.getDefaultView());

        sf::Font defaultFont;
        if (defaultFont.loadFromMemory(defaultFontBytes, sizeof(defaultFontBytes)))
            setFont(defaultFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setWindow(sf::RenderWindow& window)
    {
        m_accessToWindow = true;

        m_window = &window;
        m_container->m_window = &window;

        Clipboard::setWindowHandle(window.getSystemHandle());

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
        if ((m_view.getCenter() != view.getCenter()) || (m_view.getSize() != view.getSize()))
        {
            m_view = view;

            m_container->m_size = view.getSize();
            m_container->m_callback.size = m_container->getSize();
            m_container->sendSignal("SizeChanged", m_container->getSize());
        }
        else // Set it anyway in case something changed that we didn't care to check
            m_view = view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::handleEvent(sf::Event event)
    {
        assert(m_window != nullptr);

        // Check if the event has something to do with the mouse
        if ((event.type == sf::Event::MouseMoved) || (event.type == sf::Event::TouchMoved)
         || (event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::TouchBegan)
         || (event.type == sf::Event::MouseButtonReleased) || (event.type == sf::Event::TouchEnded)
         || (event.type == sf::Event::MouseWheelMoved))
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

                case sf::Event::TouchMoved:
                case sf::Event::TouchBegan:
                case sf::Event::TouchEnded:
                {
                    mouseCoords = m_window->mapPixelToCoords({event.touch.x, event.touch.y}, m_view);
                    event.touch.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.touch.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                default:
                    break;
            }

            // If a tooltip is visible then hide it now
            if (m_visibleToolTip != nullptr)
            {
                // Correct the position of the tool tip so that it is relative again
                m_visibleToolTip->setPosition(m_visibleToolTip->getPosition() - ToolTip::getDistanceToMouse() - m_lastMousePos);

                remove(m_visibleToolTip);
                m_visibleToolTip = nullptr;
            }

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
                Clipboard::setWindowHandle(static_cast<sf::RenderWindow*>(m_window)->getSystemHandle());
        }

        // Let the event manager handle the event
        return m_container->handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::draw()
    {
        assert(m_window != nullptr);

        // Make sure the right opengl context is set when clipping
        if (dynamic_cast<sf::RenderWindow*>(m_window))
            dynamic_cast<sf::RenderWindow*>(m_window)->setActive(true);
        else if (dynamic_cast<sf::RenderTexture*>(m_window))
            dynamic_cast<sf::RenderTexture*>(m_window)->setActive(true);

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
        m_window->setView(m_view);

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

    sf::Vector2f Gui::getSize() const
    {
        assert(m_window != nullptr);

        return sf::Vector2f{m_window->getSize()};
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

    bool Gui::remove(const Widget::Ptr& widget)
    {
        return m_container->remove(widget);
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

    std::string Gui::getWidgetName(const Widget::Ptr& widget) const
    {
        return m_container->getWidgetName(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusWidget(const Widget::Ptr& widget)
    {
        m_container->focusWidget(widget.get());
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

    void Gui::setOpacity(float opacity)
    {
        m_container->setOpacity(opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Gui::getOpacity() const
    {
        return m_container->getOpacity();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromFile(const std::string& filename)
    {
        m_container->loadWidgetsFromFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToFile(const std::string& filename)
    {
        m_container->saveWidgetsToFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromStream(std::stringstream& stream)
    {
        m_container->loadWidgetsFromStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToStream(std::stringstream& stream)
    {
        m_container->saveWidgetsToStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::updateTime(const sf::Time& elapsedTime)
    {
        m_container->m_animationTimeElapsed = elapsedTime;
        m_container->update(elapsedTime);

        if (m_tooltipPossible)
        {
            m_tooltipTime += elapsedTime;
            if (m_tooltipTime >= ToolTip::getTimeToDisplay())
            {
                Widget::Ptr tooltip = m_container->askToolTip(m_lastMousePos);
                if (tooltip)
                {
                    m_visibleToolTip = tooltip;
                    add(tooltip, "#TGUI_INTERNAL$ToolTip#");

                    // Change the relative tool tip position in an absolute one
                    tooltip->setPosition(m_lastMousePos + ToolTip::getDistanceToMouse() + tooltip->getPosition());
                }

                m_tooltipPossible = false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
