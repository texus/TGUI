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


#include <TGUI/Backends/SDL/BackendFontSDL.hpp>
#include <TGUI/Backends/SDL/FontCacheSDL.hpp>
#include <TGUI/Global.hpp>

#include <cstring> // memcpy

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendFontSDL::~BackendFontSDL()
    {
        if (m_lastCharacterSize != 0)
            FontCacheSDL::unregisterFontSize(this, m_lastCharacterSize);

        FontCacheSDL::removeFont(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSDL::loadFromFile(const String& filename)
    {
        std::size_t fileSize;
        auto fileContents = readFileToMemory(filename, fileSize);
        if (!fileContents)
            return false;

        m_fileContents = std::move(fileContents);
        m_fileSize = fileSize;

        return (getInternalFont(getGlobalTextSize()) != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSDL::loadFromMemory(const void* data, std::size_t sizeInBytes)
    {
        m_fileSize = sizeInBytes;
        m_fileContents = std::make_unique<std::uint8_t[]>(sizeInBytes);
        std::memcpy(m_fileContents.get(), data, sizeInBytes);

        return (getInternalFont(getGlobalTextSize()) != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FontGlyph BackendFontSDL::getGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        FontGlyph glyph;

        TTF_Font* font = getInternalFont(characterSize);
        if (!font)
            return glyph;

        if (bold)
            TTF_SetFontStyle(font, TTF_STYLE_BOLD);

        if (outlineThickness != 0)
            TTF_SetFontOutline(font, static_cast<int>(outlineThickness));

        int minX;
        int maxX;
        int minY;
        int maxY;
        int advance;
        if (TTF_GlyphMetrics(font, static_cast<std::uint16_t>(codePoint), &minX, &maxX, &minY, &maxY, &advance) != 0)
            return glyph;

        if (bold)
            TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

        if (outlineThickness != 0)
            TTF_SetFontOutline(font, 0);

        glyph.advance = static_cast<float>(advance);
        glyph.bounds.left = static_cast<float>(minX);
        glyph.bounds.top = static_cast<float>(-maxY);
        glyph.bounds.width = static_cast<float>(maxX - minX);
        glyph.bounds.height = static_cast<float>(maxY - minY);
        return glyph;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSDL::getKerning(char32_t first, char32_t second, unsigned int characterSize, bool bold)
    {
        TTF_Font* font = getInternalFont(characterSize);
        if (!font)
            return 0;

        if (bold)
            TTF_SetFontStyle(font, TTF_STYLE_BOLD);

        const int kerning = TTF_GetFontKerningSizeGlyphs(font, static_cast<std::uint16_t>(first), static_cast<std::uint16_t>(second));

        if (bold)
            TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

        return static_cast<float>(kerning);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSDL::getLineSpacing(unsigned int characterSize)
    {
        TTF_Font* font = getInternalFont(characterSize);
        if (!font)
            return 0;

        return static_cast<float>(TTF_FontLineSkip(font));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TTF_Font* BackendFontSDL::getInternalFont(unsigned int characterSize)
    {
        if (characterSize == 0)
            return nullptr;

        if (m_cachedFont && (characterSize == m_lastCharacterSize))
            return m_cachedFont;

        if (characterSize != m_lastCharacterSize)
        {
            if (m_lastCharacterSize != 0)
                FontCacheSDL::unregisterFontSize(this, m_lastCharacterSize);

            FontCacheSDL::registerFontSize(this, characterSize);
            m_lastCharacterSize = characterSize;
        }

        m_cachedFont = FontCacheSDL::getFont(this, characterSize);
        return m_cachedFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TTF_Font* BackendFontSDL::loadInternalFont(unsigned int characterSize) const
    {
        if (!m_fileContents || (m_fileSize == 0))
            return nullptr;

        SDL_RWops* handle = SDL_RWFromConstMem(m_fileContents.get(), static_cast<int>(m_fileSize));
        if (!handle)
            return nullptr;

        return TTF_OpenFontRW(handle, 1, characterSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
