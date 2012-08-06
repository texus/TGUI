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

    Picture::Picture() :
    m_Texture       (NULL),
    m_LoadedFilename("")
    {
        m_ObjectType = picture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture(const Picture& copy) :
    OBJECT          (copy),
    m_LoadedFilename(copy.m_LoadedFilename)
    {
        // Copy the texture
        if (TGUI_TextureManager.copyTexture(copy.m_Texture, m_Texture))
            m_Sprite.setTexture(*m_Texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::~Picture()
    {
        // Remove the texture (if we are the only one using it)
        if (m_Texture != NULL)
            TGUI_TextureManager.removeTexture(m_Texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture& Picture::operator= (const Picture& right)
    {
        TGUI_UNUSED_PARAM(right);

        // Make sure it is not the same object
        if (this != &right)
        {
            Picture temp(right);
            this->OBJECT::operator=(right);

            std::swap(m_Texture,        temp.m_Texture);
            std::swap(m_Sprite,         temp.m_Sprite);
            std::swap(m_LoadedFilename, temp.m_LoadedFilename);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture* Picture::clone()
    {
        return new Picture(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::load(const std::string filename)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Make sure that the filename isn't empty
        if (filename.empty())
            return false;

        // Store the filename
        m_LoadedFilename = filename;

        // If we have already loaded a texture then first delete it
        if (m_Texture != NULL)
            TGUI_TextureManager.removeTexture(m_Texture);

        // Try to load the texture from the file
        if (TGUI_TextureManager.getTexture(filename, m_Texture))
        {
            // Set the texture
            m_Sprite.setTexture(*m_Texture);

            // Return true to indicate that nothing went wrong
            m_Loaded = true;
            return true;
        }
        else // The texture was not loaded
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setSize(float width, float height)
    {
        // Make sure that the picture was already loaded
        if (m_Loaded == false)
            return;

        // Set the new scale
        setScale(width / m_Texture->getSize().x, height / m_Texture->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Picture::getSize() const
    {
        if (m_Loaded)
            return Vector2u(m_Texture->getSize().x, m_Texture->getSize().y);
        else
            return Vector2u(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Picture::getScaledSize() const
    {
        if (m_Loaded)
            return Vector2f(m_Texture->getSize().x * getScale().x, m_Texture->getSize().y * getScale().y);
        else
            return Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Picture::getLoadedFilename()
    {
        return m_LoadedFilename;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::mouseOnObject(float x, float y)
    {
        // Don't do anything when the image wasn't loaded
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the picture
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(getSize().x), static_cast<float>(getSize().y))).contains(x, y))
        {
            // Only return true when the pixel under the mouse isn't transparent
            return !TGUI_TextureManager.isTransparentPixel(m_Texture, static_cast<unsigned int>(x / getScale().x - getPosition().x), static_cast<unsigned int>(y / getScale().y - getPosition().y));
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::leftMousePressed(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        // Set the mouse down flag
        m_MouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::leftMouseReleased(float x, float y)
    {
        // Check if we clicked on the picture (not just mouse release)
        if (m_MouseDown == true)
        {
            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID  = callbackID;
                callback.trigger     = Callback::mouseClick;
                callback.mouseButton = sf::Mouse::Left;
                callback.mouseX      = x - getPosition().x;
                callback.mouseY      = y - getPosition().y;
                m_Parent->addCallback(callback);
            }

            m_MouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(m_Sprite, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
