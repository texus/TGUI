/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Text.hpp>
#include <TGUI/Global.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (SFML_VERSION_MAJOR == 2) && (SFML_VERSION_MINOR < 4)
    #pragma message("Text class is being compiled without support for outlines (requires SFML >= 2.4)")
#endif

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setPosition(Vector2f position)
    {
        m_position = position;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::getPosition() const
    {
        return m_position;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setString(const String& string)
    {
        m_string = string;
        m_text.setString(sf::String(string));
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& Text::getString() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setCharacterSize(unsigned int size)
    {
        m_text.setCharacterSize(size);
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Text::getCharacterSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setColor(Color color)
    {
        m_color = color;

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setFillColor(sf::Color{Color::applyOpacity(color, m_opacity)});
#else
        m_text.setColor(sf::Color{Color::applyOpacity(color, m_opacity)});
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color Text::getColor() const
    {
        return m_color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOpacity(float opacity)
    {
        m_opacity = opacity;

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setFillColor(sf::Color{Color::applyOpacity(m_color, opacity)});
        m_text.setOutlineColor(sf::Color{Color::applyOpacity(m_outlineColor, opacity)});
#else
        m_text.setColor(Color::applyOpacity(m_color, opacity));
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getOpacity() const
    {
        return m_opacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setFont(Font font)
    {
        m_font = font;

        if (font)
            m_text.setFont(*font.getFont());
        else
        {
            // We can't keep using a pointer to the old font (it might be destroyed), but sf::Text has no function to pass an empty font
            if (m_text.getFont())
            {
                const String& string = getString();
                const unsigned int characterSize = getCharacterSize();
                const TextStyle style = getStyle();

                m_text = sf::Text();
                m_text.setString(sf::String(string));
                m_text.setCharacterSize(characterSize);
                m_text.setStyle(style);

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
                m_text.setFillColor(sf::Color{Color::applyOpacity(getColor(), getOpacity())});
                m_text.setOutlineColor(sf::Color{Color::applyOpacity(getOutlineColor(), getOpacity())});
                m_text.setOutlineThickness(getOutlineThickness());
#else
                m_text.setColor(Color::applyOpacity(getColor(), getOpacity()));
#endif
            }
        }

        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font Text::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setStyle(TextStyle style)
    {
        if (style != m_text.getStyle())
        {
            m_text.setStyle(style);
            recalculateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle Text::getStyle() const
    {
        return m_text.getStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOutlineColor(Color color)
    {
        m_outlineColor = color;

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setOutlineColor(sf::Color{Color::applyOpacity(m_outlineColor, m_opacity)});
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color Text::getOutlineColor() const
    {
        return m_outlineColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOutlineThickness(float thickness)
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setOutlineThickness(thickness);
#else
        (void)thickness;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getOutlineThickness() const
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        return m_text.getOutlineThickness();
#else
        return 0;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::findCharacterPos(std::size_t index) const
    {
        return Vector2f{m_text.findCharacterPos(index)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::recalculateSize()
    {
        if (m_font == nullptr)
        {
            m_size = {0, 0};
            return;
        }

        float width = 0;
        float maxWidth = 0;
        unsigned int lines = 1;
        char32_t prevChar = 0;
        const sf::String& string = m_text.getString();
        const bool bold = (m_text.getStyle() & TextStyle::Bold) != 0;
        const unsigned int textSize = m_text.getCharacterSize();
        for (std::size_t i = 0; i < string.getSize(); ++i)
        {
            const float kerning = m_font.getKerning(prevChar, string[i], textSize);
            if (string[i] == '\n')
            {
                maxWidth = std::max(maxWidth, width);
                width = 0;
                lines++;
            }
            else if (string[i] == '\t')
                width += (static_cast<float>(m_font.getGlyph(' ', textSize, bold).advance) * 4) + kerning;
            else
                width += static_cast<float>(m_font.getGlyph(string[i], textSize, bold).advance) + kerning;

            prevChar = string[i];
        }

        const float extraVerticalSpace = Text::calculateExtraVerticalSpace(m_font, m_text.getCharacterSize(), m_text.getStyle());
        const float height = lines * m_font.getLineSpacing(m_text.getCharacterSize()) + extraVerticalSpace;
        m_size = {std::max(maxWidth, width), height};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding() const
    {
        return getExtraHorizontalPadding(getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding(Font font, unsigned int characterSize, TextStyle textStyle)
    {
        return getLineHeight(font, characterSize, textStyle) / 10.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalOffset() const
    {
        return getExtraHorizontalOffset(getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalOffset(Font font, unsigned int characterSize, TextStyle textStyle)
    {
        return getLineHeight(font, characterSize, textStyle) / 6.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraVerticalPadding(unsigned int characterSize)
    {
        return std::max(1.f, std::round(characterSize / 8.f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineHeight() const
    {
        return getLineHeight(getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineHeight(Font font, unsigned int characterSize, TextStyle textStyle)
    {
        const float extraVerticalSpace = Text::calculateExtraVerticalSpace(font, characterSize, textStyle);
        return font.getLineSpacing(characterSize) + extraVerticalSpace;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineWidth() const
    {
        return getLineWidth(getString(), getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineWidth(const String &text, Font font, unsigned int characterSize, TextStyle textStyle)
    {
        if (font == nullptr)
            return 0.0f;

        const bool bold = (textStyle & TextStyle::Bold) != 0;

        float width = 0.0f;
        char32_t prevChar = 0;
        for (std::size_t i = 0; i < text.length(); ++i)
        {
            float charWidth;
            const char32_t curChar = text[i];
            if (curChar == '\n')
                break;
            else if (curChar == '\t')
                charWidth = font.getGlyph(' ', characterSize, bold).advance * 4.0f;
            else
                charWidth = font.getGlyph(curChar, characterSize, bold).advance;

            const float kerning = font.getKerning(prevChar, curChar, characterSize);

            width = width + charWidth + kerning;
            prevChar = curChar;
        }

        return width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Text::findBestTextSize(Font font, float height, int fit)
    {
        if (!font)
            return 0;

        if (height < 2)
            return 1;

        std::vector<unsigned int> textSizes(static_cast<std::size_t>(height));
        for (unsigned int i = 0; i < static_cast<unsigned int>(height); ++i)
            textSizes[i] = i + 1;

        const auto high = std::lower_bound(textSizes.begin(), textSizes.end(), height,
                                           [&](unsigned int charSize, float h) { return font.getLineSpacing(charSize) + Text::calculateExtraVerticalSpace(font, charSize) < h; });
        if (high == textSizes.end())
            return static_cast<unsigned int>(height);

        const float highLineSpacing = font.getLineSpacing(*high);
        if (highLineSpacing == height)
            return *high;

        const auto low = high - 1;
        const float lowLineSpacing = font.getLineSpacing(*low);

        if (fit < 0)
            return *low;
        else if (fit > 0)
            return *high;
        else
        {
            if (std::abs(height - lowLineSpacing) < std::abs(height - highLineSpacing))
                return *low;
            else
                return *high;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::calculateExtraVerticalSpace(Font font, unsigned int characterSize, TextStyle style)
    {
        if (font == nullptr)
            return 0;

        const bool bold = (style & TextStyle::Bold) != 0;

        // Calculate the height of the first line (char size = everything above baseline, height + top = part below baseline)
        const float lineHeight = characterSize
                                 + font.getGlyph('g', characterSize, bold).bounds.height
                                 + font.getGlyph('g', characterSize, bold).bounds.top;

        // Get the line spacing sfml returns
        const float lineSpacing = font.getLineSpacing(characterSize);

        // Calculate the offset of the text
        return lineHeight - lineSpacing;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String Text::wordWrap(float maxWidth, const String& text, Font font, unsigned int textSize, bool bold, bool dropLeadingSpace)
    {
        if (font == nullptr)
            return U"";

        String result;
        std::size_t index = 0;
        while (index < text.length())
        {
            const std::size_t oldIndex = index;

            // Find out how many characters we can get on this line
            float width = 0;
            char32_t prevChar = 0;
            for (std::size_t i = index; i < text.length(); ++i)
            {
                float charWidth;
                const char32_t curChar = text[i];
                if (curChar == U'\n')
                {
                    index++;
                    break;
                }
                else if (curChar == U'\t')
                    charWidth = font.getGlyph(' ', textSize, bold).advance * 4;
                else
                    charWidth = font.getGlyph(curChar, textSize, bold).advance;

                const float kerning = font.getKerning(prevChar, curChar, textSize);
                if ((maxWidth == 0) || (width + charWidth + kerning <= maxWidth))
                {
                    width += kerning + charWidth;
                    index++;
                }
                else
                    break;

                prevChar = curChar;
            }

            // Every line contains at least one character
            if (index == oldIndex)
                index++;

            // Implement the word-wrap by removing the last few characters from the line
            if (text[index-1] != U'\n')
            {
                const std::size_t indexWithoutWordWrap = index;
                if ((index < text.length()) && (!isWhitespace(text[index])))
                {
                    std::size_t wordWrapCorrection = 0;
                    while ((index > oldIndex) && (!isWhitespace(text[index - 1])))
                    {
                        wordWrapCorrection++;
                        index--;
                    }

                    // The word can't be split but there is no other choice, it does not fit on the line
                    if ((index - oldIndex) <= wordWrapCorrection)
                        index = indexWithoutWordWrap;
                }
            }

            // If the next line starts with just a space, then the space need not be visible
            if (dropLeadingSpace)
            {
                if ((index < text.length()) && (text[index] == ' '))
                {
                    if ((index == 0) || (!isWhitespace(text[index-1])))
                    {
                        // But two or more spaces indicate that it is not a normal text and the spaces should not be ignored
                        if (((index + 1 < text.length()) && (!isWhitespace(text[index + 1]))) || (index + 1 == text.length()))
                            index++;
                    }
                }
            }

            result += text.substr(oldIndex, index - oldIndex);
            if ((index < text.length()) && (text[index-1] != U'\n'))
                result += U"\n";
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
