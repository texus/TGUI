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


#include <TGUI/Text.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <TGUI/Backend/Renderer/BackendText.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <algorithm>
    #include <vector>
    #include <cmath>
#endif

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
        setString(other.getString());
        setPosition(other.m_position);
        setFont(other.m_font);
        setColor(other.m_color);
        setOutlineColor(other.m_outlineColor);
        setStyle(other.getStyle());
        setCharacterSize(other.getCharacterSize());
        setOutlineThickness(other.getOutlineThickness());
        setOpacity(other.m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text& Text::operator=(const Text& other)
    {
        if (this != &other)
        {
            Text temp{other};
            std::swap(m_backendText, temp.m_backendText);
            std::swap(m_position, temp.m_position);
            std::swap(m_font, temp.m_font);
            std::swap(m_color, temp.m_color);
            std::swap(m_outlineColor, temp.m_outlineColor);
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
        m_backendText->setString(string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& Text::getString() const
    {
        return m_backendText->getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setCharacterSize(unsigned int size)
    {
        m_backendText->setCharacterSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Text::getCharacterSize() const
    {
        return m_backendText->getCharacterSize();
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

    void Text::setFont(const Font& font)
    {
        m_font = font;
        m_backendText->setFont(font.getBackendFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font Text::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setStyle(TextStyles style)
    {
        m_backendText->setStyle(style);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyles Text::getStyle() const
    {
        return m_backendText->getStyle();
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
        m_backendText->setOutlineThickness(thickness);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getOutlineThickness() const
    {
        return m_backendText->getOutlineThickness();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::findCharacterPos(std::size_t index) const
    {
        return m_backendText->findCharacterPos(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding() const
    {
        return getExtraHorizontalPadding(getFont(), getCharacterSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding(const Font& font, unsigned int characterSize)
    {
        return std::round(getLineHeight(font, characterSize) / 10.f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalOffset() const
    {
        return getExtraHorizontalOffset(getFont(), getCharacterSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalOffset(const Font& font, unsigned int characterSize)
    {
        return std::round(getLineHeight(font, characterSize) / 6.f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraVerticalPadding(unsigned int characterSize)
    {
        return std::max(1.f, std::round(characterSize / 8.f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineHeight() const
    {
        return getLineHeight(getFont(), getCharacterSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineHeight(const Font& font, unsigned int characterSize)
    {
        return std::max(font.getLineSpacing(characterSize), font.getFontHeight(characterSize));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineWidth() const
    {
        return getLineWidth(getString(), getFont(), getCharacterSize(), getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineWidth(const String &text, const Font& font, unsigned int characterSize, TextStyles textStyle)
    {
        if (font == nullptr)
            return 0.0f;

        const bool bold = (textStyle & TextStyle::Bold) != 0;

        float width = 0.0f;
        char32_t prevChar = 0;
        for (const char32_t curChar : text)
        {
            float charWidth;
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

    unsigned int Text::findBestTextSize(const Font& font, float height, int fit)
    {
        if (!font)
            return 0;

        if (height < 2)
            return 1;

        std::vector<unsigned int> textSizes(static_cast<std::size_t>(height));
        for (unsigned int i = 0; i < static_cast<unsigned int>(height); ++i)
            textSizes[i] = i + 1;

        const auto high = std::lower_bound(textSizes.begin(), textSizes.end(), height,
                                           [&](unsigned int charSize, float h) { return std::max(font.getLineSpacing(charSize), font.getFontHeight(charSize)) < h; });
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

    String Text::wordWrap(float maxWidth, const String& text, const Font& font, unsigned int textSize, bool bold)
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
                {
                    // Skip carriage return characters which aren't rendered (we only use line feed characters to indicate a new line)
                    index++;
                    continue;
                }
                else if (curChar == U'\t')
                    charWidth = font.getGlyph(U' ', textSize, bold).advance * 4;
                else
                    charWidth = font.getGlyph(curChar, textSize, bold).advance;

                const float kerning = font.getKerning(prevChar, curChar, textSize, bold);
                const bool charIsWhitespace = (curChar == U' ') || (curChar == U'\t');

                // We add the character to the line, unless a non-whitespace character exceeds the line length.
                // We don't break on whitespace characters because having a space at the beginning of the line looks wrong.
                if ((maxWidth <= 0) || charIsWhitespace || (width + charWidth + kerning <= maxWidth))
                {
                    width += kerning + charWidth;
                    index++;
                }
                else
                    break;

                prevChar = curChar;
            }

            // We must always add at least one character to the line
            if (index == oldIndex)
                index++;

            // Implement the word-wrap by removing the last few characters from the line
            if (text[index-1] != U'\n')
            {
                const std::size_t indexWithoutWordWrap = index;
                if ((index < text.length()) && (!isWhitespace(text[index])))
                {
                    while ((index > oldIndex) && !isWhitespace(text[index - 1]))
                        index--;

                    // If the entire word doesn't fit on the line then we have no other choice than to simply split the word
                    if (index == oldIndex)
                        index = indexWithoutWordWrap;
                }
            }

            result += text.substr(oldIndex, index - oldIndex);
            if ((index < text.length()) && (text[index-1] != U'\n'))
                result += U'\n';
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::vector<Text::Blueprint>> Text::wordWrap(float maxWidth, const std::vector<std::vector<Blueprint>>& lines, const Font& font)
    {
        if (font == nullptr)
            return {};

        std::vector<std::vector<Blueprint>> result;

        // Input lines can never be combined (they were split by a manual newline character),
        // so we can apply word wrapping on each line separately.
        for (const auto& inputLine : lines)
        {
            result.emplace_back();
            if (inputLine.empty())
                continue;

            // Empty lines don't need to be split
            if ((inputLine.size() == 1) && inputLine[0].text.empty())
            {
                result.back().emplace_back(inputLine[0]);
                continue;
            }

            std::size_t lineLength = 0;
            for (const auto& piece : inputLine)
            {
                if (piece.gapSize != Vector2u{})
                    ++lineLength;
                else
                    lineLength += piece.text.length();
            }

            // Since a piece can start or stop at any letter, it is possible that we need to move an already processed piece
            // to the next line, so we can't just process piece by piece. The index that we use therefore goes across all
            // pieces, and we aditionally store which piece and at which location in the piece the index corresponds to.
            std::size_t index = 0;
            std::size_t pieceIndex = 0;
            std::size_t pieceCharIndex = 0;
            while (index < lineLength)
            {
                const std::size_t oldIndex = index;
                const std::size_t oldPieceIndex = pieceIndex;
                const std::size_t oldPieceCharIndex = pieceCharIndex;

                // Find out how many characters we can get on this line
                float width = 0;
                char32_t prevChar = 0;
                while (index < lineLength)
                {
                    const auto& inputPiece = inputLine[pieceIndex];
                    TGUI_ASSERT(!inputPiece.text.empty() || (inputPiece.gapSize != Vector2u{}), "Blueprint text in Text::wordWrap is not allowed to be empty")

                    char32_t curChar;
                    float charWidth;
                    float kerning = 0;
                    if (inputPiece.gapSize != Vector2u{})
                    {
                        curChar = 0;
                        charWidth = static_cast<float>(inputPiece.gapSize.x);
                        TGUI_ASSERT(inputLine[pieceIndex].text.empty() && (pieceCharIndex == 0), "Blueprint gap in Text::wordWrap should have an empty text");
                    }
                    else // Normal text piece instead of a gap
                    {
                        curChar = inputPiece.text[pieceCharIndex];

                        const bool boldStyle = ((inputPiece.style & TextStyle::Bold) != 0);
                        TGUI_ASSERT(curChar != U'\n' && curChar != U'\r', "Newline characters must be removed before calling Text::wordWrap with blueprints")
                        if (curChar == U'\t')
                            charWidth = font.getGlyph(U' ', inputPiece.characterSize, boldStyle).advance * 4;
                        else
                            charWidth = font.getGlyph(curChar, inputPiece.characterSize, boldStyle).advance;

                        if (pieceCharIndex > 0)
                            kerning = font.getKerning(prevChar, curChar, inputPiece.characterSize, boldStyle);
                        else if ((pieceIndex > 0) && !inputLine[pieceIndex-1].text.empty())
                        {
                            // If this is the first character of the text piece but there was already another piece in front
                            // of it on the same line then we need to figure out the kerning between the two pieces.
                            const bool bold = ((inputLine[pieceIndex-1].style & TextStyle::Bold) != 0) && ((inputLine[pieceIndex].style & TextStyle::Bold) != 0);
                            const unsigned int characterSize = std::min(inputLine[pieceIndex-1].characterSize, inputLine[pieceIndex].characterSize);
                            kerning = font.getKerning(inputLine[pieceIndex-1].text.back(), inputLine[pieceIndex].text.front(), characterSize, bold);
                        }
                    }

                    const bool charIsWhitespace = (curChar == U' ') || (curChar == U'\t');

                    // We add the character to the line, unless a non-whitespace character exceeds the line length.
                    // We don't break on whitespace characters because having a space at the beginning of the line looks wrong.
                    if ((maxWidth <= 0) || charIsWhitespace || (width + charWidth + kerning <= maxWidth))
                        width += kerning + charWidth;
                    else
                        break;

                    prevChar = curChar;

                    ++index;
                    ++pieceCharIndex;
                    if (inputLine[pieceIndex].text.empty() || (pieceCharIndex == inputLine[pieceIndex].text.length()))
                    {
                        pieceCharIndex = 0;
                        ++pieceIndex;
                    }
                }

                // We must always add at least one character to the line
                if (index == oldIndex)
                {
                    ++index;
                    ++pieceCharIndex;
                    if (inputLine[pieceIndex].text.empty() || (pieceCharIndex == inputLine[pieceIndex].text.length()))
                    {
                        pieceCharIndex = 0;
                        ++pieceIndex;
                    }
                }

                // Implement the word-wrap by removing the last few characters from the line
                if ((index < lineLength) && ((inputLine[pieceIndex].gapSize != Vector2u{}) || !isWhitespace(inputLine[pieceIndex].text[pieceCharIndex])))
                {
                    const std::size_t indexWithoutWordWrap = index;
                    const std::size_t pieceIndexWithoutWordWrap = pieceIndex;
                    const std::size_t pieceCharIndexWithoutWordWrap = pieceCharIndex;

                    while (index > oldIndex)
                    {
                        if (inputLine[pieceIndex].gapSize != Vector2u{})
                            break;

                        if (pieceCharIndex > 0)
                        {
                            if (isWhitespace(inputLine[pieceIndex].text[pieceCharIndex - 1]))
                                break;
                        }
                        else
                        {
                            if (inputLine[pieceIndex - 1].gapSize != Vector2u{})
                                break;
                            if (isWhitespace(inputLine[pieceIndex - 1].text.back()))
                                break;
                        }

                        --index;
                        if (pieceCharIndex > 0)
                            --pieceCharIndex;
                        else
                        {
                            --pieceIndex;
                            if (!inputLine[pieceIndex].text.empty())
                                pieceCharIndex = inputLine[pieceIndex].text.length() - 1;
                            else
                                pieceCharIndex = 0;
                        }
                    }

                    // If the entire word doesn't fit on the line then we have no other choice than to simply split the word
                    if (index == oldIndex)
                    {
                        index = indexWithoutWordWrap;
                        pieceIndex = pieceIndexWithoutWordWrap;
                        pieceCharIndex = pieceCharIndexWithoutWordWrap;
                    }
                }

                // The first piece might have been partially placed on the previous line
                if ((oldPieceCharIndex == 0) && (pieceIndex != oldPieceIndex))
                    result.back().emplace_back(inputLine[oldPieceIndex]);
                else
                {
                    const auto& inputPiece = inputLine[oldPieceIndex];

                    TGUI_EMPLACE_BACK(outputPiece, result.back())
                    outputPiece.characterSize = inputPiece.characterSize;
                    outputPiece.style = inputPiece.style;
                    outputPiece.color = inputPiece.color;
                    outputPiece.gapSize = inputPiece.gapSize;

                    if (!inputPiece.text.empty())
                    {
                        if (pieceIndex != oldPieceIndex)
                            outputPiece.text = inputPiece.text.substr(oldPieceCharIndex);
                        else
                            outputPiece.text = inputPiece.text.substr(oldPieceCharIndex, pieceCharIndex - oldPieceCharIndex);
                    }
                }

                // If there are more than 2 pieces on this line then all except first and last can just be copied
                for (std::size_t i = oldPieceIndex + 1; i < pieceIndex; ++i)
                    result.back().emplace_back(inputLine[i]);

                // The last piece might have to be partially placed on the next line
                if ((pieceIndex < inputLine.size()) && (pieceIndex != oldPieceIndex) && (pieceCharIndex > 0))
                {
                    const auto& inputPiece = inputLine[pieceIndex];

                    TGUI_EMPLACE_BACK(outputPiece, result.back())
                    outputPiece.characterSize = inputPiece.characterSize;
                    outputPiece.style = inputPiece.style;
                    outputPiece.color = inputPiece.color;
                    outputPiece.gapSize = inputPiece.gapSize;

                    if (!inputPiece.text.empty())
                        outputPiece.text = inputPiece.text.substr(0, pieceCharIndex);
                }

                if (index < lineLength)
                    result.emplace_back();
            }
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendText> Text::getBackendText() const
    {
        return m_backendText;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
