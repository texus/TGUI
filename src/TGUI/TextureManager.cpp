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

    bool TextureManager::getTexture(const std::string filename, sf::Texture*& textureToLoad)
    {
        // Loop all our textures to check if we already have this one
        std::list<sf::Texture>::iterator it = m_Textures.begin();
        for (unsigned int i=0; i<m_Filenames.size(); ++i, ++it)
        {
            // Check if the filename matches
            if (m_Filenames[i].compare(filename) == 0)
            {
                // The texture is now used at multiple places
                ++m_Users[i];

                // We already have the texture, so we can just pass it
                textureToLoad = &*it;
                return true;
            }
        }

        // Add a new image to the list
        m_Images.push_back(sf::Image());

        // load the image
        if (m_Images.back().loadFromFile(filename))
        {
            // Create a texture from the image
            m_Textures.push_back(sf::Texture());
            if (m_Textures.back().loadFromImage(m_Images.back()))
            {
                m_Filenames.push_back(filename);
                m_Users.push_back(1);

                textureToLoad = &m_Textures.back();
                return true;
            }
            else // The texture couldn't be created
            {
                m_Images.pop_back();
                m_Textures.pop_back();
                return false;
            }
        }
        else // The image could not be loaded
        {
            m_Images.pop_back();
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::copyTexture(sf::Texture* const textureToCopy, sf::Texture*& newTexture)
    {
        // The two textures must be exactly the same
        newTexture = textureToCopy;

        // If the texture is NULL then don't even search
        if (textureToCopy == NULL)
            return false;

        // Loop all our textures to check if we already have this one
        std::list<sf::Texture>::iterator it = m_Textures.begin();
        for (unsigned int i=0; i<m_Textures.size(); ++i, ++it)
        {
            // Check if the pointer points to our texture
            if (&*it == textureToCopy)
            {
                // The texture is now used at multiple places
                ++m_Users[i];

                // We already had the texture so return true
                return true;
            }
        }

        // We didn't have the texture and we can't store it without a filename
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::removeTexture(sf::Texture*& textureToRemove)
    {
        // If the texture is NULL then it has already been removed
        if (textureToRemove == NULL)
            return;

        // Loop all our textures to check which one it is
        std::list<sf::Texture>::iterator it = m_Textures.begin();
        for (unsigned int i=0; i<m_Textures.size(); ++i, ++it)
        {
            // Check if the pointer points to our texture
            if (&*it == textureToRemove)
            {
                // If this was the only place where the texture is used then delete it
                if (--m_Users[i] == 0)
                {
                    // Remove the texture from the list
                    m_Filenames.erase(m_Filenames.begin()+i);
                    m_Images.erase(m_Images.begin()+i);
                    m_Textures.erase(it);
                    m_Users.erase(m_Users.begin()+i);

                    // The pointer is now useless
                    textureToRemove = NULL;

                    break;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::isTransparentPixel(const sf::Texture* const texture, unsigned int x, unsigned int y)
    {
        if (texture == NULL)
            return false;

        // Loop all our textures to check which one it is
        std::list<sf::Texture>::iterator it = m_Textures.begin();
        for (unsigned int i=0; i<m_Textures.size(); ++i, ++it)
        {
            if (&*it == texture)
            {
                // Check if the pixel is transparent
                if (m_Images[i].getPixel(x, y).a == 0)
                    return true;
                else
                    return false;
            }
        }

        // None of the textures matches
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
