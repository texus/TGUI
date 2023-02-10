/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/TextureManager.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <TGUI/Exception.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<String, std::list<TextureDataHolder>> TextureManager::m_imageMap;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<TextureData> TextureManager::getTexture(Texture& texture, const String& filename, bool smooth)
    {
        // Let the texture alert the texture manager when it is being copied or destroyed
        texture.setCopyCallback(&TextureManager::copyTexture);
        texture.setDestructCallback(&TextureManager::removeTexture);

        const bool isSvg = ((filename.length() > 4) && (viewEqualIgnoreCase(StringView(filename.c_str() + (filename.length() - 4), 4), U".svg")));

        // Look if we already had this image
        auto imageIt = m_imageMap.find(filename);
        if (imageIt != m_imageMap.end())
        {
            // Loop all our textures to find the one containing the image
            for (auto& dataHolder : imageIt->second)
            {
                // We can reuse everything only if the image is loaded with the same settings
                if (dataHolder.smooth == smooth)
                {
                    // The exact same texture is now used at multiple places
                    ++dataHolder.users;
                    return dataHolder.data;
                }
            }
        }
        else // The image doesn't exist yet
        {
            auto it = m_imageMap.insert({filename, {}});
            imageIt = it.first;
        }

        // Add new data to the list
        TGUI_EMPLACE_BACK(dataHolder, imageIt->second)
        dataHolder.filename = filename;
        dataHolder.users = 1;
        dataHolder.smooth = smooth;
        dataHolder.data = std::make_shared<TextureData>();

        // Load the image
        auto data = imageIt->second.back().data;
        if (isSvg)
        {
            data->svgImage.emplace(filename);
            if (data->svgImage->isSet())
                return data;
        }
        else // Not an svg
        {
            data->backendTexture = getBackend()->createTexture();
            if (texture.getBackendTextureLoader()(*data->backendTexture, filename, smooth))
                return data;
        }

        // The image could not be loaded
        if (imageIt->second.size() > 1)
            imageIt->second.pop_back();
        else
            m_imageMap.erase(imageIt);

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::copyTexture(const std::shared_ptr<TextureData>& textureDataToCopy)
    {
        // Loop all our textures to check if we already have this one
        for (auto& dataHolder : m_imageMap)
        {
            for (auto& data : dataHolder.second)
            {
                // Check if the pointer points to our texture
                if (data.data == textureDataToCopy)
                {
                    // The texture is now used at multiple places
                    ++data.users;
                    return;
                }
            }
        }

        throw Exception{U"Trying to copy texture data that was not loaded by the TextureManager."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::removeTexture(const std::shared_ptr<TextureData>& textureDataToRemove)
    {
        // Loop all our textures to check which one it is
        for (auto imageIt = m_imageMap.begin(); imageIt != m_imageMap.end(); ++imageIt)
        {
            for (auto dataIt = imageIt->second.begin(); dataIt != imageIt->second.end(); ++dataIt)
            {
                // Check if the pointer points to our texture
                if (dataIt->data == textureDataToRemove)
                {
                    // If this was the only place where the texture is used then delete it
                    if (--(dataIt->users) == 0)
                    {
                        imageIt->second.erase(dataIt);
                        if (imageIt->second.empty())
                            m_imageMap.erase(imageIt);
                    }

                    return;
                }
            }
        }

        throw Exception{U"Trying to remove a texture that was not loaded by the TextureManager."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextureManager::getCachedImagesCount()
    {
        return m_imageMap.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
