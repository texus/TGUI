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


#ifndef TGUI_IMAGE_LOADER_HPP
#define TGUI_IMAGE_LOADER_HPP


#include <TGUI/Vector2.hpp>
#include <TGUI/String.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cstdint>
    #include <memory>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Class that is internally used to load an image from a file or from memory
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API ImageLoader
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads an image from a file
        ///
        /// @param filename   The file to load
        /// @param imageSize  Output parameter that will contain the width and height of the image if loaded successfully
        ///
        /// @return RGBA array of pixels of loaded image (4 * imageSize.x * imageSize.y bytes), or nullptr if loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static std::unique_ptr<std::uint8_t[]> loadFromFile(const String& filename, Vector2u& imageSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads an image from memory (data in memory should contain the entire file, not just the pixels)
        ///
        /// @param data       Pointer to the file in memory
        /// @param dataSize   Amount of bytes of the file in memory
        /// @param imageSize  Output parameter that will contain the width and height of the image if loaded successfully
        ///
        /// @return RGBA array of pixels of loaded image (4 * imageSize.x * imageSize.y bytes), or nullptr if loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static std::unique_ptr<std::uint8_t[]> loadFromMemory(const std::uint8_t* data, std::size_t dataSize, Vector2u& imageSize);
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_IMAGE_LOADER_HPP
