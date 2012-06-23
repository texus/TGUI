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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel() :
    backgroundColor(sf::Color::Transparent)
    {
        m_ObjectType = panel;
        m_EventManager.m_Parent = this;
        m_RenderTexture = new sf::RenderTexture();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Panel& copy) :
    OBJECT         (copy),
    Group          (copy),
    backgroundColor(copy.backgroundColor)
    {
        // Copy the render texture
        if (m_RenderTexture->create(copy.m_RenderTexture->getSize().x, copy.m_RenderTexture->getSize().y) == false)
            m_Loaded = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel& Panel::operator= (const Panel& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Panel temp(right);
            this->OBJECT::operator=(right);
            this->Group::operator=(right);

            std::swap(backgroundColor, temp.backgroundColor);
            std::swap(m_RenderTexture, temp.m_RenderTexture);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::load(unsigned int width, unsigned int height, const sf::Color& bkgColor)
    {
        // Until the loading succeeds, the panel will be marked as unloaded
        m_Loaded = false;

        // Set the background color of the panel
        backgroundColor = bkgColor;

        // Create the render texture
        if (m_RenderTexture->create(width, height))
        {
            m_Loaded = true;
            return true;
        }
        else // The creation of the render texture failed
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setSize(float width, float height)
    {
        // Recreate the render texture
        if (m_RenderTexture->create(static_cast<unsigned int>(width), static_cast<unsigned int>(height)))
            m_Loaded = true;
        else
            m_Loaded = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::addCallback(Callback& callback)
    {
        // Pass the callback to the parent. It has to get to the main window eventually.
        m_Parent->addCallback(callback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::handleEvent(sf::Event& event)
    {
        // Check if the event is a mouse move or mouse down/press
        if (event.type == sf::Event::MouseMoved)
        {
            // Adjust the mouse position of the event
            event.mouseMove.x -= static_cast<int>(getPosition().x);
            event.mouseMove.y -= static_cast<int>(getPosition().y);
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            // Adjust the mouse position of the event
            event.mouseButton.x -= static_cast<int>(getPosition().x);
            event.mouseButton.y -= static_cast<int>(getPosition().y);

            // Mark the mouse as down
            m_MouseDown = true;
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            // Adjust the mouse position of the event
            event.mouseButton.x -= static_cast<int>(getPosition().x);
            event.mouseButton.y -= static_cast<int>(getPosition().y);

            // Mark the mouse as up
            m_MouseDown = false;
        }

        // Let the event manager handle the event
        m_EventManager.handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Panel::getSize() const
    {
        if (m_Loaded == true)
            return m_RenderTexture->getSize();
        else
            return Vector2u(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getScaledSize() const
    {
        if (m_Loaded == true)
            return Vector2f(m_RenderTexture->getSize().x * getScale().x, m_RenderTexture->getSize().y * getScale().y);
        else
            return Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnObject(float x, float y)
    {
        // Don't continue when the panel has not been loaded yet
        if (m_Loaded == false)
            return false;

        // Get the position
        Vector2f position = getPosition();

        // Check if the mouse is inside the panel
        if ((x > position.x) && (x < position.x + m_RenderTexture->getSize().x) && (y > position.y) && (y < position.y + m_RenderTexture->getSize().y))
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::objectUnfocused()
    {
        m_EventManager.unfocusAllObjects();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the texture wasn't created
        if (m_Loaded == false)
            return;

        // Clear the texture
        m_RenderTexture->clear(backgroundColor);

        // Draw the objects on the texture
        drawObjectGroup(m_RenderTexture, sf::RenderStates::Default);

        // Display the texture
        m_RenderTexture->display();

        // Make a copy of the render states
        sf::RenderStates statesCopy = states;

        // Adjust the transformation
        statesCopy.transform *= getTransform();

        // Draw the panel on the window
        sf::Sprite sprite(m_RenderTexture->getTexture());
        target.draw(sprite, statesCopy);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

