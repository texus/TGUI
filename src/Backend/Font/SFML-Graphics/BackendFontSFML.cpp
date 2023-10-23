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


#include <TGUI/Backend/Font/SFML-Graphics/BackendFontSFML.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

#include <SFML/Config.hpp>
#include <SFML/Graphics/Image.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontSFML::loadFromMemory(std::unique_ptr<std::uint8_t[]> data, std::size_t sizeInBytes)
    {
        m_textures.clear();
        m_textureVersions.clear();

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

        // If the internal SFML texture changes size as a result of the above getGlyph call, then the texture size wouldn't match
        // with ours anymore, causing corrupted text rendering. Luckily this hasGlyph function is never actually used in practice
        // and SFML 2.6 has a way to query the font, so we patch it here by resetting the texture and forcing a reload later.
        const_cast<BackendFontSFML*>(this)->m_textures[getGlobalTextSize()] = nullptr;

        return (glyph.advance != 0) || (glyph.bounds != sf::FloatRect{}) || (glyph.textureRect != sf::IntRect{});
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FontGlyph BackendFontSFML::getGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        const unsigned int scaledTextSize = static_cast<unsigned int>(characterSize * m_fontScale);
        const float scaledOutlineThickness = outlineThickness * m_fontScale;

        // If this is the first time requesting the glyph then mark the texture as outdated
        const auto key = constructGlyphKey(codePoint, scaledTextSize, bold, scaledOutlineThickness);
        if (m_loadedGlyphKeys.emplace(key).second)
            m_textures[scaledTextSize] = nullptr;

        const sf::Glyph& glyphSFML = m_font.getGlyph(codePoint, scaledTextSize, bold, scaledOutlineThickness);

        FontGlyph glyph;
        glyph.advance = glyphSFML.advance / m_fontScale;
#if (SFML_VERSION_MAJOR > 2) || (SFML_VERSION_MINOR >= 6)  // outline is not corrected in bounds in SFML < 2.6
        glyph.bounds = {glyphSFML.bounds.left / m_fontScale, glyphSFML.bounds.top / m_fontScale, glyphSFML.bounds.width / m_fontScale, glyphSFML.bounds.height / m_fontScale};
#else
        glyph.bounds = {glyphSFML.bounds.left / m_fontScale - scaledOutlineThickness, glyphSFML.bounds.top / m_fontScale - scaledOutlineThickness, glyphSFML.bounds.width / m_fontScale, glyphSFML.bounds.height / m_fontScale};
#endif

        // SFML uses an IntRect, but all values are unsigned
        assert(glyphSFML.textureRect.left >= 0);
        assert(glyphSFML.textureRect.top >= 0);
        assert(glyphSFML.textureRect.width >= 0);
        assert(glyphSFML.textureRect.height >= 0);
        glyph.textureRect = {
            static_cast<unsigned int>(glyphSFML.textureRect.left),
            static_cast<unsigned int>(glyphSFML.textureRect.top),
            static_cast<unsigned int>(glyphSFML.textureRect.width),
            static_cast<unsigned int>(glyphSFML.textureRect.height)};

        return glyph;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getKerning(char32_t first, char32_t second, unsigned int characterSize, bool bold)
    {
#if (SFML_VERSION_MAJOR > 2) || (SFML_VERSION_MINOR >= 6)  // bold parameter was added in SFML 2.6
        return m_font.getKerning(first, second, static_cast<unsigned int>(characterSize * m_fontScale), bold) / m_fontScale;
#else
        (void)bold;
        return m_font.getKerning(first, second, static_cast<unsigned int>(characterSize * m_fontScale)) / m_fontScale;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getLineSpacing(unsigned int characterSize)
    {
        return m_font.getLineSpacing(static_cast<unsigned int>(characterSize * m_fontScale)) / m_fontScale;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getFontHeight(unsigned int characterSize)
    {
        return getAscent(characterSize) + getDescent(characterSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getAscent(unsigned int characterSize)
    {
        const unsigned int scaledTextSize = static_cast<unsigned int>(characterSize * m_fontScale);

        // SFML doesn't provide a method to access the ascent of the font.
        // If the font contains a capital e-circumflex glyph then we use its size as our ascent value.
        // Otherwise we will simply assume that the ascent equals the character size, which is what SFML's sf::Text class does.
        // With the built-in DejaVuSans font, ascent should be 15 for a text size of 16.
#if (SFML_VERSION_MAJOR > 2) || (SFML_VERSION_MINOR >= 6)
        if (!m_font.hasGlyph(U'\u00CA'))
            return static_cast<float>(scaledTextSize) / m_fontScale;

        const FontGlyph& glyph = getGlyph(U'\u00CA', scaledTextSize, false, 0);
#else
        const FontGlyph& glyph = getGlyph(U'\u00CA', scaledTextSize, false, 0);
        if ((glyph.advance == 0) && (glyph.bounds == FloatRect{}) && (glyph.textureRect == UIntRect{}))
            return static_cast<float>(scaledTextSize) / m_fontScale;
#endif

        return glyph.bounds.height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getDescent(unsigned int characterSize)
    {
        // SFML doesn't provide a method to access the descent of the font.
        // We extract the descent by examining the 'g' glyph, assuming it exists.
        const unsigned int scaledTextSize = static_cast<unsigned int>(characterSize * m_fontScale);
        const FontGlyph& glyph = getGlyph(U'g', scaledTextSize, false);
        return glyph.bounds.height + glyph.bounds.top;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getUnderlinePosition(unsigned int characterSize)
    {
        return m_font.getUnderlinePosition(static_cast<unsigned int>(characterSize * m_fontScale)) / m_fontScale;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontSFML::getUnderlineThickness(unsigned int characterSize)
    {
        return m_font.getUnderlineThickness(static_cast<unsigned int>(characterSize * m_fontScale)) / m_fontScale;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendFontSFML::getTexture(unsigned int characterSize, unsigned int& textureVersion)
    {
        const unsigned int scaledTextSize = static_cast<unsigned int>(characterSize * m_fontScale);

        if (m_textures[scaledTextSize])
        {
            textureVersion = m_textureVersions[scaledTextSize];
            return m_textures[scaledTextSize];
        }

        // Unfortunately we currently need to extract the pixel data from the sf::Font texture, in order to load a
        // texture with TGUI, even if the TGUI texture also uses sf::Texture.
        const sf::Image& image = m_font.getTexture(scaledTextSize).copyToImage();
        auto texture = getBackend()->getRenderer()->createTexture();
        texture->loadTextureOnly({image.getSize().x, image.getSize().y}, image.getPixelsPtr(), m_isSmooth);
        m_textures[scaledTextSize] = texture;

        textureVersion = ++m_textureVersions[scaledTextSize];
        return texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u BackendFontSFML::getTextureSize(unsigned int characterSize)
    {
        const unsigned int scaledTextSize = static_cast<unsigned int>(characterSize * m_fontScale);
        const auto size = m_font.getTexture(scaledTextSize).getSize();
        return {size.x, size.y};
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

    void BackendFontSFML::setFontScale(float scale)
    {
        if (m_fontScale == scale)
            return;

        BackendFont::setFontScale(scale);

        // Destroy all textures to force texts to update their glyphs
        m_textures.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
