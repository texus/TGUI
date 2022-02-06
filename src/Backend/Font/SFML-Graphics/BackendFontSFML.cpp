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


#include <TGUI/Backend/Font/SFML-Graphics/BackendFontSFML.hpp>
#include <TGUI/Backend/Window/Backend.hpp>

#include <SFML/Config.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSFML::loadFromMemory(std::unique_ptr<std::uint8_t[]> data, std::size_t sizeInBytes)
    {
        m_fileContents = std::move(data);
        return m_font.loadFromMemory(m_fileContents.get(), sizeInBytes);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSFML::hasGlyph(char32_t codePoint) const
    {
#if (SFML_VERSION_MAJOR > 2) || (SFML_VERSION_MINOR >= 6)  // hasGlyph was added in SFML 2.6
        return m_font.hasGlyph(codePoint);
#else
        const sf::Glyph& glyph = m_font.getGlyph(codePoint, getGlobalTextSize(), false, 0);
        return (glyph.advance != 0) || (glyph.bounds != sf::FloatRect{}) || (glyph.textureRect != sf::IntRect{});
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FontGlyph BackendFontSFML::getGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        // If this is the first time requesting the glyph then mark the texture as outdated
        const auto key = constructGlyphKey(codePoint, characterSize, bold, outlineThickness);
        if (m_loadedGlyphKeys.emplace(key).second)
            m_textures[characterSize] = nullptr;

        const sf::Glyph& glyphSFML = m_font.getGlyph(codePoint, characterSize, bold, outlineThickness);

        FontGlyph glyph;
        glyph.advance = glyphSFML.advance;
#if (SFML_VERSION_MAJOR > 2) || (SFML_VERSION_MINOR >= 6)  // outline is not corrected in bounds in SFML < 2.6
        glyph.bounds = {glyphSFML.bounds.left, glyphSFML.bounds.top, glyphSFML.bounds.width, glyphSFML.bounds.height};
#else
        glyph.bounds = {glyphSFML.bounds.left - outlineThickness, glyphSFML.bounds.top - outlineThickness, glyphSFML.bounds.width, glyphSFML.bounds.height};
#endif
        glyph.textureRect = {glyphSFML.textureRect.left, glyphSFML.textureRect.top, glyphSFML.textureRect.width, glyphSFML.textureRect.height};
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

    float BackendFontSFML::getUnderlinePosition(unsigned int characterSize)
    {
        return m_font.getUnderlinePosition(characterSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getUnderlineThickness(unsigned int characterSize)
    {
        return m_font.getUnderlineThickness(characterSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendFontSFML::getTexture(unsigned int characterSize)
    {
        if (m_textures[characterSize])
            return m_textures[characterSize];

        // Unfortunately we currently need to extract the pixel data from the sf::Font texture, in order to load a
        // texture with TGUI, even if the TGUI texture also uses sf::Texture.
        const sf::Image& image = m_font.getTexture(characterSize).copyToImage();

        auto texture = getBackend()->getRenderer()->createTexture();
        texture->loadTextureOnly({image.getSize().x, image.getSize().y}, image.getPixelsPtr(), m_isSmooth);
        m_textures[characterSize] = texture;
        return texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendFontSFML::setSmooth(bool smooth)
    {
        BackendFont::setSmooth(smooth);
        for (auto& pair : m_textures)
        {
            auto& texture = pair.second;
            if (texture)
                texture->setSmooth(m_isSmooth);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Font& BackendFontSFML::getInternalFont()
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
