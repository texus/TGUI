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


#include <TGUI/Backends/SFML/BackendFontSFML.hpp>

#include <SFML/Config.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSFML::loadFromFile(const String& filename)
    {
        return m_font.loadFromFile(filename.toStdString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSFML::loadFromMemory(const void* data, std::size_t sizeInBytes)
    {
        return m_font.loadFromMemory(data, sizeInBytes);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FontGlyph BackendFontSFML::getGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        const sf::Glyph& glyphSFML = m_font.getGlyph(codePoint, characterSize, bold, outlineThickness);

        FontGlyph glyph;
        glyph.advance = glyphSFML.advance;
        glyph.bounds = {glyphSFML.bounds.left, glyphSFML.bounds.top, glyphSFML.bounds.width, glyphSFML.bounds.height};
        return glyph;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getKerning(char32_t first, char32_t second, unsigned int characterSize, bool bold)
    {
#if (SFML_VERSION_MAJOR > 2) || (SFML_VERSION_MINOR >= 6)  // bold parameter was added in SFML 2.6
    return m_font.getKerning(first, second, characterSize, bold);
#else
    (void)bold;
    return m_font.getKerning(first, second, characterSize);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getLineSpacing(unsigned int characterSize)
    {
        return m_font.getLineSpacing(characterSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Font& BackendFontSFML::getInternalFont()
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
