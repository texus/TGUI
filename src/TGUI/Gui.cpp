/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Objects.hpp>
#include <TGUI/Gui.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui() :
    m_Window(NULL)
    {
        // The main window is always focused
        m_GroupFocused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderWindow& window) :
    m_Window(&window)
    {
        // The main window is always focused
        m_GroupFocused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setWindow(sf::RenderWindow& window)
    {
        m_Window = &window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderWindow* Gui::getWindow()
    {
        return m_Window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::handleEvent(sf::Event event)
    {
        // Check if the event is a mouse move or mouse down/press
        if (event.type == sf::Event::MouseMoved)
        {
            Vector2f mouseCoords = m_Window->mapPixelToCoords(Vector2i(event.mouseMove.x, event.mouseMove.y), m_Window->getView());

            // Adjust the mouse position of the event
            event.mouseMove.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseMove.y = static_cast<int>(mouseCoords.y + 0.5f);
        }
        else if ((event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::MouseButtonReleased))
        {
            Vector2f mouseCoords = m_Window->mapPixelToCoords(Vector2i(event.mouseButton.x, event.mouseButton.y), m_Window->getView());

            // Adjust the mouse position of the event
            event.mouseButton.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseButton.y = static_cast<int>(mouseCoords.y + 0.5f);
        }

        // Let the event manager handle the event
        m_EventManager.handleEvent(event);
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

        // Draw the window with all objects inside it
        drawObjectGroup(m_Window, sf::RenderStates::Default);

        // Check if clipping was previously enabled
        if (clippingEnabled)
        {
            // Reset the old clipping
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }
        else // Clipping was previously disabled
        {
            // Disable the clipping again
            glDisable(GL_SCISSOR_TEST);
        }
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

    void Gui::updateTime(const sf::Time& elapsedTime)
    {
        m_EventManager.updateTime(elapsedTime);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::addChildCallback(Callback& callback)
    {
        // If there is no global callback function then add the callback to the queue
        if (m_GlobalCallbackFunctions.empty())
            m_Callback.push(callback);
        else
        {
            // Loop through all callback functions and call them
            for (std::list< boost::function<void(const Callback&)> >::const_iterator it = m_GlobalCallbackFunctions.begin(); it != m_GlobalCallbackFunctions.end(); ++it)
                (*it)(callback);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Gui::getDisplaySize()
    {
        return Vector2f(m_Window->getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
