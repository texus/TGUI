/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Form::Form(sf::RenderWindow* window)
    {
        m_Window = window;

        // The main window is always focused
        m_GroupFocused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Form::~Form()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Form::handleEvent(sf::Event& event)
    {
        // Check if the event is a mouse move or mouse down/press
        if (event.type == sf::Event::MouseMoved)
        {
            Vector2f mouseCoords = m_Window->convertCoords(Vector2i(event.mouseMove.x, event.mouseMove.y));

            // Adjust the mouse position of the event
            event.mouseMove.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseMove.y = static_cast<int>(mouseCoords.y + 0.5f);
        }
        else if ((event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::MouseButtonReleased))
        {
            Vector2f mouseCoords = m_Window->convertCoords(Vector2i(event.mouseButton.x, event.mouseButton.y));

            // Adjust the mouse position of the event
            event.mouseButton.x = static_cast<int>(mouseCoords.x + 0.5f);
            event.mouseButton.y = static_cast<int>(mouseCoords.y + 0.5f);
        }

        // Let the event manager handle the event
        m_EventManager.handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Form::draw(const sf::RenderStates& states)
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
        drawObjectGroup(m_Window, states);

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

    bool Form::getCallback(Callback& callback)
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

    void Form::addCallback(Callback& callback)
    {
        // Add the callback to the list
        m_Callback.push(callback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
