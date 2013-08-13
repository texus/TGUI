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


#include <SFML/OpenGL.hpp>

#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/Gui.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui() :
    m_Window(nullptr)
    {
        m_Container.bindGlobalCallback(&Gui::addChildCallback, this);

        // The main window is always focused
        m_Container.m_ContainerFocused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderWindow& window) :
    m_Window(&window)
    {
        m_Container.m_Window = &window;
        m_Container.bindGlobalCallback(&Gui::addChildCallback, this);

        // The main window is always focused
        m_Container.m_ContainerFocused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setWindow(sf::RenderWindow& window)
    {
        m_Window = &window;
        m_Container.m_Window = &window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderWindow* Gui::getWindow()
    {
        return m_Window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::handleEvent(sf::Event event)
    {
        // Check if the event has something to do with the mouse
        if (event.type == sf::Event::MouseMoved)
        {
            sf::Vector2f mouseCoords = m_Window->mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), m_Window->getView());

            // Adjust the mouse position of the event
            event.mouseMove.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseMove.y = static_cast<int>(mouseCoords.y + 0.5f);
        }
        else if ((event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::MouseButtonReleased))
        {
            sf::Vector2f mouseCoords = m_Window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), m_Window->getView());

            // Adjust the mouse position of the event
            event.mouseButton.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseButton.y = static_cast<int>(mouseCoords.y + 0.5f);
        }
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            sf::Vector2f mouseCoords = m_Window->mapPixelToCoords(sf::Vector2i(event.mouseWheel.x, event.mouseWheel.y), m_Window->getView());

            // Adjust the mouse position of the event
            event.mouseWheel.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseWheel.y = static_cast<int>(mouseCoords.y + 0.5f);
        }

        // Let the event manager handle the event
        return m_Container.m_EventManager.handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::draw()
    {
        // Update the time
        updateTime(m_Clock.restart());

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
            glScissor(0, 0, m_Window->getSize().x, m_Window->getSize().y);
        }

        // Draw the window with all widgets inside it
        m_Container.drawWidgetContainer(m_Window, sf::RenderStates::Default);

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
        if (m_Callback.empty())
            return false;
        else // The queue is not empty
        {
            // Get the next callback
            callback = m_Callback.front();

            // Remove the callback from the queue
            m_Callback.pop();

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Gui::getSize() const
    {
        return sf::Vector2f(m_Window->getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::setGlobalFont(const std::string& filename)
    {
        return m_Container.setGlobalFont(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setGlobalFont(const sf::Font& font)
    {
        m_Container.setGlobalFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font& Gui::getGlobalFont() const
    {
        return m_Container.getGlobalFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector< Widget::Ptr >& Gui::getWidgets()
    {
        return m_Container.getWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String>& Gui::getWidgetNames()
    {
        return m_Container.getWidgetNames();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        m_Container.add(widgetPtr, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Gui::get(const sf::String& widgetName) const
    {
        return m_Container.get(widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Gui::copy(const Widget::Ptr& oldWidget, const sf::String& newWidgetName)
    {
        return m_Container.copy(oldWidget, newWidgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::remove(const Widget::Ptr& widget)
    {
        m_Container.remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::removeAllWidgets()
    {
        m_Container.removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusWidget(Widget::Ptr& widget)
    {
        m_Container.focusWidget(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unfocusWidget(Widget::Ptr& widget)
    {
        m_Container.unfocusWidget(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unfocusAllWidgets()
    {
        m_Container.unfocusAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::uncheckRadioButtons()
    {
        m_Container.uncheckRadioButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::moveWidgetToFront(Widget::Ptr& widget)
    {
        m_Container.moveWidgetToFront(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::moveWidgetToBack(Widget::Ptr& widget)
    {
        m_Container.moveWidgetToBack(&*widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::bindGlobalCallback(std::function<void(const Callback&)> func)
    {
        m_Container.bindGlobalCallback(func);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unbindGlobalCallback()
    {
        m_Container.unbindGlobalCallback();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::loadWidgetsFromFile(const std::string& filename)
    {
        return m_Container.loadWidgetsFromFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::updateTime(const sf::Time& elapsedTime)
    {
        m_Container.m_EventManager.updateTime(elapsedTime);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::addChildCallback(const Callback& callback)
    {
        // Add the callback to the queue
        m_Callback.push(callback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
