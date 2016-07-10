/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Clipping.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label()
    {
        m_type = "Label";
        m_callback.widgetType = "Label";

        addSignal<sf::String>("DoubleClicked");

        m_renderer = aurora::makeCopied<LabelRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setTextColor({60, 60, 60});
        getRenderer()->setBorderColor({60, 60, 60});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Ptr Label::copy(Label::ConstPtr label)
    {
        if (label)
            return std::static_pointer_cast<Label>(label->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // You are no longer auto-sizing
        m_autoSize = false;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setText(const sf::String& string)
    {
        m_string = string;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Label::getText() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextSize(unsigned int size)
    {
        if (size != m_textSize)
        {
            m_textSize = size;
            rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Label::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setHorizontalAlignment(HorizontalAlignment alignment)
    {
        m_horizontalAlignment = alignment;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::HorizontalAlignment Label::getHorizontalAlignment() const
    {
        return m_horizontalAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setVerticalAlignment(VerticalAlignment alignment)
    {
        m_verticalAlignment = alignment;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::VerticalAlignment Label::getVerticalAlignment() const
    {
        return m_verticalAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setAutoSize(bool autoSize)
    {
        if (m_autoSize != autoSize)
        {
            m_autoSize = autoSize;
            rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setMaximumTextWidth(float maximumWidth)
    {
        if (m_maximumTextWidth != maximumWidth)
        {
            m_maximumTextWidth = maximumWidth;
            rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Label::getMaximumTextWidth() const
    {
        if (m_autoSize)
            return m_maximumTextWidth;
        else
            return getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setParent(Container* parent)
    {
        bool autoSize = getAutoSize();
        Widget::setParent(parent);
        setAutoSize(autoSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::leftMouseReleased(float x, float y)
    {
        bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(x, y);

        if (mouseDown)
        {
            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                m_callback.text = m_string;
                sendSignal("DoubleClicked", m_string);
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if (property == "textcolor")
        {
            sf::Color color = calcColorOpacity(value.getColor(), getRenderer()->getOpacity());
            for (auto& line : m_lines)
                line.setFillColor(color);
        }
        else if ((property == "borders") || (property == "padding") || (property == "font") || (property == "textstyle"))
        {
            rearrangeText();
        }
        else if (property == "opacity")
        {
            sf::Color color = calcColorOpacity(getRenderer()->getTextColor(), value.getNumber());
            for (auto& line : m_lines)
                line.setFillColor(color);
        }
        else if ((property != "bordercolor") && (property != "backgroundcolor"))
            Widget::rendererChanged(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        // When double-clicking, the second click has to come within 500 milliseconds
        if (m_animationTimeElapsed >= sf::milliseconds(500))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(std::round(getPosition().x), std::round(getPosition().y));

        // Draw the background
        if (getRenderer()->getBackgroundColor() != sf::Color::Transparent)
            drawRectangleShape(target, states, {0,0}, getSize(), getRenderer()->getBackgroundColor());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getPosition(), getSize(), getRenderer()->getBorderColor());

            // Don't try to draw the text when there is no space left for it
            if ((getSize().x <= borders.left + borders.right) || (getSize().y <= borders.top + borders.bottom))
                return;
        }

        // Apply clipping when needed
        std::unique_ptr<Clipping> clipping;
        if (!m_autoSize)
        {
            Padding padding = getRenderer()->getPadding();
            clipping = std::make_unique<Clipping>(target,
                                                  sf::Vector2f{getAbsolutePosition().x + padding.left, getAbsolutePosition().y + padding.top},
                                                  sf::Vector2f{getAbsolutePosition().x + getSize().x - padding.right, getAbsolutePosition().y + getSize().y - padding.bottom}
                                                 );
        }

        // Draw the text
        for (auto& line : m_lines)
            target.draw(line, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::rearrangeText()
    {
        std::shared_ptr<sf::Font> font = getRenderer()->getFont();
        if (!font)
            return;

        // Find the maximum width of one line
        float maxWidth = 0;
        if (m_autoSize)
            maxWidth = m_maximumTextWidth;
        else
        {
            Borders borders = getRenderer()->getPadding() + getRenderer()->getBorders();
            if (getSize().x > borders.left + borders.right)
                maxWidth = getSize().x - getRenderer()->getPadding().left - getRenderer()->getPadding().right;
            else // There is no room for text
                return;
        }

        m_lines.clear();
        unsigned int index = 0;
        unsigned int lineCount = 0;
        float calculatedLabelWidth = 0;
        bool bold = (m_textStyle & sf::Text::Bold) != 0;
        while (index < m_string.getSize())
        {
            lineCount++;
            unsigned int oldIndex = index;

            float width = 0;
            sf::Uint32 prevChar = 0;
            for (std::size_t i = index; i < m_string.getSize(); ++i)
            {
                float charWidth;
                sf::Uint32 curChar = m_string[i];
                if (curChar == '\n')
                {
                    index++;
                    break;
                }
                else if (curChar == '\t')
                    charWidth = static_cast<float>(font->getGlyph(' ', m_textSize, bold).textureRect.width) * 4;
                else
                    charWidth = static_cast<float>(font->getGlyph(curChar, m_textSize, bold).textureRect.width);

                float kerning = static_cast<float>(font->getKerning(prevChar, curChar, m_textSize));
                if ((maxWidth == 0) || (width + charWidth + kerning <= maxWidth))
                {
                    if (curChar == '\t')
                        width += (static_cast<float>(font->getGlyph(' ', m_textSize, bold).advance) * 4) + kerning;
                    else
                        width += static_cast<float>(font->getGlyph(curChar, m_textSize, bold).advance) + kerning;

                    index++;
                }
                else
                    break;

                prevChar = curChar;
            }

            calculatedLabelWidth = std::max(calculatedLabelWidth, width);

            // Every line contains at least one character
            if (index == oldIndex)
                index++;

            // Implement the word-wrap
            if (m_string[index-1] != '\n')
            {
                unsigned int indexWithoutWordWrap = index;

                if ((index < m_string.getSize()) && (!isWhitespace(m_string[index])))
                {
                    unsigned int wordWrapCorrection = 0;
                    while ((index > oldIndex) && (!isWhitespace(m_string[index - 1])))
                    {
                        wordWrapCorrection++;
                        index--;
                    }

                    // The word can't be split but there is no other choice, it does not fit on the line
                    if ((index - oldIndex) <= wordWrapCorrection)
                        index = indexWithoutWordWrap;
                }
            }

            // Add the next line
            m_lines.emplace_back();
            m_lines.back().setFont(*font);
            m_lines.back().setCharacterSize(getTextSize());
            m_lines.back().setStyle(getRenderer()->getTextStyle());
            m_lines.back().setFillColor(calcColorOpacity(getRenderer()->getTextColor(), getRenderer()->getOpacity()));

            if ((index < m_string.getSize()) && (m_string[index-1] != '\n'))
                m_lines.back().setString(m_string.substring(oldIndex, index - oldIndex) + "\n");
            else
                m_lines.back().setString(m_string.substring(oldIndex, index - oldIndex));

            // If the next line starts with just a space, then the space need not be visible
            if ((index < m_string.getSize()) && (m_string[index] == ' '))
            {
                if ((index == 0) || (!isWhitespace(m_string[index-1])))
                {
                    // But two or more spaces indicate that it is not a normal text and the spaces should not be ignored
                    if (((index + 1 < m_string.getSize()) && (!isWhitespace(m_string[index + 1]))) || (index + 1 == m_string.getSize()))
                        index++;
                }
            }
        }

        // There is always at least one line
        lineCount = std::max(1u, lineCount);

        if (m_autoSize)
        {
            Borders borders = getRenderer()->getPadding() + getRenderer()->getBorders();
            m_size = {std::max(calculatedLabelWidth, maxWidth) + borders.left + borders.right,
                      (lineCount * font->getLineSpacing(m_textSize)) + borders.top + borders.bottom};
        }

        // Update the line positions
        {
            Borders borders = getRenderer()->getPadding() + getRenderer()->getBorders();
            if ((getSize().x <= borders.left + borders.right) || (getSize().y <= borders.top + borders.bottom))
                return;

            sf::Vector2f pos{borders.left, borders.top - getTextVerticalCorrection(font, m_textSize, m_textStyle)};

            if (m_verticalAlignment != VerticalAlignment::Top)
            {
                float totalHeight = getSize().y - borders.top - borders.bottom;
                float totalTextHeight = m_lines.size() * font->getLineSpacing(m_textSize);

                if (m_verticalAlignment == VerticalAlignment::Center)
                    pos.y += (totalHeight - totalTextHeight) / 2.f;
                else if (m_verticalAlignment == VerticalAlignment::Bottom)
                    pos.y += totalHeight - totalTextHeight;
            }

            if (m_horizontalAlignment == HorizontalAlignment::Left)
            {
                for (auto& line : m_lines)
                {
                    line.setPosition(pos.x, std::floor(pos.y));
                    pos.y += font->getLineSpacing(m_textSize);
                }
            }
            else // Center or Right alignment
            {
                float totalWidth = getSize().x - borders.left - borders.right;

                for (auto& line : m_lines)
                {
                    line.setPosition(0, 0);

                    std::size_t lastChar = line.getString().getSize();
                    while (lastChar > 0 && isWhitespace(line.getString()[lastChar-1]))
                        lastChar--;

                    float textWidth = line.findCharacterPos(lastChar).x;

                    if (m_horizontalAlignment == HorizontalAlignment::Center)
                        line.setPosition(std::round(pos.x + (totalWidth - textWidth) / 2.f), std::floor(pos.y));
                    else if (m_horizontalAlignment == HorizontalAlignment::Right)
                        line.setPosition(std::round(pos.x + totalWidth - textWidth), std::floor(pos.y));

                    pos.y += font->getLineSpacing(m_textSize);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
