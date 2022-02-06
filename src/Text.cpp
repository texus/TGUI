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


#include <TGUI/Text.hpp>
#include <TGUI/Backend.hpp>
#include <TGUI/BackendText.hpp>
#include <algorithm>
#include <vector>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Text::Text() :
        m_backendText(getBackend()->createText())
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text::Text(const Text& other) :
        m_backendText(getBackend()->createText())
    {
        setString(other.m_string);
        setPosition(other.m_position);
        setFont(other.m_font);
        setColor(other.m_color);
        setOutlineColor(other.m_outlineColor);
        setStyle(other.m_textStyle);
        setCharacterSize(other.m_characterSize);
        setOutlineThickness(other.m_outlineThickness);
        setOpacity(other.m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text& Text::operator=(const Text& other)
    {
        if (this != &other)
        {
            Text temp{other};
            std::swap(m_backendText, temp.m_backendText);
            std::swap(m_string, temp.m_string);
            std::swap(m_position, temp.m_position);
            std::swap(m_font, temp.m_font);
            std::swap(m_color, temp.m_color);
            std::swap(m_outlineColor, temp.m_outlineColor);
            std::swap(m_textStyle, temp.m_textStyle);
            std::swap(m_characterSize, temp.m_characterSize);
            std::swap(m_outlineThickness, temp.m_outlineThickness);
            std::swap(m_opacity, temp.m_opacity);
        }

        return *this;
    }

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
        return m_backendText->getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setString(const String& string)
    {
        m_string = string;
        m_backendText->setString(string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& Text::getString() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setCharacterSize(unsigned int size)
    {
        m_characterSize = size;
        m_backendText->setCharacterSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Text::getCharacterSize() const
    {
        return m_characterSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setColor(Color color)
    {
        m_color = color;
        m_backendText->setFillColor(Color::applyOpacity(color, m_opacity));
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

        m_backendText->setFillColor(Color::applyOpacity(m_color, opacity));
        m_backendText->setOutlineColor(Color::applyOpacity(m_outlineColor, opacity));
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
        m_backendText->setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font Text::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setStyle(TextStyles style)
    {
        if (style == m_textStyle)
            return;

        m_textStyle = style;
        m_backendText->setStyle(style);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyles Text::getStyle() const
    {
        return m_textStyle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOutlineColor(Color color)
    {
        m_outlineColor = color;
        m_backendText->setOutlineColor(Color::applyOpacity(m_outlineColor, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color Text::getOutlineColor() const
    {
        return m_outlineColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOutlineThickness(float thickness)
    {
        m_outlineThickness = thickness;
        m_backendText->setOutlineThickness(thickness);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getOutlineThickness() const
    {
        return m_outlineThickness;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::findCharacterPos(std::size_t index) const
    {
        return m_backendText->findCharacterPos(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding() const
    {
        return getExtraHorizontalPadding(getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding(Font font, unsigned int characterSize, TextStyles textStyle)
    {
        return getLineHeight(font, characterSize, textStyle) / 10.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalOffset() const
    {
        return getExtraHorizontalOffset(getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalOffset(Font font, unsigned int characterSize, TextStyles textStyle)
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

    float Text::getLineHeight(Font font, unsigned int characterSize, TextStyles textStyle)
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

    float Text::getLineWidth(const String &text, Font font, unsigned int characterSize, TextStyles textStyle)
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
            else if (curChar == U'\r')
                continue; // Skip carriage return characters which aren't rendered (we only use line feed characters to indicate a new line)
            else if (curChar == '\t')
                charWidth = font.getGlyph(' ', characterSize, bold).advance * 4.0f;
            else
                charWidth = font.getGlyph(curChar, characterSize, bold).advance;

            const float kerning = font.getKerning(prevChar, curChar, characterSize, bold);

            width += charWidth + kerning;
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

    float Text::calculateExtraVerticalSpace(Font font, unsigned int characterSize, TextStyles style)
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
                else if (curChar == U'\r')
                    continue; // Skip carriage return characters which aren't rendered (we only use line feed characters to indicate a new line)
                else if (curChar == U'\t')
                    charWidth = font.getGlyph(' ', textSize, bold).advance * 4;
                else
                    charWidth = font.getGlyph(curChar, textSize, bold).advance;

                const float kerning = font.getKerning(prevChar, curChar, textSize, bold);
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
                result += U'\n';
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTextBase> Text::getBackendText() const
    {
        return m_backendText;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
