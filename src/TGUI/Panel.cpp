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

    Panel::Panel() :
    backgroundColor                (sf::Color::Transparent),
    m_Size                         (100, 100),
    m_Texture                      (NULL),
    m_LoadedBackgroundImageFilename("")
    {
        m_ObjectType = panel;
        m_EventManager.m_Parent = this;

        m_AllowFocus = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Panel& panelToCopy) :
    OBJECT                         (panelToCopy),
    Group                          (panelToCopy),
    backgroundColor                (panelToCopy.backgroundColor),
    m_Size                         (panelToCopy.m_Size),
    m_LoadedBackgroundImageFilename(panelToCopy.m_LoadedBackgroundImageFilename)
    {
        // Copy the texture of te background image
        if (TGUI_TextureManager.copyTexture(panelToCopy.m_Texture, m_Texture))
            m_Sprite.setTexture(*m_Texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::~Panel()
    {
        // Delete the texture of our background image
        if (m_Texture != NULL)
            TGUI_TextureManager.removeTexture(m_Texture);
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

            std::swap(backgroundColor,                 temp.backgroundColor);
            std::swap(m_Size,                          temp.m_Size);
            std::swap(m_Texture,                       temp.m_Texture);
            std::swap(m_Sprite,                        temp.m_Sprite);
            std::swap(m_LoadedBackgroundImageFilename, temp.m_LoadedBackgroundImageFilename);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::initialize(const sf::Font& font)
    {
        globalFont = font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel* Panel::clone()
    {
        return new Panel(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::load(unsigned int width, unsigned int height, const sf::Color& bkgColor, const std::string filename)
    {
        // Until the loading succeeds, the panel will be marked as unloaded
        m_Loaded = false;

        // Set the background color of the panel
        backgroundColor = bkgColor;

        // Store the filename
        m_LoadedBackgroundImageFilename = filename;

        // Set the size of the panel
        m_Size.x = width;
        m_Size.y = height;

        // If we have already loaded a background image then first delete it
        if (m_Texture != NULL)
            TGUI_TextureManager.removeTexture(m_Texture);

        // Check if a filename was given
        if (filename.empty() == false)
        {
            // Try to load the texture from the file
            if (TGUI_TextureManager.getTexture(filename, m_Texture))
            {
                // Set the texture for the sprite
                m_Sprite.setTexture(*m_Texture, true);

                // Set the size of the sprite
                m_Sprite.setScale(static_cast<float>(width) / m_Texture->getSize().x, static_cast<float>(height) / m_Texture->getSize().y);

                // Return true to indicate that nothing went wrong
                m_Loaded = true;
                return true;
            }
            else // The texture was not loaded
                return false;
        }
        else // No image has to be loaded
        {
            m_Loaded = true;
            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setSize(float width, float height)
    {
        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the panel
        m_Size.x = width;
        m_Size.y = height;

        // If there is no background image then te panel can be considered loaded
        if (m_LoadedBackgroundImageFilename.empty())
            m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::addCallback(Callback& callback)
    {
        // Pass the callback to the parent. It has to get to the main window eventually.
        m_Parent->addCallback(callback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::handleEvent(sf::Event& event, const float mouseX, const float mouseY)
    {
        // Check if the event is a mouse move or mouse down/press
        if (event.type == sf::Event::MouseMoved)
        {
            // Adjust the mouse position of the event
            event.mouseMove.x = mouseX - static_cast<int>(getPosition().x);
            event.mouseMove.y = mouseY - static_cast<int>(getPosition().y);
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            // Adjust the mouse position of the event
            event.mouseButton.x = mouseX - static_cast<int>(getPosition().x);
            event.mouseButton.y = mouseY - static_cast<int>(getPosition().y);
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            // Adjust the mouse position of the event
            event.mouseButton.x = mouseX - static_cast<int>(getPosition().x);
            event.mouseButton.y = mouseY - static_cast<int>(getPosition().y);
        }

        // Let the event manager handle the event
        m_EventManager.handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::focusNextObject()
    {
        return m_EventManager.focusNextObject();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Panel::getSize() const
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getScaledSize() const
    {
        return Vector2f(m_Size.x * getScale().x, m_Size.y * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::setBackgroundImage(const std::string filename)
    {
        // Remember the background image filename
        m_LoadedBackgroundImageFilename = filename;

        // If we have already loaded a background image then first delete it
        if (m_Texture != NULL)
            TGUI_TextureManager.removeTexture(m_Texture);

        // Try to load the texture from the file
        if (TGUI_TextureManager.getTexture(filename, m_Texture))
        {
            // Set the texture for the sprite
            m_Sprite.setTexture(*m_Texture, true);

            // Set the size of the sprite
            m_Sprite.setScale(static_cast<float>(m_Size.x) / m_Texture->getSize().x, static_cast<float>(m_Size.y) / m_Texture->getSize().y);

            m_Loaded = true;
            return true;
        }
        else // The texture was not loaded
        {
            m_Loaded = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Panel::getLoadedBackgroundImageFilename()
    {
        return m_LoadedBackgroundImageFilename;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnObject(float x, float y)
    {
        // Don't continue when the panel has not been loaded yet
        if (m_Loaded == false)
            return false;

        // Check if the mouse is inside the panel
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), static_cast<float>(getSize().y))).contains(x, y))
            return true;
        else
        {
            // Tell the objects inside the panel that the mouse is no longer on top of them
            m_EventManager.mouseNotOnObject();
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::objectFocused()
    {
        m_EventManager.tabKeyPressed();
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

        // Set the transform
        states.transform *= getTransform();

        // Draw the background
        sf::RectangleShape background(Vector2f(static_cast<float>(m_Size.x),  static_cast<float>(m_Size.y)));
        background.setFillColor(backgroundColor);
        target.draw(background, states);

        // Draw the background image if there is one
        if (m_Texture != NULL)
            target.draw(m_Sprite, states);

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the global translation
        sf::Vector2f globalTranslation = states.transform.transformPoint(getPosition());

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Set the clipping area
        glScissor((globalTranslation.x - target.getView().getCenter().x + (target.getView().getSize().x / 2.f)) * scaleViewX,
                  target.getSize().y - ((globalTranslation.y + m_Size.y - target.getView().getCenter().y + (target.getView().getSize().y / 2.f)) * scaleViewY),
                  m_Size.x * scaleViewX, m_Size.y * scaleViewY);

        // Draw the objects
        drawObjectGroup(&target, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

