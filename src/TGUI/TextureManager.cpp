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

    TextureManager::~TextureManager()
    {
        // Remove all textures (if the TextureManager is used correctly then there shouldn't be any more textures)
        for (unsigned int x=0; x<m_Texture.size(); ++x)
        {
            delete m_Texture[x];

            m_Filename.erase(m_Filename.begin()+x);
            m_Texture.erase(m_Texture.begin()+x);
            m_Users.erase(m_Users.begin()+x);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::getTexture(const std::string filename, sf::Texture*& textureToLoad)
    {
        // Loop all our textures to check if we already have this one
        for (unsigned int x=0; x<m_Filename.size(); ++x)
        {
            // Check if the filename matches
            if (m_Filename.at(x).compare(filename) == 0)
            {
                // The texture is now used at multiple places
                ++m_Users.at(x);

                // We already have the texture, so we can just pass it
                textureToLoad = m_Texture.at(x);
                return true;
            }
        }

        // Create a new texture
        textureToLoad = new sf::Texture();

        // load the texture
        if (textureToLoad->loadFromFile(filename))
        {
            // add the texture to the list
            m_Texture.push_back(textureToLoad);
            m_Filename.push_back(filename);
            m_Users.push_back(1);

            return true;
        }
        else // The texture could not be loaded
        {
            // Delete it
            delete textureToLoad;
            textureToLoad = NULL;

            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::copyTexture(sf::Texture* textureToCopy, sf::Texture*& newTexture)
    {
        // The two textures must be exactly the same
        newTexture = textureToCopy;

        // If the texture is NULL then don't even search
        if (textureToCopy == NULL)
            return false;

        // Loop all our textures to check if we already have this one
        for (unsigned int x=0; x<m_Texture.size(); ++x)
        {
            // Check if the pointer points to our texture
            if (m_Texture[x] == textureToCopy)
            {
                // The texture is now used at multiple places
                ++m_Users[x];

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
        // If the texture is NULL then it has already been deleted
        if (textureToRemove == NULL)
            return;

        // Loop all our textures to check which one it is
        for (unsigned int x=0; x<m_Texture.size(); ++x)
        {
            // Check if the pointer points to our texture
            if (m_Texture[x] == textureToRemove)
            {
                // If this was the only place where the texture is used then delete it
                if (--m_Users[x] == 0)
                {
                    // Delete the texture
                    delete m_Texture[x];
                    textureToRemove = NULL;

                    // Remove the texture from the list
                    m_Filename.erase(m_Filename.begin()+x);
                    m_Texture.erase(m_Texture.begin()+x);
                    m_Users.erase(m_Users.begin()+x);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
