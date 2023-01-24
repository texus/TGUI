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


#include <TGUI/Backend/Renderer/BackendText.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f BackendText::getSize()
    {
        if (m_verticesNeedUpdate)
            updateVertices();

        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::setString(const String& string)
    {
        if (m_string == string)
            return;

        m_string = string;
        m_verticesNeedUpdate = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& BackendText::getString() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::setCharacterSize(unsigned int characterSize)
    {
        if (m_characterSize == characterSize)
            return;

        m_characterSize = characterSize;
        m_verticesNeedUpdate = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int BackendText::getCharacterSize() const
    {
        return m_characterSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::setFillColor(const Color& color)
    {
        if (m_fillColor == color)
            return;

        m_fillColor = color;

        // If we weren't already planning to recreate the vertices then we can change the color of the vertices directly
        // instead of having to rebuild the entire vertex array.
        if (m_verticesNeedUpdate)
            return;

        TGUI_ASSERT(m_vertices != nullptr, "m_vertices should have already been created if m_verticesNeedUpdate is false");
        const Vertex::Color vertexColor(color);
        for (std::size_t i = 0; i < m_vertices->size(); ++i)
            (*m_vertices)[i].color = vertexColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color BackendText::getFillColor() const
    {
        return m_fillColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::setOutlineColor(const Color& color)
    {
        if (m_outlineColor == color)
            return;

        m_outlineColor = color;

        // If we weren't already planning to recreate the vertices then we can change the color of the vertices directly
        // instead of having to rebuild the entire vertex array.
        if (m_verticesNeedUpdate)
            return;

        if (m_outlineVertices)
        {
            const Vertex::Color vertexColor(color);
            for (std::size_t i = 0; i < m_outlineVertices->size(); ++i)
                (*m_outlineVertices)[i].color = vertexColor;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color BackendText::getOutlineColor() const
    {
        return m_outlineColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::setOutlineThickness(float thickness)
    {
        if (m_outlineThickness == thickness)
            return;

        m_outlineThickness = thickness;
        m_verticesNeedUpdate = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendText::getOutlineThickness() const
    {
        return m_outlineThickness;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::setStyle(TextStyles style)
    {
        if (m_style == style)
            return;

        m_style = style;
        m_verticesNeedUpdate = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyles BackendText::getStyle() const
    {
        return m_style;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::setFont(const std::shared_ptr<BackendFont>& font)
    {
        if (m_font == font)
            return;

        m_font = font;
        m_verticesNeedUpdate = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendFont> BackendText::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f BackendText::findCharacterPos(std::size_t index) const
    {
        if (!m_font)
            return {};

        if (index > m_string.length())
            index = m_string.length();

        const bool isBold           = static_cast<unsigned int>(m_style) & TextStyle::Bold;
        const float whitespaceWidth = m_font->getGlyph(U' ', m_characterSize, isBold).advance;
        const float lineSpacing     = m_font->getLineSpacing(m_characterSize);

        Vector2f position;
        char32_t prevChar = 0;
        for (std::size_t i = 0; i < index; ++i)
        {
            const char32_t curChar = m_string[i];

            // Skip the carriage return character as we will pretend it isn't there
            if (curChar == U'\r')
                continue;

            // Apply the kerning offset
            position.x += m_font->getKerning(prevChar, curChar, m_characterSize, isBold);
            prevChar = curChar;

            // Handle special characters
            switch (curChar)
            {
                case ' ':  position.x += whitespaceWidth;             continue;
                case '\t': position.x += whitespaceWidth * 4;         continue;
                case '\n': position.y += lineSpacing; position.x = 0; continue;
            }

            // For regular characters, add the advance offset of the glyph
            position.x += m_font->getGlyph(curChar, m_characterSize, isBold).advance;
        }

        return position;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendText::TextVertexData BackendText::getVertexData()
    {
        BackendText::TextVertexData data;

        if (!m_font)
            return data;

        unsigned int textureVersion;
        auto texture = m_font->getTexture(m_characterSize, textureVersion);
        if (!texture)
            return data;

        // If the font texture changes then we need to update the texture coordinates
        if (textureVersion != m_lastFontTextureVersion)
            m_verticesNeedUpdate = true;

        if (m_verticesNeedUpdate)
        {
            updateVertices();

            // It is possible that the texture changes during the update
            texture = m_font->getTexture(m_characterSize, m_lastFontTextureVersion);
        }

        if (m_outlineVertices && !m_outlineVertices->empty())
            data.emplace_back(texture, m_outlineVertices);

        if (m_vertices && !m_vertices->empty())
            data.emplace_back(texture, m_vertices);

        return data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::updateVertices()
    {
        if (!m_font || !m_verticesNeedUpdate)
            return;

        m_verticesNeedUpdate = false;

        if (!m_vertices)
            m_vertices = std::make_shared<std::vector<Vertex>>();
        if (!m_outlineVertices && (m_outlineThickness != 0))
            m_outlineVertices = std::make_shared<std::vector<Vertex>>();

        m_size = {0, 0};
        m_vertices->clear();
        if (m_outlineVertices)
            m_outlineVertices->clear();

        if (m_characterSize == 0)
            return;

        const Vertex::Color vertexFillColor(m_fillColor);
        const Vertex::Color vertexOutlineColor(m_outlineColor);

        const bool isBold              = (static_cast<unsigned int>(m_style) & TextStyle::Bold) != 0;
        const bool isUnderlined        = (static_cast<unsigned int>(m_style) & TextStyle::Underlined) != 0;
        const bool isStrikeThrough     = (static_cast<unsigned int>(m_style) & TextStyle::StrikeThrough) != 0;
        const float italicShear        = (static_cast<unsigned int>(m_style) & TextStyle::Italic) ? 0.20944f : 0.f; // 12 degrees in radians
        const float underlineOffset    = m_font->getUnderlinePosition(m_characterSize);
        const float underlineThickness = m_font->getUnderlineThickness(m_characterSize);
        const float fontScale          = m_font->getFontScale();

        // Compute the location of the strike through dynamically
        // We use the center point of the lowercase 'x' glyph as the reference
        // We reuse the underline thickness as the thickness of the strike through as well
        const FloatRect xBounds = isStrikeThrough ? m_font->getGlyph(U'x', m_characterSize, isBold).bounds : FloatRect{};
        const float strikeThroughOffset = isStrikeThrough ? (xBounds.top + (xBounds.height / 2.f)) : 0.f;

        // Precompute the variables needed by the algorithm
        const float whitespaceWidth = m_font->getGlyph(U' ', m_characterSize, isBold).advance;
        const float lineSpacing = m_font->getLineSpacing(m_characterSize);
        float x = m_outlineThickness;
        float y = m_font->getAscent(m_characterSize) + m_outlineThickness;

        // Create one quad for each character
        float maxX = 0.f;
        char32_t prevChar = 0;
        unsigned int nrLines = 1;
        for (const char32_t curChar : m_string)
        {
            // Skip the carriage return character since we can't render it
            if (curChar == U'\r')
                continue;

            // Apply the kerning offset
            x += m_font->getKerning(prevChar, curChar, m_characterSize, isBold);

            // If we're using the underlined style and there's a new line, draw a line
            if (isUnderlined && (curChar == U'\n' && prevChar != U'\n'))
            {
                addLine(*m_vertices, x, y, vertexFillColor, underlineOffset, underlineThickness, 0, fontScale);

                if (m_outlineThickness != 0)
                    addLine(*m_outlineVertices, x, y, vertexOutlineColor, underlineOffset, underlineThickness, m_outlineThickness, fontScale);
            }

            // If we're using the strike through style and there's a new line, draw a line across all characters
            if (isStrikeThrough && (curChar == U'\n' && prevChar != U'\n'))
            {
                addLine(*m_vertices, x, y, vertexFillColor, strikeThroughOffset, underlineThickness, 0, fontScale);

                if (m_outlineThickness != 0)
                    addLine(*m_outlineVertices, x, y, vertexOutlineColor, strikeThroughOffset, underlineThickness, m_outlineThickness, fontScale);
            }

            prevChar = curChar;

            // Handle special characters
            if ((curChar == U' ') || (curChar == U'\n') || (curChar == U'\t'))
            {
                if (curChar == U'\n')
                {
                    ++nrLines;
                    maxX = std::max(maxX, x);
                }

                switch (curChar)
                {
                    case U' ':  x += whitespaceWidth;     break;
                    case U'\t': x += whitespaceWidth * 4; break;
                    case U'\n': y += lineSpacing; x = 0;  break;
                }

                // Next glyph, no need to create a quad for whitespace
                continue;
            }

            // Apply the outline
            if (m_outlineThickness != 0)
            {
                const auto& glyph = m_font->getGlyph(curChar, m_characterSize, isBold, m_outlineThickness);

                float top    = glyph.bounds.top;
                float right  = glyph.bounds.left + glyph.bounds.width;

                // Add the outline glyph to the vertices
                addGlyphQuad(*m_outlineVertices, {x, y}, vertexOutlineColor, glyph, fontScale, italicShear);
                maxX = std::max(maxX, x + right - italicShear * top - m_outlineThickness);
            }

            // Extract the current glyph's description
            const auto& glyph = m_font->getGlyph(curChar, m_characterSize, isBold);

            // Add the glyph to the vertices
            addGlyphQuad(*m_vertices, {x, y}, vertexFillColor, glyph, fontScale, italicShear);

            // Update the current bounds with the non outlined glyph bounds
            if (m_outlineThickness == 0)
                maxX = std::max(maxX, x + glyph.bounds.left + glyph.bounds.width - italicShear * glyph.bounds.top);

            // Advance to the next character
            x += glyph.advance;
        }

        maxX = std::max(maxX, x);

        // If we're using the underlined style, add the last line
        if (isUnderlined && (x > 0))
        {
            addLine(*m_vertices, x, y, vertexFillColor, underlineOffset, underlineThickness, 0, fontScale);

            if (m_outlineThickness != 0)
                addLine(*m_outlineVertices, x, y, vertexOutlineColor, underlineOffset, underlineThickness, m_outlineThickness, fontScale);
        }

        // If we're using the strike through style, add the last line across all characters
        if (isStrikeThrough && (x > 0))
        {
            addLine(*m_vertices, x, y, vertexFillColor, strikeThroughOffset, underlineThickness, 0, fontScale);

            if (m_outlineThickness != 0)
                addLine(*m_outlineVertices, x, y, vertexOutlineColor, strikeThroughOffset, underlineThickness, m_outlineThickness, fontScale);
        }

        // The height of a line can sometimes be slightly larger than the line spacing returned by the font.
        // To not clip the last line, one line can use the font height instead of the line spacing, to include the extra offset.
        const float fontHeight = m_font->getFontHeight(m_characterSize);
        const float height = std::max(fontHeight, lineSpacing) + (nrLines - 1) * lineSpacing;

        m_size = {maxX + 2 * m_outlineThickness, height + 2 * m_outlineThickness};

        // Normalize the texture coordinates
        const Vector2u textureSize = m_font->getTextureSize(m_characterSize);
        if ((textureSize.x > 0) && (textureSize.y > 0))
        {
            const float textureWidth = static_cast<float>(textureSize.x);
            const float textureHeight = static_cast<float>(textureSize.y);

            for (auto& vertex : *m_vertices)
            {
                vertex.texCoords.x /= textureWidth;
                vertex.texCoords.y /= textureHeight;
            }
            if (m_outlineVertices)
            {
                for (auto& vertex : *m_outlineVertices)
                {
                    vertex.texCoords.x /= textureWidth;
                    vertex.texCoords.y /= textureHeight;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::addGlyphQuad(std::vector<Vertex>& vertices, Vector2f position, const Vertex::Color& color, const FontGlyph& glyph, float fontScale, float italicShear)
    {
        const float padding = 1;

        const float left   = glyph.bounds.left - padding / fontScale;
        const float top    = glyph.bounds.top - padding / fontScale;
        const float right  = glyph.bounds.left + glyph.bounds.width + padding / fontScale;
        const float bottom = glyph.bounds.top  + glyph.bounds.height + padding / fontScale;

        const float u1 = glyph.textureRect.left - padding;
        const float v1 = glyph.textureRect.top - padding;
        const float u2 = glyph.textureRect.left + glyph.textureRect.width + padding;
        const float v2 = glyph.textureRect.top  + glyph.textureRect.height + padding;

        vertices.emplace_back(Vector2f{position.x + left  - italicShear * top   , position.y + top   }, color, Vector2f{u1, v1});
        vertices.emplace_back(Vector2f{position.x + right - italicShear * top   , position.y + top   }, color, Vector2f{u2, v1});
        vertices.emplace_back(Vector2f{position.x + left  - italicShear * bottom, position.y + bottom}, color, Vector2f{u1, v2});
        vertices.emplace_back(Vector2f{position.x + left  - italicShear * bottom, position.y + bottom}, color, Vector2f{u1, v2});
        vertices.emplace_back(Vector2f{position.x + right - italicShear * top   , position.y + top   }, color, Vector2f{u2, v1});
        vertices.emplace_back(Vector2f{position.x + right - italicShear * bottom, position.y + bottom}, color, Vector2f{u2, v2});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendText::addLine(std::vector<Vertex>& vertices, float lineLength, float lineTop, const Vertex::Color& color, float offset, float thickness, float outlineThickness, float fontScale)
    {
        const float top = std::round((lineTop + offset - (thickness / 2)) * fontScale) / fontScale;
        const float bottom = top + std::round(thickness * fontScale) / fontScale;

        vertices.emplace_back(Vector2f{-outlineThickness,             top    - outlineThickness}, color, Vector2f{1, 1});
        vertices.emplace_back(Vector2f{lineLength + outlineThickness, top    - outlineThickness}, color, Vector2f{1, 1});
        vertices.emplace_back(Vector2f{-outlineThickness,             bottom + outlineThickness}, color, Vector2f{1, 1});
        vertices.emplace_back(Vector2f{-outlineThickness,             bottom + outlineThickness}, color, Vector2f{1, 1});
        vertices.emplace_back(Vector2f{lineLength + outlineThickness, top    - outlineThickness}, color, Vector2f{1, 1});
        vertices.emplace_back(Vector2f{lineLength + outlineThickness, bottom + outlineThickness}, color, Vector2f{1, 1});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
