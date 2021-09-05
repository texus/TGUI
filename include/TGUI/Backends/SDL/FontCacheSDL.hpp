/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_BACKEND_FONT_CACHE_SDL_HPP
#define TGUI_BACKEND_FONT_CACHE_SDL_HPP

#include <TGUI/Config.hpp>
#include <unordered_map>

#include <SDL_ttf.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    class BackendFontSDLttf;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Caches TTF_Font so that they can be reused
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API FontCacheSDL
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a font and all loaded character sizes
        ///
        /// @param fontId  Id of the font to remove
        ///
        /// @warning This function will assert if the font was still registered somwhere.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("The FontCacheSDL class is no longer used!") static void removeFont(const BackendFontSDLttf* fontId);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the cache that someone is using the font with a certain text size
        ///
        /// @param fontId         Id of the font that is being used
        /// @param characterSize  Text size that will be used
        ///
        /// @warning This function must be called before the font is accessed.
        /// @warning Each call to this function requires a call to unregisterFontSize.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("The FontCacheSDL class is no longer used!") static void registerFontSize(const BackendFontSDLttf* fontId, unsigned int characterSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the cache that the font with a certain text size now has one less user
        ///
        /// @param fontId         Id of the font that was being used
        /// @param characterSize  Text size that was being used
        ///
        /// @warning This function has to be called for each call to registerFontSize.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("The FontCacheSDL class is no longer used!") static void unregisterFontSize(const BackendFontSDLttf* fontId, unsigned int characterSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @param Retrieves the TTF_Font for a specific font at a specific text size
        ///
        /// @param fontId         Id of the requested font
        /// @param characterSize  Requested text size
        ///
        /// @warning registerFontSize must have been called before calling getFont
        ///
        /// If the font did not exist yet in the cache then it will be created here.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("The FontCacheSDL class is no longer used!") static TTF_Font* getFont(const BackendFontSDLttf* fontId, unsigned int characterSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        // fontId -> character size -> (nrUsers, font)
        static std::unordered_map<const BackendFontSDLttf*, std::unordered_map<unsigned int, std::pair<unsigned int, TTF_Font*>>> m_fonts;
    };
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_FONT_CACHE_SDL_HPP
