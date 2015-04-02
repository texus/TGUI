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


#include <TGUI/Texture.hpp>
#include <TGUI/TextureManager.hpp>
#include <TGUI/Global.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<ImageLoader> TextureManager::m_imageLoader = std::make_shared<ImageLoader>();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::getTexture(Texture& texture, const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middleRect, bool repeated)
    {
        // Look if we already had this image
        auto imageIt = m_imageMap.find(filename);
        if (imageIt != m_imageMap.end())
        {
            // Loop all our textures to find the one containing the image
            for (std::list<TextureData>::iterator it = imageIt->second.data.begin(); it != imageIt->second.data.end(); ++it)
            {
                // Only reuse the texture when the exact same part of the image is used
                if (it->rect == partRect)
                {
                    // The texture is now used at multiple places
                    ++(it->users);

                    // Set the texture in the sprite
                    if (middleRect != sf::IntRect{})
                        texture.setTexture(*it, middleRect);
                    else
                        texture.setTexture(*it, {0, 0, static_cast<int>(it->texture.getSize().x), static_cast<int>(it->texture.getSize().y)});

                    return;
                }
            }
        }
        else // The image doesn't exist yet
        {
            auto it = m_imageMap.insert({filename, ImageMapData()});
            imageIt = it.first;
        }

        // Add new data to the list
        TextureData data;
        data.rect = partRect;
        data.image = &imageIt->second.image;

        // Load the image
        if (m_imageLoader->load(filename, *data.image))
        {
            // Create a texture from the image
            bool success;
            if (partRect == sf::IntRect{})
                success = data.texture.loadFromImage(*data.image);
            else
                success = data.texture.loadFromImage(*data.image, partRect);

            if (success)
            {
                if (repeated)
                    data.texture.setRepeated(repeated);

                // Set the other members of the data
                data.filename = filename;
                data.users = 1;

                // Set the texture in the sprite
                imageIt->second.data.push_back(std::move(data));

                if (middleRect != sf::IntRect{})
                    texture.setTexture(imageIt->second.data.back(), middleRect);
                else
                    texture.setTexture(imageIt->second.data.back(), {0, 0, static_cast<int>(data.texture.getSize().x), static_cast<int>(data.texture.getSize().y)});

                return;
            }
        }

        // The image could not be loaded
        m_imageMap.erase(imageIt);
        throw Exception{"Failed to load image " + filename + "."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::copyTexture(const Texture& textureToCopy)
    {
        // Loop all our textures to check if we already have this one
        for (auto imageIt = m_imageMap.begin(); imageIt != m_imageMap.end(); ++imageIt)
        {
            for (auto dataIt = imageIt->second.data.begin(); dataIt != imageIt->second.data.end(); ++dataIt)
            {
                // Check if the pointer points to our texture
                if (&(*dataIt) == textureToCopy.getData())
                {
                    // The texture is now used at multiple places
                    ++(dataIt->users);
                    return;
                }
            }
        }

        throw Exception{"Trying to copy a texture that was not loaded by the TextureManager."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::removeTexture(const Texture& textureToRemove)
    {
        // Loop all our textures to check which one it is
        for (auto imageIt = m_imageMap.begin(); imageIt != m_imageMap.end(); ++imageIt)
        {
            for (auto dataIt = imageIt->second.data.begin(); dataIt != imageIt->second.data.end(); ++dataIt)
            {
                // Check if the pointer points to our texture
                if (&(*dataIt) == textureToRemove.getData())
                {
                    // If this was the only place where the texture is used then delete it
                    if (--(dataIt->users) == 0)
                    {
                        // Remove the texture from the list, or even the whole image if it isn't used anywhere else
                        int usage = std::count_if(imageIt->second.data.begin(), imageIt->second.data.end(), [dataIt](TextureData& data){ return data.image == dataIt->image; });
                        if (usage == 1)
                            m_imageMap.erase(imageIt);
                        else
                            imageIt->second.data.erase(dataIt);
                    }

                    return;
                }
            }
        }

        throw Exception{"Trying to remove a texture that was not loaded by the TextureManager."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TextureManager::setImageLoader(std::shared_ptr<ImageLoader> imageLoader)
	{
		if (imageLoader != nullptr)
			m_imageLoader = imageLoader;
	}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool ImageLoader::load(const std::string& filename, sf::Image& image)
	{
		return image.loadFromFile(filename);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
