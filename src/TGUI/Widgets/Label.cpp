/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


///TODO: FIX BORDERS in calculations
#include <TGUI/Container.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Loading/Theme.hpp>

#include <SFML/OpenGL.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label()
    {
        m_callback.widgetType = "Label";
        addSignal<sf::String>("DoubleClicked");

        m_renderer = aurora::makeCopied<tgui::LabelRenderer>();
        setRenderer(m_renderer->getData());
        m_background.setFillColor(getRenderer()->getBackgroundColor());
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

    LabelRenderer* Label::getRenderer() const
    {
        return aurora::downcast<LabelRenderer*>(m_renderer.get());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        m_background.setPosition(getPosition());

        if (getFont())
        {
            sf::Vector2f pos{std::round(getPosition().x + getRenderer()->getPadding().left),
                             getPosition().y + getRenderer()->getPadding().top - getTextVerticalCorrection(getFont(), m_textSize, m_textStyle)};

            if (m_verticalAlignment != VerticalAlignment::Top)
            {
                float totalHeight = getSize().y - getRenderer()->getPadding().top - getRenderer()->getPadding().bottom;
                float totalTextHeight = m_lines.size() * getFont()->getLineSpacing(m_textSize);

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
                    pos.y += getFont()->getLineSpacing(m_textSize);
                }
            }
            else // Center or Right alignment
            {
                float totalWidth = getSize().x - getRenderer()->getPadding().left - getRenderer()->getPadding().right;

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

                    pos.y += getFont()->getLineSpacing(m_textSize);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_background.setSize(getSize());

        // You are no longer auto-sizing
        m_autoSize = false;

        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setFont(const Font& font)
    {
        Widget::setFont(font);
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setText(const sf::String& string)
    {
        m_string = string;
        rearrangeText();
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
        updatePosition();
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
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::VerticalAlignment Label::getVerticalAlignment() const
    {
        return m_verticalAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextStyle(sf::Uint32 style)
    {
        m_textStyle = style;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Uint32 Label::getTextStyle() const
    {
        return m_textStyle;
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

    void Label::rendererChanged(const std::string& property, ObjectConverter&& value)
    {
        if (property == "textcolor")
        {
            sf::Color textColor = calcColorOpacity(value.getColor(), getRenderer()->getOpacity());
            for (auto& line : m_lines)
                line.setFillColor(textColor);
        }
        else if (property == "backgroundcolor")
        {
            m_background.setFillColor(calcColorOpacity(value.getColor(), getRenderer()->getOpacity()));
        }
        else if ((property == "borders") || (property == "padding"))
        {
            rearrangeText();
        }
        else if (property == "opacity")
        {
            sf::Color textColor = calcColorOpacity(getRenderer()->getTextColor(), value.getNumber());
            for (auto& line : m_lines)
                line.setFillColor(textColor);

            m_background.setFillColor(calcColorOpacity(getRenderer()->getBackgroundColor(), value.getNumber()));
        }
        else if (property != "bordercolor")
            Widget::rendererChanged(property, std::move(value));
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

    void Label::rearrangeText()
    {
        if (!getFont())
            return;

        // Find the maximum width of one line
        float maxWidth = 0;
        if (m_autoSize)
            maxWidth = m_maximumTextWidth;
        else if (getSize().x > getRenderer()->getPadding().left + getRenderer()->getPadding().right)
            maxWidth = getSize().x - getRenderer()->getPadding().left - getRenderer()->getPadding().right;

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
                    charWidth = static_cast<float>(getFont()->getGlyph(' ', m_textSize, bold).textureRect.width) * 4;
                else
                    charWidth = static_cast<float>(getFont()->getGlyph(curChar, m_textSize, bold).textureRect.width);

                float kerning = static_cast<float>(getFont()->getKerning(prevChar, curChar, m_textSize));
                if ((maxWidth == 0) || (width + charWidth + kerning <= maxWidth))
                {
                    if (curChar == '\t')
                        width += (static_cast<float>(getFont()->getGlyph(' ', m_textSize, bold).advance) * 4) + kerning;
                    else
                        width += static_cast<float>(getFont()->getGlyph(curChar, m_textSize, bold).advance) + kerning;

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
            m_lines.back().setFont(*getFont());
            m_lines.back().setCharacterSize(getTextSize());
            m_lines.back().setStyle(getTextStyle());
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
            m_size = {std::max(calculatedLabelWidth, maxWidth) + getRenderer()->getPadding().left + getRenderer()->getPadding().right,
                      (lineCount * getFont()->getLineSpacing(m_textSize)) + getRenderer()->getPadding().top + getRenderer()->getPadding().bottom};

            m_background.setSize(getSize());
        }

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_autoSize)
        {
            // Draw the background
            if (m_background.getFillColor() != sf::Color::Transparent)
                target.draw(m_background, states);

            // Draw the text
            for (auto& line : m_lines)
                target.draw(line, states);
        }
        else
        {
            const sf::View& view = target.getView();

            // Calculate the scale factor of the view
            float scaleViewX = target.getSize().x / view.getSize().x;
            float scaleViewY = target.getSize().y / view.getSize().y;

            // Get the global position
            sf::Vector2f topLeftPosition = {((getAbsolutePosition().x + getRenderer()->getPadding().left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                            ((getAbsolutePosition().y + getRenderer()->getPadding().top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
            sf::Vector2f bottomRightPosition = {(getAbsolutePosition().x + getSize().x - getRenderer()->getPadding().right - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                (getAbsolutePosition().y + getSize().y - getRenderer()->getPadding().bottom - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

            // Get the old clipping area
            GLint scissor[4];
            glGetIntegerv(GL_SCISSOR_BOX, scissor);

            // Calculate the clipping area
            GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
            GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
            GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
            GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

            // If the object outside the window then don't draw anything
            if (scissorRight < scissorLeft)
                scissorRight = scissorLeft;
            else if (scissorBottom < scissorTop)
                scissorTop = scissorBottom;

            // Draw the background
            if (m_background.getFillColor() != sf::Color::Transparent)
                target.draw(m_background, states);

            // Set the clipping area
            glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

            // Draw the text
            for (auto& line : m_lines)
                target.draw(line, states);

            // Reset the old clipping area
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }

        // Draw the borders around the button
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0, 0, 0, 0})
            drawBorders(target, states, borders, getPosition(), getSize(), calcColorOpacity(getRenderer()->getBorderColor(), getRenderer()->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_RENDERER_PROPERTY_BORDERS(LabelRenderer, Borders, Borders(0))
    TGUI_RENDERER_PROPERTY_BORDERS(LabelRenderer, Padding, Padding(0))
    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, TextColor, Color(60, 60, 60))
    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, BackgroundColor, sf::Color::Transparent)
    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, BorderColor, Color(0, 0, 0))

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
