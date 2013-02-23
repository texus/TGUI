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


#include <TGUI/Defines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::getTexture(const std::string& filename, sf::Texture*& textureToLoad)
    {
        // Loop all our textures to check if we already have this one
        for (std::list<TextureData>::iterator it = m_Data.begin(); it != m_Data.end(); ++it)
        {
            // Check if the filename matches
            if (it->filename.compare(filename) == 0)
            {
                // The texture is now used at multiple places
                ++it->users;

                // We already have the texture, so we can just pass it
                textureToLoad = &it->texture;
                return true;
            }
        }

        // Add new data to the list
        m_Data.push_back(TextureData());
        TextureData& data = m_Data.back();

        // load the image
        if (data.image.loadFromFile(filename))
        {
            // Create a texture from the image
            if (data.texture.loadFromImage(data.image))
            {
                data.filename = filename;
                data.users = 1;

                textureToLoad = &data.texture;
                return true;
            }
        }

        m_Data.pop_back();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::copyTexture(const sf::Texture* const textureToCopy, sf::Texture*& newTexture)
    {
        // Loop all our textures to check if we already have this one
        for (std::list<TextureData>::iterator it = m_Data.begin(); it != m_Data.end(); ++it)
        {
            // Check if the pointer points to our texture
            if (&it->texture == textureToCopy)
            {
                // The texture is now used at multiple places
                ++it->users;
                newTexture = &it->texture;
                return true;
            }
        }

        // We didn't have the texture and we can't store it without a filename
        newTexture = NULL;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::removeTexture(sf::Texture*& textureToRemove)
    {
        // Loop all our textures to check which one it is
        for (std::list<TextureData>::iterator it = m_Data.begin(); it != m_Data.end(); ++it)
        {
            // Check if the pointer points to our texture
            if (&it->texture == textureToRemove)
            {
                // If this was the only place where the texture is used then delete it
                if (--it->users == 0)
                {
                    // Remove the texture from the list
                    m_Data.erase(it);

                    // The pointer is now useless
                    textureToRemove = NULL;
                    return;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::isTransparentPixel(const sf::Texture* texture, unsigned int x, unsigned int y)
    {
        // Loop all our textures to check which one it is
        for (std::list<TextureData>::const_iterator it = m_Data.begin(); it != m_Data.end(); ++it)
        {
            if (&it->texture == texture)
            {
                // Check if the pixel is transparent
                if (it->image.getPixel(x, y).a == 0)
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
