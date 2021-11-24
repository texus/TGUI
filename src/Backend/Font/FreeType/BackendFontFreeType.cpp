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


#include <TGUI/Backend/Font/FreeType/BackendFontFreeType.hpp>
#include <TGUI/Backend/Window/Backend.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendFontFreetype::~BackendFontFreetype()
    {
        cleanup();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontFreetype::loadFromMemory(std::unique_ptr<std::uint8_t[]> data, std::size_t sizeInBytes)
    {
        cleanup();
        m_cachedLineSpacing.clear();
        m_glyphs.clear();
        m_rows.clear();
        m_pixels = nullptr;
        m_texture = nullptr;
        m_textureSize = 0;
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

        // Initialize the freetype library each time, since we currently don't reuse the library object between fonts.
        FT_Library library;
        if (FT_Init_FreeType(&library) != 0)
            throw Exception{"Failed to initialize Freetype"};

        // Load the font face from the font file that we have in memory
        FT_Face face;
        if (FT_New_Memory_Face(library, static_cast<const FT_Byte*>(data.get()), static_cast<FT_Long>(sizeInBytes), 0, &face) != 0)
        {
            FT_Done_FreeType(library);
            throw Exception{"Failed to load font face"};
        }

        // Select the unicode character map. Can we ignore a failure from this function?
        if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
        {
            FT_Done_Face(face);
            FT_Done_FreeType(library);
            throw Exception{"Failed to select font character map"};
        }

        m_fileContents = std::move(data);
        m_library = library;
        m_face = face;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontFreetype::hasGlyph(char32_t codePoint) const
    {
        return FT_Get_Char_Index(m_face, static_cast<FT_ULong>(codePoint)) != 0; // m_face is allowed to be nullptr
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FontGlyph BackendFontFreetype::getGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        FontGlyph glyph;
        if (!m_face)
            return glyph;

        const Glyph internalGlyph = getInternalGlyph(codePoint, characterSize, bold, outlineThickness);
        glyph.advance = internalGlyph.advance;
        glyph.bounds = internalGlyph.bounds;
        glyph.textureRect = internalGlyph.textureRect;
        return glyph;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    IntRect BackendFontFreetype::findAvailableGlyphRect(unsigned int width, unsigned int height)
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
            m_rows.push_back({m_nextRow, rowHeight});
            m_nextRow += rowHeight;
            bestRow = &m_rows.back();
        }

        // Find the glyph's rectangle on the selected row
        IntRect rect(bestRow->width, bestRow->top, width, height);

        // Update the row informations
        bestRow->width += width;
        return rect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendFontFreetype::Glyph BackendFontFreetype::loadGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        Glyph glyph;
        if (!m_face)
            return glyph;

        if (!setCurrentSize(characterSize))
            return glyph;
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
        // Load the glyph corresponding to the code point
        FT_Int32 flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
        if (outlineThickness != 0)
            flags |= FT_LOAD_NO_BITMAP;
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
        if (FT_Load_Char(m_face, codePoint, flags) != 0)
            return glyph;

        // Retrieve the glyph
        FT_Glyph glyphDesc;
        if (FT_Get_Glyph(m_face->glyph, &glyphDesc) != 0)
            return glyph;

        // Add an outline if requested and the font supports it
        if ((outlineThickness != 0) && (glyphDesc->format == FT_GLYPH_FORMAT_OUTLINE))
        {
            // Create the stroker if this is the first time an outline was requested with this font
            if (!m_stroker)
                FT_Stroker_New(m_library, &m_stroker);

            if (m_stroker)
            {
                FT_Stroker_Set(m_stroker, static_cast<FT_Fixed>(outlineThickness * static_cast<float>(1 << 6)), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
                FT_Glyph_Stroke(&glyphDesc, m_stroker, true);
            }
        }

        // Apply the bold style if requested and outlines are supported
        const FT_Pos boldWeight = 1 << 6;
        const bool outlineSupport = (glyphDesc->format == FT_GLYPH_FORMAT_OUTLINE);
        if (bold && outlineSupport)
        {
            FT_OutlineGlyph outlineGlyph = reinterpret_cast<FT_OutlineGlyph>(glyphDesc);
            FT_Outline_Embolden(&outlineGlyph->outline, boldWeight);
        }

        // Rasterize the glyph to a bitmap
        // Warning: use bitmapGlyph->root instead of glyphDesc to access the glyph after this conversion
        if (FT_Glyph_To_Bitmap(&glyphDesc, FT_RENDER_MODE_NORMAL, 0, 1) != 0)
        {
            FT_Done_Glyph(glyphDesc);
            return glyph;
        }
        FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyphDesc);
        FT_Bitmap& bitmap = bitmapGlyph->bitmap;

        // If bold was requested but the font didn't support outlines then apply bold here using a different (lower quality) method
        // We have to cache the outline support bool because FT_Glyph_To_Bitmap changes format to FT_GLYPH_FORMAT_BITMAP.
        if (bold && !outlineSupport)
            FT_Bitmap_Embolden(static_cast<FT_Library>(m_library), &bitmap, boldWeight, boldWeight);

        // Bit shift is possible without loss because we use FT_LOAD_FORCE_AUTOHINT flag
        glyph.advance = static_cast<float>(bitmapGlyph->root.advance.x >> 16);
        if (bold)
            glyph.advance += static_cast<float>(boldWeight) / static_cast<float>(1 << 6);

        glyph.lsbDelta = static_cast<float>(m_face->glyph->lsb_delta);
        glyph.rsbDelta = static_cast<float>(m_face->glyph->rsb_delta);

        // Compute the glyph's bounding box
        glyph.bounds.left = static_cast<float>(bitmapGlyph->left);
        glyph.bounds.top = static_cast<float>(-bitmapGlyph->top);
        glyph.bounds.width = static_cast<float>(bitmap.width);
        glyph.bounds.height = static_cast<float>(bitmap.rows);

        if ((bitmap.width == 0) || (bitmap.rows == 0))
        {
            // This branch will e.g. be executed when loading a space character. The glyph will have an advance but no bitmap.
            FT_Done_Glyph(glyphDesc);
            return glyph;
        }

        // Find a good position for the new glyph into the texture.
        // We leave a small padding around characters, so that filtering doesn't pollute them with pixels from neighbors.
        const unsigned int padding = 2;
        glyph.textureRect = findAvailableGlyphRect(bitmap.width + (2 * padding), bitmap.rows + (2 * padding));
        glyph.textureRect.left += padding;
        glyph.textureRect.top += padding;
        glyph.textureRect.width -= 2 * padding;
        glyph.textureRect.height -= 2 * padding;

        // Extract the glyph's pixels from the bitmap
        const std::uint8_t* glyphBitmapPixels = bitmap.buffer;
        if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
        {
            // Pixels are 1 bit monochrome values
            for (unsigned int y = 0; y < bitmap.rows; ++y)
            {
                for (unsigned int x = 0; x < bitmap.width; ++x)
                {
                    // The color channels remain white, just fill the alpha channel
                    const std::size_t index = (glyph.textureRect.left + x) + (glyph.textureRect.top + y) * m_textureSize;
                    m_pixels[index * 4 + 3] = ((glyphBitmapPixels[(y * bitmap.pitch) + (x / 8)]) & (1 << (7 - (x % 8)))) ? 255 : 0;
                }
            }
        }
        else
        {
            // Pixels are 8 bits gray levels
            for (unsigned int y = 0; y < bitmap.rows; ++y)
            {
                for (unsigned int x = 0; x < bitmap.width; ++x)
                {
                    // The color channels remain white, just fill the alpha channel
                    const std::size_t index = (glyph.textureRect.left + x) + (glyph.textureRect.top + y) * m_textureSize;
                    m_pixels[index * 4 + 3] = glyphBitmapPixels[(y * bitmap.pitch) + x];
                }
            }
        }

        // We will have to recreate the texture now that the pixels changed
        m_texture = nullptr;

        FT_Done_Glyph(glyphDesc);
        return glyph;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendFontFreetype::Glyph BackendFontFreetype::getInternalGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness)
    {
        const std::uint64_t glyphKey = constructGlyphKey(codePoint, characterSize, bold, outlineThickness);

        const auto it = m_glyphs.find(glyphKey);
        if (it != m_glyphs.end())
            return it->second;

        const Glyph glyph = loadGlyph(codePoint, characterSize, bold, outlineThickness);
        return m_glyphs.insert({glyphKey, glyph}).first->second;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontFreetype::getKerning(char32_t first, char32_t second, unsigned int characterSize, bool bold)
    {
        // There is no kerning if one of the two characters is the null character
        if ((first == 0) || (second == 0))
            return 0;

        if (!m_face || !setCurrentSize(characterSize))
            return 0;

        // Retrieve position compensation deltas generated by FT_LOAD_FORCE_AUTOHINT flag
        const float firstRsbDelta = getInternalGlyph(first, characterSize, bold, 0).rsbDelta;
        const float secondLsbDelta = getInternalGlyph(second, characterSize, bold, 0).lsbDelta;

        // Get the kerning vector if present
        FT_Vector kerning;
        kerning.x = kerning.y = 0;
        if (FT_HAS_KERNING(m_face))
        {
            const FT_UInt index1 = FT_Get_Char_Index(m_face, static_cast<FT_ULong>(first));
            const FT_UInt index2 = FT_Get_Char_Index(m_face, static_cast<FT_ULong>(second));
            FT_Get_Kerning(m_face, index1, index2, FT_KERNING_UNFITTED, &kerning);
        }

        // X advance is already in pixels for bitmap fonts
        if (!FT_IS_SCALABLE(m_face))
            return static_cast<float>(kerning.x);

        // Combine kerning with compensation deltas and return the X advance
        // Flooring is required as we use FT_KERNING_UNFITTED flag which is not quantized in 64 based grid
        return std::floor((secondLsbDelta - firstRsbDelta + kerning.x + 32) / 64.f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontFreetype::getLineSpacing(unsigned int characterSize)
    {
        const auto it = m_cachedLineSpacing.find(characterSize);
        if (it != m_cachedLineSpacing.end())
            return it->second;

        if (!m_face || !setCurrentSize(characterSize))
            return 0;

        return m_face->size->metrics.height / 64.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontFreetype::getUnderlinePosition(unsigned int characterSize)
    {
        if (!m_face || !setCurrentSize(characterSize))
            return 0;

        // Return a fixed position if font is a bitmap font
        if (!FT_IS_SCALABLE(m_face))
            return characterSize / 10.f;

        return -static_cast<float>(FT_MulFix(m_face->underline_position, m_face->size->metrics.y_scale)) / static_cast<float>(1 << 6);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendFontFreetype::getUnderlineThickness(unsigned int characterSize)
    {
        if (!m_face || !setCurrentSize(characterSize))
            return 0;

        // Return a fixed thickness if font is a bitmap font
        if (!FT_IS_SCALABLE(m_face))
            return characterSize / 14.f;

        return static_cast<float>(FT_MulFix(m_face->underline_thickness, m_face->size->metrics.y_scale)) / static_cast<float>(1 << 6);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendFontFreetype::getTexture(unsigned int)
    {
        if (m_texture)
            return m_texture;

        m_texture = getBackend()->getRenderer()->createTexture();
        m_texture->loadTextureOnly({m_textureSize, m_textureSize}, m_pixels.get(), m_isSmooth);
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendFontFreetype::setSmooth(bool smooth)
    {
        BackendFont::setSmooth(smooth);
        if (m_texture)
            m_texture->setSmooth(m_isSmooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendFontFreetype::setCurrentSize(unsigned int characterSize)
    {
        // Don't call FT_Set_Pixel_Sizes if we don't have to
        if (characterSize == m_face->size->metrics.x_ppem)
            return true;

        return (FT_Set_Pixel_Sizes(m_face, 0, characterSize) == FT_Err_Ok);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendFontFreetype::cleanup()
    {
        if (m_stroker)
            FT_Stroker_Done(m_stroker);

        if (m_face)
            FT_Done_Face(m_face);

        if (m_library)
            FT_Done_FreeType(m_library);

        m_library = nullptr;
        m_face    = nullptr;
        m_stroker = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
