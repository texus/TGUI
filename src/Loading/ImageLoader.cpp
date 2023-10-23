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


#include <TGUI/Loading/ImageLoader.hpp>
#include <TGUI/Global.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/extlibs/IncludeStbImage.hpp>
#endif

#include <cstring> // memcpy

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<std::uint8_t[]> ImageLoader::loadFromFile(const String& filename, Vector2u& imageSize)
    {
        std::size_t fileSize;
        auto fileContents = readFileToMemory(filename, fileSize);
        if (!fileContents)
            return nullptr;

        return loadFromMemory(fileContents.get(), fileSize, imageSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<std::uint8_t[]> ImageLoader::loadFromMemory(const std::uint8_t* data, std::size_t dataSize, Vector2u& imageSize)
    {
        std::unique_ptr<std::uint8_t[]> pixelData = nullptr;

        int imgWidth;
        int imgHeight;
        int imgChannels;
        stbi_uc* buffer = stbi_load_from_memory(static_cast<const stbi_uc*>(data), static_cast<int>(dataSize), &imgWidth, &imgHeight, &imgChannels, 4);
        if (!buffer)
            return pixelData;

        if ((imgWidth > 0) && (imgHeight > 0))
        {
            const std::size_t pixelDataSize = 4 * static_cast<std::size_t>(imgWidth) * static_cast<std::size_t>(imgHeight);
            pixelData = MakeUniqueForOverwrite<std::uint8_t[]>(pixelDataSize);
            std::memcpy(pixelData.get(), buffer, pixelDataSize);

            imageSize.x = static_cast<unsigned int>(imgWidth);
            imageSize.y = static_cast<unsigned int>(imgHeight);
        }

        stbi_image_free(buffer);
        return pixelData;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
