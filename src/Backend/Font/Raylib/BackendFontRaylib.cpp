/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Font/Raylib/BackendFontRaylib.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

#include <raylib.h>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <numeric>
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontRaylib::loadFromMemory(std::unique_ptr<std::uint8_t[]> data, std::size_t sizeInBytes)
    {
        m_cachedAscents.clear();
        m_glyphs.clear();
        m_nextRow = 3; // First 2 rows contain pixels for underlining

        constexpr unsigned int initialTextureSize = 128;
        m_pixels = MakeUniqueForOverwrite<std::uint8_t[]>(initialTextureSize * initialTextureSize * 4);

        std::uint8_t* pixelPtr = m_pixels.get();
        for (unsigned int y = 0; y < initialTextureSize; ++y)
        {
            for (unsigned int x = 0; x < initialTextureSize; ++x)
            {
                // Color is always white, alpha channel contains whether the pixel is empty of not
                *pixelPtr++ = 255;
                *pixelPtr++ = 255;
                *pixelPtr++ = 255;
                *pixelPtr++ = 0;
            }
        }

        // Reserve a 2x2 white square in the top left corner of the texture that we can use for drawing an underline
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int x = 0; x < 2; ++x)
                m_pixels[((initialTextureSize * y) + x) * 4 + 3] = 255;
        }

        m_textureSize = initialTextureSize;

        m_fileContents = std::move(data);
        m_fileSize = sizeInBytes;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontRaylib::hasGlyph(char32_t codePoint) const
    {
        const unsigned int scaledTextSize = static_cast<unsigned int>(getGlobalTextSize() * m_fontScale);
        const std::uint64_t glyphKey = constructGlyphKey(codePoint, scaledTextSize, false, 0);

        const auto it = m_glyphs.find(glyphKey);
        if (it != m_glyphs.end())
            return true;

        int codePointInt = static_cast<int>(codePoint);
        GlyphInfo* glyphsInfo = LoadFontData(m_fileContents.get(), static_cast<int>(m_fileSize), static_cast<int>(scaledTextSize), &codePointInt, 1, FONT_DEFAULT);
        if (!glyphsInfo)
            return false;

        const bool glyphHasBitmap = (glyphsInfo[0].image.data != nullptr);
        UnloadFontData(glyphsInfo, 1);
        return glyphHasBitmap;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int BackendFontRaylib::estimateFontSize(unsigned int scaledTextSize)
    {
        std::array<int, 3> codePoints = {{'a', 'g', 0x00CA}};
        GlyphInfo* glyphsInfo = LoadFontData(m_fileContents.get(), static_cast<int>(m_fileSize), static_cast<int>(scaledTextSize),
                                             codePoints.data(), static_cast<int>(codePoints.size()), FONT_DEFAULT);
        if (!glyphsInfo)
            return static_cast<int>(scaledTextSize);

        int ascent;
        if (glyphsInfo[2].image.height) // height of capital e-circumflex
            ascent = glyphsInfo[2].image.height;
        else if (glyphsInfo[0].image.height) // height of "a"
            ascent = glyphsInfo[0].image.height + glyphsInfo[0].offsetY;
        else // Fall back to just using font size
            ascent = static_cast<int>(scaledTextSize);

        int descent = 0;
        if (glyphsInfo[1].image.height)
            descent = glyphsInfo[1].image.height + glyphsInfo[1].offsetY - ascent;

        UnloadFontData(glyphsInfo, static_cast<int>(codePoints.size()));

        if ((ascent == 0) || (descent == 0))
            return static_cast<int>(scaledTextSize);

        return static_cast<int>(scaledTextSize * ((ascent + descent) / static_cast<float>(ascent)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FontGlyph BackendFontRaylib::getGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        const unsigned int scaledTextSize = static_cast<unsigned int>(characterSize * m_fontScale);
        const float scaledOutlineThickness = outlineThickness * m_fontScale;
        const std::uint64_t glyphKey = constructGlyphKey(codePoint, scaledTextSize, bold, scaledOutlineThickness);

        const auto it = m_glyphs.find(glyphKey);
        if (it != m_glyphs.end())
            return it->second;

        // Loading a new glyph involves reloading the font, so we preload the default character set when attempting to load the first glyph
        if (m_glyphs.find(constructGlyphKey(U'a', scaledTextSize, bold, scaledOutlineThickness)) == m_glyphs.end())
        {
            m_correctedTextSizes[scaledTextSize] = estimateFontSize(scaledTextSize);

            std::array<int, 97> codePoints;
            std::iota(codePoints.begin(), codePoints.begin() + 96, 32); // Fill the array with codepoints 32-126
            codePoints[96] = 0x00CA; // capital e-circumflex to estimate font ascent

            GlyphInfo* glyphsInfo = LoadFontData(m_fileContents.get(), static_cast<int>(m_fileSize),
                                                 static_cast<int>(m_correctedTextSizes[scaledTextSize]),
                                                 codePoints.data(), static_cast<int>(codePoints.size()), FONT_DEFAULT);
            if (glyphsInfo)
            {
                // Raylib has no way of getting the font ascent and only provides a character offset from the top instead of the baseline.
                // So we try to figure out what the ascent is by getting the height of the capital e-circumflex glyph. If this glyph does
                // not exist in the font then we will use the height of the "a" glyph plus the offset from the top position.
                if (glyphsInfo[96].image.height) // height of capital e-circumflex
                    m_cachedAscents[scaledTextSize] = glyphsInfo[96].image.height;
                else if (glyphsInfo[97-32].image.height) // height of "a"
                    m_cachedAscents[scaledTextSize] = glyphsInfo[97-32].image.height + glyphsInfo[97-32].offsetY;
                else // Fall back to just using font size
                    m_cachedAscents[scaledTextSize] = static_cast<int>(scaledTextSize);

                for (std::size_t i = 0; i < codePoints.size(); ++i)
                    loadGlyph(glyphsInfo[i], static_cast<char32_t>(codePoints[i]), scaledTextSize, bold, scaledOutlineThickness);

                UnloadFontData(glyphsInfo, static_cast<int>(codePoints.size()));
            }
            else // We couldn't load the glyphs
            {
                m_cachedAscents[scaledTextSize] = static_cast<int>(scaledTextSize);
                m_glyphs.insert({constructGlyphKey(U'a', scaledTextSize, bold, scaledOutlineThickness), FontGlyph()});
            }

            // It's possible that we just loaded the glyph that we were searching for
            const auto retryIt = m_glyphs.find(glyphKey);
            if (retryIt != m_glyphs.end())
                return retryIt->second;
        }

        int codePointInt = static_cast<int>(codePoint);
        GlyphInfo* glyphsInfo = LoadFontData(m_fileContents.get(), static_cast<int>(m_fileSize), static_cast<int>(m_correctedTextSizes[scaledTextSize]), &codePointInt, 1, FONT_DEFAULT);
        if (!glyphsInfo)
            return m_glyphs.insert({glyphKey, FontGlyph()}).first->second;

        FontGlyph glyph = loadGlyph(glyphsInfo[0], codePoint, scaledTextSize, bold, scaledOutlineThickness);
        UnloadFontData(glyphsInfo, 1);
        return glyph;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontRaylib::getKerning(char32_t, char32_t, unsigned int, bool)
    {
        // Raylib doesn't support kerning
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontRaylib::getLineSpacing(unsigned int characterSize)
    {
        // Raylib doesn't read line spacing from the font
        return getFontHeight(characterSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontRaylib::getFontHeight(unsigned int characterSize)
    {
        return getAscent(characterSize) + getDescent(characterSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontRaylib::getAscent(unsigned int characterSize)
    {
        // Raylib doesn't provide a method to access the ascent of the font.
        // The getGlyph function will update m_cachedAscents when we didn't have a cached value yet.
        const unsigned int scaledTextSize = static_cast<unsigned int>(characterSize * m_fontScale);
        if (!m_cachedAscents[scaledTextSize])
            (void)getGlyph(U' ', characterSize, false, 0);

        return static_cast<float>(m_cachedAscents[scaledTextSize]) / m_fontScale;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontRaylib::getDescent(unsigned int characterSize)
    {
        // Raylib doesn't provide a method to access the descent of the font.
        // We extract the descent by examining the 'g' glyph, assuming it exists.
        const unsigned int scaledTextSize = static_cast<unsigned int>(characterSize * m_fontScale);
        const FontGlyph& glyph = getGlyph(U'g', scaledTextSize, false);
        return glyph.bounds.height + glyph.bounds.top;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontRaylib::getUnderlinePosition(unsigned int characterSize)
    {
        // Raylib doesn't support underlined text, so we choose an offset ourselves
        return characterSize / 10.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontRaylib::getUnderlineThickness(unsigned int characterSize)
    {
        // Raylib doesn't support text styles, so we choose an thickness ourselves
        return characterSize / 14.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendFontRaylib::getTexture(unsigned int, unsigned int& textureVersion)
    {
        if (m_texture)
        {
            textureVersion = m_textureVersion;
            return m_texture;
        }

        m_texture = getBackend()->getRenderer()->createTexture();
        m_texture->loadTextureOnly({m_textureSize, m_textureSize}, m_pixels.get(), m_isSmooth);

        textureVersion = ++m_textureVersion;
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u BackendFontRaylib::getTextureSize(unsigned int)
    {
        return {m_textureSize, m_textureSize};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendFontRaylib::setSmooth(bool smooth)
    {
        BackendFont::setSmooth(smooth);
        if (m_texture)
            m_texture->setSmooth(m_isSmooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendFontRaylib::setFontScale(float scale)
    {
        if (m_fontScale == scale)
            return;

        BackendFont::setFontScale(scale);

        // Destroy the texture to force texts to update their glyphs
        m_texture = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FontGlyph BackendFontRaylib::loadGlyph(const GlyphInfo& glyphInfo, char32_t codePoint, unsigned int scaledTextSize, bool bold, float scaledOutlineThickness)
    {
        const std::uint64_t glyphKey = constructGlyphKey(codePoint, scaledTextSize, bold, scaledOutlineThickness);
        const int fontAscent = m_cachedAscents[scaledTextSize];

        FontGlyph glyph;
        if (glyphInfo.image.data)
        {
            const int outlineInt = static_cast<int>(std::round(std::abs(scaledOutlineThickness)));
            const int charSpacing = 1;
            glyph.advance = static_cast<float>(glyphInfo.advanceX + charSpacing + (bold ? 1 : 0)) / m_fontScale;
            glyph.bounds.left = static_cast<float>(glyphInfo.offsetX - outlineInt) / m_fontScale;
            glyph.bounds.top = static_cast<float>(glyphInfo.offsetY - fontAscent - outlineInt) / m_fontScale;
            glyph.bounds.width = static_cast<float>(glyphInfo.image.width + (2 * outlineInt) + (bold ? 1 : 0)) / m_fontScale;
            glyph.bounds.height = static_cast<float>(glyphInfo.image.height + (2 * outlineInt)) / m_fontScale;

            const auto imagePixels = static_cast<const std::uint8_t*>(glyphInfo.image.data);
            if ((glyphInfo.image.mipmaps == 1)
             && (glyphInfo.image.format == PIXELFORMAT_UNCOMPRESSED_GRAYSCALE)
             && (glyphInfo.image.width > 0) && (glyphInfo.image.height > 0))
            {
                const unsigned int imageWidth = static_cast<unsigned int>(glyphInfo.image.width);
                const unsigned int imageHeight = static_cast<unsigned int>(glyphInfo.image.height);
                const unsigned int outlineUInt = static_cast<unsigned int>(outlineInt);

                // Find a good position for the new glyph into the texture.
                // We leave a small padding around characters, so that filtering doesn't pollute them with pixels from neighbors.
                const unsigned int padding = 2;
                if (bold)
                    glyph.textureRect = findAvailableGlyphRect(imageWidth + 1 + (2 * outlineUInt) + (2 * padding), imageHeight + (2 * outlineUInt) + (2 * padding));
                else
                    glyph.textureRect = findAvailableGlyphRect(imageWidth + (2 * outlineUInt) + (2 * padding), imageHeight + (2 * outlineUInt) + (2 * padding));
                glyph.textureRect.left += padding;
                glyph.textureRect.top += padding;
                glyph.textureRect.width -= 2 * padding;
                glyph.textureRect.height -= 2 * padding;

                if (outlineUInt != 0)
                {
                    for (int y = -outlineInt; y < static_cast<int>(imageHeight) + outlineInt; ++y)
                    {
                        for (int x = -outlineInt; x < static_cast<int>(imageWidth) + outlineInt; ++x)
                        {
                            // Use the maximum of all nearby pixels to expand the glyph to the size of the outline
                            const int minY = std::max(y - outlineInt, 0);
                            const int maxY = std::min(y + outlineInt, static_cast<int>(imageHeight - 1));
                            const int minX = std::max(x - outlineInt, 0);
                            const int maxX = std::min(x + outlineInt, static_cast<int>(imageWidth - 1));

                            std::uint8_t pixelValue = 0;
                            for (int pixelY = minY; pixelY <= maxY; ++pixelY)
                            {
                                for (int pixelX = minX; pixelX <= maxX; ++pixelX)
                                    pixelValue = std::max(pixelValue, imagePixels[(pixelY * static_cast<int>(imageWidth)) + pixelX]);
                            }

                            const std::size_t index = glyph.textureRect.left + static_cast<std::size_t>(outlineInt + x)
                                + ((glyph.textureRect.top + static_cast<std::size_t>(outlineInt + y)) * m_textureSize);
                            m_pixels[index * 4 + 3] = pixelValue;
                        }
                    }
                }
                else
                {
                    for (unsigned int y = 0; y < imageHeight; ++y)
                    {
                        for (unsigned int x = 0; x < imageWidth; ++x)
                        {
                            const std::size_t index = (glyph.textureRect.left + x) + ((glyph.textureRect.top + y) * m_textureSize);
                            m_pixels[index * 4 + 3] = imagePixels[(y * imageWidth) + x];
                        }
                    }
                }

                // Raylib doesn't support bolding text, so we fake it by duplicating pixels on the x-axis. It's not perfect, but it's better than doing nothing.
                if (bold)
                {
                    for (unsigned int y = 0; y < imageHeight; ++y)
                    {
                        for (unsigned int x = imageWidth + (2 * outlineUInt); x > 0; --x) // Loop right-to-left to not read overwritten pixels back
                        {
                            const std::size_t index = (glyph.textureRect.left + x) + (glyph.textureRect.top + y) * m_textureSize;
                            const std::size_t neigborIndex = (glyph.textureRect.left + (x - 1)) + (glyph.textureRect.top + y) * m_textureSize;
                            m_pixels[index * 4 + 3] = std::max(m_pixels[index * 4 + 3], m_pixels[neigborIndex * 4 + 3]);
                        }
                    }
                }

                // We will have to recreate the texture now that the pixels changed
                m_texture = nullptr;
            }
        }

        return m_glyphs.insert({glyphKey, glyph}).first->second;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    UIntRect BackendFontRaylib::findAvailableGlyphRect(unsigned int width, unsigned int height)
    {
        // Find the line that where the glyph fits well.
        // This is based on the sf::Font class in the SFML library. It might not be the most optimal method, but it is good enough for now.
        Row* bestRow = nullptr;
        float bestRatio = 0;
        for (auto& row : m_rows)
        {
            float ratio = static_cast<float>(height) / row.height;

            // Ignore rows that are either too small or too high
            if ((ratio < 0.7f) || (ratio > 1.f))
                continue;

            // Check if there's enough horizontal space left in the row
            if (width > m_textureSize - row.width)
                continue;

            // Make sure that this new row is the best found so far
            if (ratio < bestRatio)
                continue;

            // The current row passed all the tests: we can select it
            bestRow = &row;
            bestRatio = ratio;
        }

        // If we didn't find a matching row, create a new one (10% taller than the glyph)
        if (!bestRow)
        {
            // Check if the glyph can fit in the texture and resize the texture otherwise
            const unsigned int rowHeight = height + (height / 10);
            while ((m_nextRow + rowHeight >= m_textureSize) || (width >= m_textureSize))
            {
                /// TODO: Check if maximum texture size is reached and use multiple textures if there isn't enough space left

                // Copy existing pixels to the top left quadrant
                auto pixels = MakeUniqueForOverwrite<std::uint8_t[]>((m_textureSize * 2) * (m_textureSize * 2) * 4);
                for (unsigned int y = 0; y < m_textureSize; ++y)
                    std::memcpy(&pixels[y * (2 * m_textureSize) * 4], &m_pixels[y * m_textureSize * 4], m_textureSize * 4);

                // Top right quadrant and bottom halves are filled with empty values
                // Color is always white, alpha channel contains whether the pixel is empty of not
                for (unsigned int y = 0; y < m_textureSize; ++y)
                {
                    std::uint8_t* pixelPtr = &pixels[((y * (2 * m_textureSize)) + m_textureSize) * 4];
                    for (unsigned int i = 0; i < m_textureSize; ++i)
                    {
                        *pixelPtr++ = 255;
                        *pixelPtr++ = 255;
                        *pixelPtr++ = 255;
                        *pixelPtr++ = 0;
                    }
                }
                std::uint8_t* pixelPtr = &pixels[m_textureSize * (2 * m_textureSize) * 4];
                for (unsigned int i = 0; i < (2 * m_textureSize) * m_textureSize; ++i)
                {
                    *pixelPtr++ = 255;
                    *pixelPtr++ = 255;
                    *pixelPtr++ = 255;
                    *pixelPtr++ = 0;
                }

                m_pixels = std::move(pixels);
                m_textureSize *= 2;
            }

            // We can now create the new row
            m_rows.emplace_back(m_nextRow, rowHeight);
            m_nextRow += rowHeight;
            bestRow = &m_rows.back();
        }

        // Find the glyph's rectangle on the selected row
        UIntRect rect{bestRow->width, bestRow->top, width, height};

        // Update the row informations
        bestRow->width += width;
        return rect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
