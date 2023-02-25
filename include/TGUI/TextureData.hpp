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


#ifndef TGUI_TEXTURE_DATA_HPP
#define TGUI_TEXTURE_DATA_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Rect.hpp>
#include <TGUI/Optional.hpp>
#include <TGUI/SvgImage.hpp>

#include <TGUI/Backend/Renderer/BackendTexture.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <memory>
    #include <string>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Used by the Texture class
    struct TGUI_API TextureData
    {
        // Either svgImage or backendTexture MUST have a value
        Optional<SvgImage> svgImage;
        std::shared_ptr<BackendTexture> backendTexture;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Wrapper around TextureData to be used in TextureManager
    struct TGUI_API TextureDataHolder
    {
        std::shared_ptr<TextureData> data;
        String  filename;
        bool smooth = true;
        unsigned int users = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TEXTURE_DATA_HPP
