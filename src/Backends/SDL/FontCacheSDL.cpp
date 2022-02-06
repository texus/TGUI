/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Backends/SDL/FontCacheSDL.hpp>
#include <TGUI/Backends/SDL/BackendFontSDL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<const BackendFontSDL*, std::map<unsigned int, std::pair<unsigned int, TTF_Font*>>> FontCacheSDL::m_fonts;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FontCacheSDL::removeFont(const BackendFontSDL* fontId)
    {
        TGUI_ASSERT(m_fonts[fontId].size() == 0, "FontCacheSDL::removeFont for a font that still had registered users");
        m_fonts.erase(fontId);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FontCacheSDL::registerFontSize(const BackendFontSDL* fontId, unsigned int characterSize)
    {
        m_fonts[fontId][characterSize].first++;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FontCacheSDL::unregisterFontSize(const BackendFontSDL* fontId, unsigned int characterSize)
    {
        unsigned int& users = m_fonts[fontId][characterSize].first;
        TGUI_ASSERT(users > 0, "FontCacheSDL::unregisterFontSize can't be called more often than registerFontSize");
        users--;
        if (users == 0)
        {
            if (m_fonts[fontId][characterSize].second)
                TTF_CloseFont(m_fonts[fontId][characterSize].second);

            m_fonts[fontId].erase(characterSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TTF_Font* FontCacheSDL::getFont(const BackendFontSDL* fontId, unsigned int characterSize)
    {
        TGUI_ASSERT(m_fonts[fontId][characterSize].first > 0, "FontCacheSDL::getFont can't be called before registerFontSize is called");

        if (m_fonts[fontId][characterSize].second)
            return m_fonts[fontId][characterSize].second;
        else
        {
            TTF_Font* font = fontId->loadInternalFont(characterSize);
            m_fonts[fontId][characterSize].second = font;
            return font;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
