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
#include <TGUI/ClickableObject.hpp>
#include <TGUI/Picture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture()
    {
        m_Callback.objectType = Type_Picture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture(const Picture& copy) :
    ClickableObject (copy),
    m_LoadedFilename(copy.m_LoadedFilename)
    {
        // Copy the texture
        TGUI_TextureManager.copyTexture(copy.m_Texture, m_Texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::~Picture()
    {
        // Remove the texture (if we are the only one using it)
        if (m_Texture.data != NULL)
            TGUI_TextureManager.removeTexture(m_Texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture& Picture::operator= (const Picture& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Picture temp(right);
            this->ClickableObject::operator=(right);

            std::swap(m_LoadedFilename, temp.m_LoadedFilename);
            std::swap(m_Texture,        temp.m_Texture);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture* Picture::clone()
    {
        return new Picture(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::load(const std::string& filename)
    {
        m_LoadedFilename = filename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;
        m_Size.x = 0;
        m_Size.y = 0;

        // Make sure that the filename isn't empty
        if (filename.empty())
            return false;

        // If we have already loaded a texture then first delete it
        if (m_Texture.data != NULL)
            TGUI_TextureManager.removeTexture(m_Texture);

        // Try to load the texture from the file
        if (TGUI_TextureManager.getTexture(filename, m_Texture))
        {
            // Remember the size of the texture
            m_Size = Vector2f(m_Texture.getSize());

            // Return true to indicate that nothing went wrong
            return m_Loaded = true;
        }
        else // The texture was not loaded
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Picture::getLoadedFilename()
    {
        return m_LoadedFilename;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setSmooth(bool smooth)
    {
        if (m_Loaded)
            m_Texture.data->texture.setSmooth(smooth);
        else
            TGUI_OUTPUT("TGUI warning: Picture::setSmooth called while Picture wasn't loaded yet.")
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::isSmooth() const
    {
        if (m_Loaded)
            return m_Texture.data->texture.isSmooth();
        else
        {
            TGUI_OUTPUT("TGUI warning: Picture::isSmooth called while Picture wasn't loaded yet.");
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::mouseOnObject(float x, float y)
    {
        // Don't do anything when the image wasn't loaded
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the picture
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
        {
            Vector2f scaling;
            scaling.x = m_Size.x / m_Texture.getSize().x * getScale().x;
            scaling.y = m_Size.y / m_Texture.getSize().y * getScale().y;

            // Only return true when the pixel under the mouse isn't transparent
            if (!m_Texture.isTransparentPixel(static_cast<unsigned int>((x - getPosition().x) / scaling.x), static_cast<unsigned int>((y - getPosition().y) / scaling.y)))
                return true;
        }

        if (m_MouseHover == true)
            mouseLeftObject();

        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't continue when the picture wasn't loaded correctly
        if (m_Loaded == false)
            return;

        states.transform *= getTransform();
        states.transform.scale(m_Size.x / m_Texture.getSize().x, m_Size.y / m_Texture.getSize().y);
        target.draw(m_Texture, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
