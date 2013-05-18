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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture() :
    data(NULL)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2u Texture::getSize() const
    {
        return sf::Vector2u(sprite.getTextureRect().width, sprite.getTextureRect().height);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Texture::getWidth() const
    {
        return sprite.getTextureRect().width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Texture::getHeight() const
    {
        return sprite.getTextureRect().height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isTransparentPixel(unsigned int x, unsigned int y)
    {
        if (data->image.getPixel(sprite.getTextureRect().left + x, sprite.getTextureRect().top + y).a == 0)
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::operator const sf::Sprite&() const
    {
        return sprite;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::getTexture(const std::string& filename, Texture& texture, const sf::IntRect& rect)
    {
        // Loop all our textures to check if we already have this one
        for (std::list<TextureData>::iterator it = m_Data.begin(); it != m_Data.end(); ++it)
        {
            // Check if the filename matches
            if (it->filename == filename)
            {
                // The texture is now used at multiple places
                ++(it->users);

                // We already have the texture, so pass the data
                texture.data = &(*it);

                // Set the texture in the sprite
                texture.sprite.setTexture(it->texture, true);

                // Set only a part of the texture when asked
                if (rect != sf::IntRect(0, 0, 0, 0))
                    texture.sprite.setTextureRect(rect);

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
                // Set the texture in the sprite
                texture.sprite.setTexture(data.texture, true);

                // Set only a part of the texture when asked
                if (rect != sf::IntRect(0, 0, 0, 0))
                    texture.sprite.setTextureRect(rect);

                // Set the other members of the data
                data.filename = filename;
                data.users = 1;

                // Pass the pointer
                texture.data = &data;
                return true;
            }
        }

        // The image couldn't be loaded
        m_Data.pop_back();
        texture.data = NULL;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::copyTexture(const Texture& textureToCopy, Texture& newTexture)
    {
        // Loop all our textures to check if we already have this one
        for (std::list<TextureData>::iterator it = m_Data.begin(); it != m_Data.end(); ++it)
        {
            // Check if the pointer points to our texture
            if (&(*it) == textureToCopy.data)
            {
                // The texture is now used at multiple places
                ++(it->users);
                newTexture = textureToCopy;
                return true;
            }
        }

        // We didn't have the texture and we can't store it without a filename
        newTexture.data = NULL;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::removeTexture(Texture& textureToRemove)
    {
        // Loop all our textures to check which one it is
        for (std::list<TextureData>::iterator it = m_Data.begin(); it != m_Data.end(); ++it)
        {
            // Check if the pointer points to our texture
            if (&(*it) == textureToRemove.data)
            {
                // If this was the only place where the texture is used then delete it
                if (--(it->users) == 0)
                {
                    // Remove the texture from the list
                    m_Data.erase(it);
                    break;
                }
            }
        }

        // The pointer is now useless
        textureToRemove.data = NULL;
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
