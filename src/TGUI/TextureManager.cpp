/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Global.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture() :
    data(nullptr)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2u Texture::getSize() const
    {
        if (data != nullptr)
            return data->texture.getSize();
        else
            return sf::Vector2u(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isTransparentPixel(unsigned int x, unsigned int y)
    {
        if (data->image->getPixel(x + data->rect.left, y + data->rect.top).a == 0)
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::getTexture(const std::string& filename, Texture& texture, const sf::IntRect& rect)
    {
        // Look if we already had this image
        auto imageIt = m_ImageMap.find(filename);
        if (imageIt != m_ImageMap.end())
        {
            // Loop all our textures to find the one containing the image
            for (std::list<TextureData>::iterator it = imageIt->second.data.begin(); it != imageIt->second.data.end(); ++it)
            {
                // Only reuse the texture when the exact same part of the image is used
                if (it->rect == rect)
                {
                    // The texture is now used at multiple places
                    ++(it->users);

                    // We already have the texture, so pass the data
                    texture.data = &(*it);

                    // Set the texture in the sprite
                    texture.sprite.setTexture(it->texture, true);

                    return true;
                }
            }
        }
        else // The image doesn't exist yet
        {
            auto it = m_ImageMap.insert(std::make_pair(filename, ImageMapData()));
            imageIt = it.first;
        }

        // Add new data to the list
        imageIt->second.data.push_back(TextureData());
        texture.data = &imageIt->second.data.back();
        texture.data->image = &imageIt->second.image;
        texture.data->rect = rect;

        // load the image
        if (texture.data->image->loadFromFile(filename))
        {
            // Create a texture from the image
            bool success;
            if (rect == sf::IntRect(0, 0, 0, 0))
                success = texture.data->texture.loadFromImage(*texture.data->image);
            else
                success = texture.data->texture.loadFromImage(*texture.data->image, rect);

            if (success)
            {
                // Set the texture in the sprite
                texture.sprite.setTexture(texture.data->texture, true);

                // Set the other members of the data
                texture.data->filename = filename;
                texture.data->users = 1;
                return true;
            }
        }

        // The image couldn't be loaded
        m_ImageMap.erase(imageIt);
        texture.data = nullptr;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextureManager::copyTexture(const Texture& textureToCopy, Texture& newTexture)
    {
        // Ignore null pointers
        if (textureToCopy.data == nullptr)
        {
            newTexture.data = nullptr;
            return true;
        }

        // Loop all our textures to check if we already have this one
        for (auto imageIt = m_ImageMap.begin(); imageIt != m_ImageMap.end(); ++imageIt)
        {
            for (auto dataIt = imageIt->second.data.begin(); dataIt != imageIt->second.data.end(); ++dataIt)
            {
                // Check if the pointer points to our texture
                if (&(*dataIt) == textureToCopy.data)
                {
                    // The texture is now used at multiple places
                    ++(dataIt->users);
                    newTexture = textureToCopy;
                    return true;
                }
            }
        }

        TGUI_OUTPUT("TGUI warning: Can't copy texture that wasn't loaded by TextureManager.");
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::removeTexture(Texture& textureToRemove)
    {
        // Loop all our textures to check which one it is
        for (auto imageIt = m_ImageMap.begin(); imageIt != m_ImageMap.end(); ++imageIt)
        {
            for (auto dataIt = imageIt->second.data.begin(); dataIt != imageIt->second.data.end(); ++dataIt)
            {
                // Check if the pointer points to our texture
                if (&(*dataIt) == textureToRemove.data)
                {
                    // If this was the only place where the texture is used then delete it
                    if (--(dataIt->users) == 0)
                    {
                        // Remove the texture from the list, or even the whole image if it isn't used anywhere else
                        int usage = std::count_if(imageIt->second.data.begin(), imageIt->second.data.end(), [dataIt](TextureData& data){ return data.image == dataIt->image; });
                        if (usage == 1)
                            m_ImageMap.erase(imageIt);
                        else
                            imageIt->second.data.erase(dataIt);
                    }

                    // The pointer is now useless
                    textureToRemove.data = nullptr;
                    return;
                }
            }
        }

        TGUI_OUTPUT("TGUI warning: Can't remove texture that wasn't loaded by TextureManager.");
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
