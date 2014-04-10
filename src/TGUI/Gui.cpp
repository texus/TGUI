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


#include <SFML/OpenGL.hpp>

#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/Gui.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui() :
    m_window (nullptr)
    {
        m_container.bindGlobalCallback(&Gui::addChildCallback, this);

        m_container.m_focused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderWindow& window) :
    m_window (&window)
    {
        m_container.m_window = &window;
        m_container.bindGlobalCallback(&Gui::addChildCallback, this);

        m_container.m_focused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setWindow(sf::RenderWindow& window)
    {
        m_window = &window;
        m_container.m_window = &window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderWindow* Gui::getWindow()
    {
        return m_window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::handleEvent(sf::Event event, bool resetView)
    {
        // Check if the event has something to do with the mouse
        if (event.type == sf::Event::MouseMoved)
        {
            sf::Vector2f mouseCoords;
            if (resetView)
                mouseCoords = m_window->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), m_window->getDefaultView());
            else
                mouseCoords = m_window->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), m_window->getView());

            // Adjust the mouse position of the event
            event.mouseMove.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseMove.y = static_cast<int>(mouseCoords.y + 0.5f);
        }
        else if ((event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::MouseButtonReleased))
        {
            sf::Vector2f mouseCoords;
            if (resetView)
                mouseCoords = m_window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), m_window->getDefaultView());
            else
                mouseCoords = m_window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), m_window->getView());

            // Adjust the mouse position of the event
            event.mouseButton.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseButton.y = static_cast<int>(mouseCoords.y + 0.5f);
        }
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            sf::Vector2f mouseCoords;
            if (resetView)
                mouseCoords = m_window->mapPixelToCoords(sf::Vector2i(event.mouseWheel.x, event.mouseWheel.y), m_window->getDefaultView());
            else
                mouseCoords = m_window->mapPixelToCoords(sf::Vector2i(event.mouseWheel.x, event.mouseWheel.y), m_window->getView());

            // Adjust the mouse position of the event
            event.mouseWheel.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseWheel.y = static_cast<int>(mouseCoords.y + 0.5f);
        }

        // Keep track of whether the window is focused or not
        else if (event.type == sf::Event::LostFocus)
        {
            m_container.m_focused = false;
        }
        else if (event.type == sf::Event::GainedFocus)
        {
            m_container.m_focused = true;
        }

        // Let the event manager handle the event
        return m_container.handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::draw(bool resetView)
    {
        sf::View oldView = m_window->getView();

        // Reset the view when requested
        if (resetView)
            m_window->setView(m_window->getDefaultView());

        // Update the time
        if (m_container.m_focused)
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

        // Draw the window with all widgets inside it
        m_container.drawWidgetContainer(m_window, sf::RenderStates::Default);

        // Reset clipping to its original state
        if (clippingEnabled)
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        else
            glDisable(GL_SCISSOR_TEST);

        m_window->setView(oldView);
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

    bool Gui::hasFocus()
    {
        return m_container.m_focused;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Gui::getSize() const
    {
        return sf::Vector2f(m_window->getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container& Gui::getContainer()
    {
        return m_container;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::setGlobalFont(const std::string& filename)
    {
        return m_container.setGlobalFont(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setGlobalFont(const sf::Font& font)
    {
        m_container.setGlobalFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Gui::getGlobalFont() const
    {
        return m_container.getGlobalFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector< Widget::Ptr >& Gui::getWidgets()
    {
        return m_container.getWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String>& Gui::getWidgetNames()
    {
        return m_container.getWidgetNames();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        m_container.add(widgetPtr, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Gui::get(const sf::String& widgetName) const
    {
        return m_container.get(widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Gui::copy(const Widget::Ptr& oldWidget, const sf::String& newWidgetName)
    {
        return m_container.copy(oldWidget, newWidgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::remove(const Widget::Ptr& widget)
    {
        m_container.remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::removeAllWidgets()
    {
        m_container.removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::setWidgetName(const Widget::Ptr& widget, const std::string& name)
    {
        return m_container.setWidgetName(widget, name);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::getWidgetName(const Widget::Ptr& widget, std::string& name) const
    {
        return m_container.getWidgetName(widget, name);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusWidget(Widget::Ptr& widget)
    {
        m_container.focusWidget(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusNextWidget()
    {
        m_container.focusNextWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusPreviousWidget()
    {
        m_container.focusPreviousWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unfocusWidgets()
    {
        m_container.unfocusWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::uncheckRadioButtons()
    {
        m_container.uncheckRadioButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::moveWidgetToFront(Widget::Ptr& widget)
    {
        m_container.moveWidgetToFront(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::moveWidgetToBack(Widget::Ptr& widget)
    {
        m_container.moveWidgetToBack(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::bindGlobalCallback(std::function<void(const Callback&)> func)
    {
        m_container.bindGlobalCallback(func);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unbindGlobalCallback()
    {
        m_container.unbindGlobalCallback();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromFile(const std::string& filename)
    {
        m_container.loadWidgetsFromFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToFile(const std::string& filename)
    {
        m_container.saveWidgetsToFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::updateTime(const sf::Time& elapsedTime)
    {
        m_container.m_animationTimeElapsed = elapsedTime;
        m_container.update();
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
