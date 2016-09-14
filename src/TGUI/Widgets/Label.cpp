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

    Label::Ptr Label::create(sf::String text)
    {
        auto label = std::make_shared<Label>();

        if (!text.isEmpty())
            label->setText(text);

        return label;
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

    void Label::leftMouseReleased(sf::Vector2f pos)
    {
        bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

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
            Color color = value.getColor();
            for (auto& line : m_lines)
                line.setColor(color);
        }
        else if ((property == "borders") || (property == "padding") || (property == "font") || (property == "textstyle"))
        {
            rearrangeText();
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();
            for (auto& line : m_lines)
                line.setOpacity(opacity);
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

    void Label::rearrangeText()
    {
        if (getRenderer()->getFont() == nullptr)
            return;

        // Find the maximum width of one line
        float maxWidth = 0;
        if (m_autoSize)
            maxWidth = m_maximumTextWidth;
        else
        {
            Borders borders = getRenderer()->getBorders();
            Padding padding = getRenderer()->getPadding();
            if (getSize().x > borders.left + borders.right + padding.left + padding.right)
                maxWidth = getSize().x - borders.left - borders.right - padding.left - padding.right;
            else // There is no room for text
                return;
        }

        // Fit the text in the available space
        sf::String string = Text::wordWrap(maxWidth, m_string, getRenderer()->getFont(), m_textSize, m_textStyle & sf::Text::Bold);

        // Split the string in multiple lines
        m_lines.clear();
        float width = 0;
        std::size_t searchPosStart = 0;
        std::size_t newLinePos = 0;
        while (newLinePos != sf::String::InvalidPos)
        {
            newLinePos = string.find('\n', searchPosStart);

            m_lines.emplace_back();
            m_lines.back().setCharacterSize(getTextSize());
            m_lines.back().setFont(getRenderer()->getFont());
            m_lines.back().setStyle(getRenderer()->getTextStyle());
            m_lines.back().setColor(getRenderer()->getTextColor());
            m_lines.back().setOpacity(getRenderer()->getOpacity());

            if (newLinePos != sf::String::InvalidPos)
                m_lines.back().setString(string.substring(searchPosStart, newLinePos - searchPosStart));
            else
                m_lines.back().setString(string.substring(searchPosStart));

            if (m_lines.back().getSize().x > width)
                width = m_lines.back().getSize().x;

            searchPosStart = newLinePos + 1;
        }

        std::shared_ptr<sf::Font> font = getRenderer()->getFont();
        Outline outline = getRenderer()->getPadding() + getRenderer()->getBorders();

        // Update the size of the label
        if (m_autoSize)
        {
            m_size = {std::max(width, maxWidth) + outline.left + outline.right,
                      (std::max<std::size_t>(m_lines.size(), 1) * font->getLineSpacing(m_textSize)) + Text::calculateExtraVerticalSpace(font, m_textSize, m_textStyle) + outline.top + outline.bottom};
        }

        // Update the line positions
        {
            if ((getSize().x <= outline.left + outline.right) || (getSize().y <= outline.top + outline.bottom))
                return;

            sf::Vector2f pos{getRenderer()->getBorders().left, getRenderer()->getBorders().top};

            if (m_verticalAlignment != VerticalAlignment::Top)
            {
                float totalHeight = getSize().y - outline.top - outline.bottom;
                float totalTextHeight = m_lines.size() * font->getLineSpacing(m_textSize);

                if (m_verticalAlignment == VerticalAlignment::Center)
                    pos.y += (totalHeight - totalTextHeight) / 2.f;
                else if (m_verticalAlignment == VerticalAlignment::Bottom)
                    pos.y += totalHeight - totalTextHeight;
            }

            if (m_horizontalAlignment == HorizontalAlignment::Left)
            {
                float lineSpacing = font->getLineSpacing(m_textSize);
                for (auto& line : m_lines)
                {
                    line.setPosition(pos.x, pos.y);
                    pos.y += lineSpacing;
                }
            }
            else // Center or Right alignment
            {
                float totalWidth = getSize().x - outline.left - outline.right;

                for (auto& line : m_lines)
                {
                    line.setPosition(0, 0);

                    std::size_t lastChar = line.getString().getSize();
                    while (lastChar > 0 && isWhitespace(line.getString()[lastChar-1]))
                        lastChar--;

                    float textWidth = line.findCharacterPos(lastChar).x;

                    if (m_horizontalAlignment == HorizontalAlignment::Center)
                        line.setPosition(pos.x + ((totalWidth - textWidth) / 2.f), pos.y);
                    else if (m_horizontalAlignment == HorizontalAlignment::Right)
                        line.setPosition(pos.x + totalWidth - textWidth, pos.y);

                    pos.y += font->getLineSpacing(m_textSize);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(std::round(getPosition().x), std::round(getPosition().y));

        Borders borders = getRenderer()->getBorders();
        sf::Vector2f innerSize = {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};

        // Draw the borders
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());
            states.transform.translate({borders.left, borders.top});
        }

        // Draw the background
        if (getRenderer()->getBackgroundColor() != sf::Color::Transparent)
            drawRectangleShape(target, states, innerSize, getRenderer()->getBackgroundColor());

        // Apply clipping when needed
        std::unique_ptr<Clipping> clipping;
        if (!m_autoSize)
        {
            Padding padding = getRenderer()->getPadding();
            innerSize.x -= padding.left + padding.right;
            innerSize.y -= padding.top + padding.bottom;

            clipping = std::make_unique<Clipping>(target, states, sf::Vector2f{padding.left, padding.top}, innerSize);
        }

        // Draw the text
        for (const auto& line : m_lines)
            line.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
