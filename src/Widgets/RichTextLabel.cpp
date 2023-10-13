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


#include <TGUI/Widgets/RichTextLabel.hpp>

#include <cassert>
#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static constexpr const std::array<std::pair<StringView, char32_t>, 4> symbolNamesMap
    {
        {{U"lt"sv, U'<'},
         {U"gt"sv, U'>'},
         {U"amp"sv, U'&'}}
    };

#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char RichTextLabel::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RichTextLabel::RichTextLabel(const char* typeName, bool initRenderer) :
        Label{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RichTextLabel::Ptr RichTextLabel::create(const String& text)
    {
        auto label = std::make_shared<RichTextLabel>();

        if (!text.empty())
            label->setText(text);

        return label;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RichTextLabel::Ptr RichTextLabel::copy(const RichTextLabel::ConstPtr& label)
    {
        if (label)
            return std::static_pointer_cast<RichTextLabel>(label->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::rendererChanged(const String& property)
    {
        if (property == U"TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            rearrangeText();
        }
        else
            Label::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::rearrangeText()
    {
        if (m_fontCached == nullptr)
        {
            m_lines.clear();
            m_images.clear();
            return;
        }

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached);

        // Show or hide the scrollbar
        if (m_autoSize)
            m_scrollbar->setVisible(false);
        else
        {
            if (m_scrollbarPolicy == Scrollbar::Policy::Always)
            {
                m_scrollbar->setVisible(true);
                m_scrollbar->setAutoHide(false);
            }
            else if (m_scrollbarPolicy == Scrollbar::Policy::Never)
            {
                m_scrollbar->setVisible(false);
            }
            else // Scrollbar::Policy::Automatic
            {
                m_scrollbar->setVisible(true);
                m_scrollbar->setAutoHide(true);
            }
        }

        const Outline outline = {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                                 m_paddingCached.getTop() + m_bordersCached.getTop(),
                                 m_paddingCached.getRight() + m_bordersCached.getRight(),
                                 m_paddingCached.getBottom() + m_bordersCached.getBottom()};

        if ((getSize().x <= outline.getLeft() + outline.getRight()) || (getSize().y <= outline.getTop() + outline.getBottom()))
        {
            m_lines.clear();
            m_images.clear();
            return;
        }

        // Find the maximum width of one line
        float maxWidth;
        if (m_autoSize)
            maxWidth = std::max(0.f, m_maximumTextWidth - 2*textOffset);
        else
        {
            maxWidth = getSize().x - outline.getLeft() - outline.getRight() - 2*textOffset;

            // If the scrollbar is always visible then we take it into account, otherwise we assume there is no scrollbar
            if (m_scrollbarPolicy == Scrollbar::Policy::Always)
                maxWidth -= m_scrollbar->getSize().x;

            if (maxWidth <= 0)
            {
                m_lines.clear();
                m_images.clear();
                return;
            }
        }

        // Convert the text into text pieces and images
        std::vector<Texture> images;
        std::vector<std::vector<Text::Blueprint>> textPiecesLines;
        constructRichLineBlueprints(textPiecesLines, images);

        // Now that the new images are loaded, we can remove the old ones. If we did it in the other order then
        // we wouldn't be able to retrieve the images from the texture manager cache during loading.
        m_lines.clear();
        m_images.clear();

        // Fit the text in the available space
        Optional<std::vector<std::vector<Text::Blueprint>>> wordWrappedLines = (maxWidth > 0)
            ? Text::wordWrap(maxWidth, textPiecesLines, m_fontCached)
            : Optional<std::vector<std::vector<Text::Blueprint>>>();
        auto* textPiecesLinesPtr = wordWrappedLines.has_value() ? &wordWrappedLines.value() : &textPiecesLines;

        const float defaultLineSpacing = m_fontCached.getLineSpacing(m_textSizeCached);
        float requiredTextHeight = calculateTextHeight(*textPiecesLinesPtr, defaultLineSpacing);

        if (!m_autoSize)
        {
            // If the text doesn't fit in the label then we need to run the word-wrap again, but this time taking the scrollbar into account
            if ((m_scrollbarPolicy == Scrollbar::Policy::Automatic) && (requiredTextHeight > getSize().y - outline.getTop() - outline.getBottom()))
            {
                maxWidth -= m_scrollbar->getSize().x;
                if (maxWidth <= 0)
                    return;

                wordWrappedLines = Text::wordWrap(maxWidth, textPiecesLines, m_fontCached);
                textPiecesLinesPtr = &wordWrappedLines.value();

                requiredTextHeight = calculateTextHeight(*textPiecesLinesPtr, defaultLineSpacing);
            }

            m_scrollbar->setSize(m_scrollbar->getSize().x, static_cast<unsigned int>(getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()));
            m_scrollbar->setViewportSize(static_cast<unsigned int>(getSize().y - outline.getTop() - outline.getBottom()));
            m_scrollbar->setMaximum(static_cast<unsigned int>(requiredTextHeight));
            m_scrollbar->setPosition({getSize().x - m_bordersCached.getRight() - m_scrollbar->getSize().x, m_bordersCached.getTop()});
            m_scrollbar->setScrollAmount(m_textSizeCached);
        }

        std::vector<float> lineWidths;
        lineWidths.reserve(textPiecesLinesPtr->size());

        // Split the string in multiple lines and position each text piece
        float width = 0;
        std::size_t imageIndex = 0;
        Vector2f pos{m_paddingCached.getLeft() + textOffset, m_paddingCached.getTop()};
        for (std::size_t i = 0; i < textPiecesLinesPtr->size(); ++i)
        {
            std::size_t imageIndexBeforeLine = imageIndex;

            const auto& textPiecesLine = (*textPiecesLinesPtr)[i];
            TGUI_EMPLACE_BACK(line, m_lines)

            float lineWidth = 0;
            float maxLineHeight = 0;
            for (std::size_t j = 0; j < textPiecesLine.size(); ++j)
            {
                if (textPiecesLine[j].gapSize != Vector2u{})
                {
                    m_images.emplace_back(images[imageIndex]);
                    m_images.back().setPosition({pos.x + lineWidth, pos.y});
                    ++imageIndex;

                    maxLineHeight = std::max(maxLineHeight, static_cast<float>(textPiecesLine[j].gapSize.y));
                    lineWidth += textPiecesLine[j].gapSize.x;
                }
                else // Normal piece of text instead of an image
                {
                    TGUI_EMPLACE_BACK(textPiece, line)
                    textPiece.setCharacterSize(textPiecesLine[j].characterSize);
                    textPiece.setFont(m_fontCached);
                    textPiece.setStyle(textPiecesLine[j].style);
                    textPiece.setColor(textPiecesLine[j].color);
                    textPiece.setOpacity(m_opacityCached);
                    textPiece.setOutlineColor(m_textOutlineColorCached);
                    textPiece.setOutlineThickness(m_textOutlineThicknessCached);
                    textPiece.setString(textPiecesLine[j].text);
                    textPiece.setPosition({pos.x + lineWidth, pos.y});

                    maxLineHeight = std::max(maxLineHeight, textPiece.getSize().y);
                    lineWidth += textPiece.getSize().x;

                    // Take kerning into account
                    if (j > 0 && !textPiecesLine[j-1].text.empty() && !textPiecesLine[j].text.empty())
                    {
                        const bool bold = ((textPiecesLine[j-1].style & TextStyle::Bold) != 0) && ((textPiecesLine[j].style & TextStyle::Bold) != 0);
                        const unsigned int characterSize = std::min(textPiecesLine[j-1].characterSize, textPiecesLine[j].characterSize);
                        lineWidth += m_fontCached.getKerning(textPiecesLine[j-1].text.back(), textPiecesLine[j].text.front(), characterSize, bold);
                    }
                }
            }

            lineWidths.push_back(lineWidth);
            if (lineWidth > width)
                width = lineWidth;

            // If a line contains pieces of multiple sizes then align their bottom position
            for (auto& textPiece : line)
            {
                const float pieceHeight = textPiece.getSize().y;
                if (pieceHeight != maxLineHeight)
                    textPiece.setPosition({textPiece.getPosition().x, textPiece.getPosition().y + (maxLineHeight - pieceHeight)});
            }
            for (std::size_t j = imageIndexBeforeLine; j < imageIndex; ++j)
            {
                const float pieceHeight = m_images[j].getSize().y;
                if (pieceHeight != maxLineHeight)
                    m_images[j].setPosition({m_images[j].getPosition().x, m_images[j].getPosition().y + (maxLineHeight - pieceHeight)});
            }

            pos.y += calculateLineSpacing(textPiecesLine, defaultLineSpacing);
        }

        // Make the maxWidth variable contain the same for auto-sized and manually sized labels below
        if (maxWidth == 0)
            maxWidth = width;

        // Horizontally align the lines now that we know their sizes
        if (m_horizontalAlignment != HorizontalAlignment::Left)
        {
            const float availableWidth = maxWidth;

            imageIndex = 0;
            assert(textPiecesLinesPtr->size() == m_lines.size());
            for (std::size_t i = 0; i < textPiecesLinesPtr->size(); ++i)
            {
                const auto& textPiecesLine = (*textPiecesLinesPtr)[i];
                if (textPiecesLine.empty())
                    continue;

                // If the line ends with whitespace then remove them from the line width for aligning horizontally
                float whitespaceOffset = 0;
                const auto& lastTextPiece = textPiecesLine.back();
                if ((lastTextPiece.gapSize == Vector2u{}) && !lastTextPiece.text.empty())
                {
                    assert(!m_lines[i].empty());

                    std::size_t charsToUse = lastTextPiece.text.length();
                    while (charsToUse > 0 && isWhitespace(lastTextPiece.text[charsToUse-1]))
                        charsToUse--;

                    if (charsToUse != lastTextPiece.text.length())
                        whitespaceOffset = m_lines[i].back().getSize().x - m_lines[i].back().findCharacterPos(charsToUse).x;
                }

                const float lineWidth = lineWidths[i] - whitespaceOffset;

                float horizontalOffset;
                if (m_horizontalAlignment == HorizontalAlignment::Right)
                    horizontalOffset = availableWidth - lineWidth;
                else // if (m_horizontalAlignment == HorizontalAlignment::Center)
                    horizontalOffset = (availableWidth - lineWidth) / 2.f;

                for (auto& textPiece : m_lines[i])
                    textPiece.setPosition({textPiece.getPosition().x + horizontalOffset, textPiece.getPosition().y});

                for (std::size_t j = 0; j < textPiecesLine.size(); ++j)
                {
                    if (textPiecesLine[j].gapSize != Vector2u{})
                    {
                        auto& image = m_images[imageIndex];
                        image.setPosition({image.getPosition().x + horizontalOffset, image.getPosition().y});
                        ++imageIndex;
                    }
                }
            }
        }

        // Update the size of the label
        if (m_autoSize)
        {
            m_autoLayout = AutoLayout::Manual;
            // NOLINTNEXTLINE(bugprone-parent-virtual-call)
            Widget::setSize({maxWidth + outline.getLeft() + outline.getRight() + 2*textOffset, requiredTextHeight + outline.getTop() + outline.getBottom()});
            m_bordersCached.updateParentSize(getSize());
            m_paddingCached.updateParentSize(getSize());

            m_spriteBackground.setSize({getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                        getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()});
        }

        // Vertically align the lines if the text is smaller than the box
        if (m_verticalAlignment != VerticalAlignment::Top)
        {
            const float totalHeight = getSize().y - outline.getTop() - outline.getBottom();

            if (!m_scrollbar->isShown() || (requiredTextHeight < totalHeight))
            {
                float verticalOffset;
                if (m_verticalAlignment == VerticalAlignment::Bottom)
                    verticalOffset = totalHeight - requiredTextHeight;
                else // if (m_verticalAlignment == VerticalAlignment::Center)
                    verticalOffset = (totalHeight - requiredTextHeight) / 2.f;

                for (auto& line : m_lines)
                {
                    for (auto& textPiece : line)
                        textPiece.setPosition({textPiece.getPosition().x, textPiece.getPosition().y + verticalOffset});
                }
                for (auto& image : m_images)
                    image.setPosition({image.getPosition().x, image.getPosition().y + verticalOffset});
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RichTextLabel::calculateLineSpacing(const std::vector<Text::Blueprint>& line, float defaultLineSpacing)
    {
        if (line.size() == 1 && (line[0].gapSize == Vector2u{}) && line[0].characterSize == m_textSizeCached)
            return defaultLineSpacing;

        if (line.empty())
            return 0;

        unsigned int maxGapHeight = 0;
        unsigned int lineMaxTextSize = 0;
        for (const auto& textPiece : line)
        {
            if (textPiece.gapSize != Vector2u{})
                maxGapHeight = std::max(maxGapHeight, textPiece.gapSize.y);
            else
                lineMaxTextSize = std::max(lineMaxTextSize, textPiece.characterSize);
        }

        float lineSpacing = (lineMaxTextSize > 0) ? m_fontCached.getLineSpacing(lineMaxTextSize) : 0;
        if (static_cast<float>(maxGapHeight) > lineSpacing)
            lineSpacing = static_cast<float>(maxGapHeight);

        return lineSpacing;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RichTextLabel::calculateTextHeight(const std::vector<std::vector<Text::Blueprint>>& textPiecesLines, float defaultLineSpacing)
    {
        assert(!textPiecesLines.empty());

        float requiredTextHeight = 0;
        for (const auto& line : textPiecesLines)
            requiredTextHeight += calculateLineSpacing(line, defaultLineSpacing);

        unsigned int lastLineMaxTextSize = 0;
        for (const auto& textPiece : textPiecesLines.back())
            lastLineMaxTextSize = std::max(lastLineMaxTextSize, textPiece.characterSize);

        // Add some extra space below the last line, to not cut of low letters
        const float lastLineFontHeight = m_fontCached.getFontHeight(lastLineMaxTextSize);
        const float lastLineLineSpacing = m_fontCached.getLineSpacing(lastLineMaxTextSize);
        requiredTextHeight += Text::getExtraVerticalPadding(m_textSizeCached);
        if (lastLineFontHeight > lastLineLineSpacing)
            requiredTextHeight += (lastLineFontHeight - lastLineLineSpacing);

        return requiredTextHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::constructRichLineBlueprints(std::vector<std::vector<Text::Blueprint>>& textPiecesLines, std::vector<Texture>& images)
    {
        TGUI_ASSERT(textPiecesLines.empty() && images.empty(), "RichTextLabel::constructRichLineBlueprints must be called with empty vectors")

        // Make sure there is always at least one line
        textPiecesLines.emplace_back();

        unsigned int currentTextSize = m_textSize;
        unsigned int currentTextStyle = m_textStyleCached;
        Color currentColor = m_textColorCached;
        String currentString;
        std::vector<Color> colorStack;
        std::vector<unsigned int> textSizeStack;

        const auto addTextPiece = [&]{
            // Don't add empty pieces, unless the line would otherwise be empty.
            // We need those empty strings to know the line spacing to use for that empty line.
            auto& line = textPiecesLines.back();
            if (!line.empty() && currentString.empty())
                return;

            // Only add a new piece if the previous one contained some text or was an image,
            // otherwise just overwrite the previous piece.
            if (line.empty() || !line.back().text.empty() || (line.back().gapSize != Vector2u{}))
                line.emplace_back();

            Text::Blueprint& textPiece = line.back();
            textPiece.characterSize = currentTextSize;
            textPiece.style = currentTextStyle;
            textPiece.color = currentColor;
            textPiece.text = currentString;
            currentString.clear();
        };

        std::size_t i = 0;
        while (i < m_string.length())
        {
            // Using "break" in the switch will result in the code reaching the end of the loop and adding the
            // character that is being processed to the last string piece.
            // If a special value is parsed (e.g. opening tag for bold text style), then the value of "i"
            // should be set to after the special value and "continue" should be called inside the switch.
            switch (m_string[i])
            {
                case U'\r':
                    continue;

                case U'\n':
                {
                    addTextPiece();
                    textPiecesLines.emplace_back();
                    ++i;
                    continue;
                }

                case U'&':
                {
                    const std::size_t semiColonPos = m_string.find(U';', i + 1);
                    if ((semiColonPos == String::npos) || (semiColonPos <= i + 1))
                        break;

                    const std::size_t symbolLength = semiColonPos - (i + 1);
                    const StringView symbolName(&m_string[i + 1], symbolLength);

                    bool symbolFound = false;
                    for (const auto& pair : symbolNamesMap)
                    {
                        if (symbolName == pair.first)
                        {
                            currentString.append(pair.second);
                            symbolFound = true;
                            break;
                        }
                    }

                    if (symbolFound)
                    {
                        i = semiColonPos + 1;
                        continue;
                    }
                    break;
                }

                case U'<':
                {
                    const std::size_t rightAngleBracketPos = m_string.find(U'>', i + 1);
                    if ((rightAngleBracketPos == String::npos) || (rightAngleBracketPos <= i + 1))
                        break;

                    const bool closingTag = (m_string[i + 1] == U'/');
                    if (closingTag)
                    {
                        if (rightAngleBracketPos == i + 2)
                            break;

                        const std::size_t symbolLength = rightAngleBracketPos - (i + 2);

                        const StringView symbolName(&m_string[i + 2], symbolLength);
                        if (symbolName == U"b")
                        {
                            addTextPiece();
                            currentTextStyle &= ~TextStyle::Bold;
                        }
                        else if (symbolName == U"i")
                        {
                            addTextPiece();
                            currentTextStyle &= ~TextStyle::Italic;
                        }
                        else if (symbolName == U"u")
                        {
                            addTextPiece();
                            currentTextStyle &= ~TextStyle::Underlined;
                        }
                        else if (symbolName == U"s")
                        {
                            addTextPiece();
                            currentTextStyle &= ~TextStyle::StrikeThrough;
                        }
                        else if (symbolName == U"color")
                        {
                            if (!colorStack.empty())
                            {
                                addTextPiece();
                                colorStack.pop_back();
                            }

                            if (colorStack.empty())
                                currentColor = m_textColorCached;
                            else
                                currentColor = colorStack.back();
                        }
                        else if (symbolName == U"size")
                        {
                            if (!textSizeStack.empty())
                            {
                                addTextPiece();
                                textSizeStack.pop_back();
                            }

                            if (textSizeStack.empty())
                                currentTextSize = m_textSize;
                            else
                                currentTextSize = textSizeStack.back();
                        }
                        else
                            break;
                    }
                    else // opening tag
                    {
                        const std::size_t symbolLength = rightAngleBracketPos - (i + 1);
                        const StringView symbolName(&m_string[i + 1], symbolLength);

                        if (symbolName == U"b")
                        {
                            addTextPiece();
                            currentTextStyle |= TextStyle::Bold;
                        }
                        else if (symbolName == U"i")
                        {
                            addTextPiece();
                            currentTextStyle |= TextStyle::Italic;
                        }
                        else if (symbolName == U"u")
                        {
                            addTextPiece();
                            currentTextStyle |= TextStyle::Underlined;
                        }
                        else if (symbolName == U"s")
                        {
                            addTextPiece();
                            currentTextStyle |= TextStyle::StrikeThrough;
                        }
                        else if (viewStartsWith(symbolName, U"color="))
                        {
                            Color newColor = currentColor;
                            const String colorStr(StringView(&symbolName[6], symbolName.length() - 6));
                            if (!colorStr.empty())
                            {
                                try
                                {
                                    newColor = Deserializer::deserialize(ObjectConverter::Type::Color, colorStr).getColor();
                                    if (newColor.isSet())
                                    {
                                        addTextPiece();
                                        currentColor = newColor;
                                    }
                                }
                                catch (const Exception&)
                                {
                                    // Ignore colors that don't exist
                                }
                            }

                            colorStack.push_back(newColor);
                        }
                        else if (viewStartsWith(symbolName, U"size="))
                        {
                            unsigned int newTextSize = currentTextSize;
                            const String textSizeStr(StringView(&symbolName[5], symbolName.length() - 5));

                            if (textSizeStr.attemptToUInt(newTextSize) && (newTextSize > 0))
                            {
                                addTextPiece();
                                currentTextSize = newTextSize;
                            }

                            textSizeStack.push_back(newTextSize);
                        }
                        else if (viewStartsWith(symbolName, U"img="))
                        {
                            const String imageFilename(StringView(&symbolName[4], symbolName.length() - 4));
                            try
                            {
                                auto textureWrapper = Deserializer::deserialize(ObjectConverter::Type::Texture, imageFilename);
                                const Texture& texture = textureWrapper.getTexture();
                                if (texture.getImageSize() != Vector2u{})
                                {
                                    // Finish the text piece before the image. If the image is placed on a new line
                                    // then don't insert an empty text piece here.
                                    if (!currentString.empty())
                                        addTextPiece();

                                    images.push_back(texture);

                                    auto& line = textPiecesLines.back();
                                    TGUI_EMPLACE_BACK(gapPiece, line)
                                    gapPiece.gapSize = texture.getImageSize();
                                }
                            }
                            catch (const Exception&)
                            {
                                // Ignore the texture if we can't load it
                            }
                        }
                        else
                            break;
                    }

                    i = rightAngleBracketPos + 1;
                    continue;
                }

                default:
                    break;
            };

            currentString.append(m_string[i]);
            ++i;
        }

        // Add the remaining text
        addTextPiece();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the background, text and scrollbars
        Label::draw(target, states);

        if (m_bordersCached != Borders{0})
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});

        // Draw the images
        if (m_autoSize)
        {
            for (const auto& image : m_images)
                target.drawSprite(states, image);
        }
        else
        {
            Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                  getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom() - m_paddingCached.getTop() - m_paddingCached.getBottom()};

            target.addClippingLayer(states, {{m_paddingCached.getLeft(), m_paddingCached.getTop()}, innerSize});

            if (m_scrollbar->isShown())
                states.transform.translate({0, -static_cast<float>(m_scrollbar->getValue())});

            for (const auto& image : m_images)
                target.drawSprite(states, image);

            target.removeClippingLayer();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr RichTextLabel::clone() const
    {
        return std::make_shared<RichTextLabel>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
