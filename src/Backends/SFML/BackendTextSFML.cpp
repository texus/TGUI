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


#include <TGUI/Backends/SFML/BackendTextSFML.hpp>
#include <TGUI/Backends/SFML/BackendFontSFML.hpp>

#include <SFML/Config.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Vector2f BackendTextSFML::getSize()
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSFML::setString(const String& string)
    {
        m_text.setString(sf::String::fromUtf32(string.begin(), string.end()));
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSFML::setCharacterSize(unsigned int characterSize)
    {
        m_text.setCharacterSize(characterSize);
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSFML::setFillColor(const Color& color)
    {
        m_text.setFillColor({color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSFML::setOutlineColor(const Color& color)
    {
        m_text.setOutlineColor({color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSFML::setOutlineThickness(float thickness)
    {
        m_text.setOutlineThickness(thickness);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSFML::setStyle(TextStyles style)
    {
        m_text.setStyle(style);
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSFML::setFont(const Font& font)
    {
        if (font)
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendFontSFML>(font.getBackendFont()), "BackendTextSFML::setFont requires font of type BackendFontSFML");
            m_text.setFont(std::static_pointer_cast<BackendFontSFML>(font.getBackendFont())->getInternalFont());
        }
        else
        {
            // If an empty font is replaced by an empty font then no work needs to be done
            if (!m_text.getFont())
                return;

            // sf::Text has no function to pass an empty font and we can't keep using a pointer to the old font (it might be destroyed)
            // So the text object has to be completely recreated with all properties copied except for the font.
            sf::Text newText;
            newText.setString(m_text.getString());
            newText.setCharacterSize(m_text.getCharacterSize());
            newText.setStyle(m_text.getStyle());
            newText.setFillColor(m_text.getFillColor());
            newText.setOutlineColor(m_text.getOutlineColor());
            newText.setOutlineThickness(m_text.getOutlineThickness());
            m_text = std::move(newText);
        }

        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f BackendTextSFML::findCharacterPos(std::size_t index) const
    {
        return m_text.findCharacterPos(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Text& BackendTextSFML::getInternalText()
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Text& BackendTextSFML::getInternalText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextSFML::recalculateSize()
    {
        const sf::Font* font = m_text.getFont();
        if (!font)
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
#if (SFML_VERSION_MAJOR > 2) || (SFML_VERSION_MINOR >= 6)  // bold parameter was added in SFML 2.6
            const float kerning = font->getKerning(prevChar, string[i], textSize, bold);
#else
            const float kerning = font->getKerning(prevChar, string[i], textSize);
#endif
            if (string[i] == '\n')
            {
                maxWidth = std::max(maxWidth, width);
                width = 0;
                lines++;
            }
            else if (string[i] == '\r')
                continue; // Skip carriage return characters which aren't rendered (we only use line feed characters to indicate a new line)
            else if (string[i] == '\t')
                width += (static_cast<float>(font->getGlyph(' ', textSize, bold).advance) * 4) + kerning;
            else
                width += static_cast<float>(font->getGlyph(string[i], textSize, bold).advance) + kerning;

            prevChar = string[i];
        }

        const float extraVerticalSpace = calculateExtraLineSpace(font, m_text.getCharacterSize(), m_text.getStyle());
        const float height = lines * font->getLineSpacing(m_text.getCharacterSize()) + extraVerticalSpace;
        m_size = {std::max(maxWidth, width), height};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BackendTextSFML::calculateExtraLineSpace(const sf::Font* font, unsigned int characterSize, unsigned int style)
    {
        TGUI_ASSERT(font != nullptr, "BackendTextSFML::calculateExtraLineSpace can't be called with nullptr");

        const bool bold = (style & sf::Text::Style::Bold) != 0;

        // Calculate the height of the first line (char size = everything above baseline, height + top = part below baseline)
        const float lineHeight = characterSize
                                 + font->getGlyph('g', characterSize, bold).bounds.height
                                 + font->getGlyph('g', characterSize, bold).bounds.top;

        // Get the line spacing sfml returns
        const float lineSpacing = font->getLineSpacing(characterSize);

        // Calculate the offset of the text
        return lineHeight - lineSpacing;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
