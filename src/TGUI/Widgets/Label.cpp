/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{}},
                {"bordercolor", Color{60, 60, 60}},
                {"textcolor", Color{60, 60, 60}},
                {"backgroundcolor", sf::Color::Transparent}
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label()
    {
        m_type = "Label";
        m_callback.widgetType = "Label";

        addSignal<sf::String>("DoubleClicked");

        m_renderer = aurora::makeCopied<LabelRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));
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

    void Label::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
            rearrangeText();
        }
        else if (property == "padding")
        {
            m_paddingCached = getRenderer()->getPadding();
            rearrangeText();
        }
        else if (property == "textstyle")
        {
            m_textStyleCached = getRenderer()->getTextStyle();
            rearrangeText();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getRenderer()->getTextColor();
            for (auto& line : m_lines)
                line.setColor(m_textColorCached);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);
            rearrangeText();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            for (auto& line : m_lines)
                line.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
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
        if (m_fontCached == nullptr)
            return;

        // Find the maximum width of one line
        float maxWidth = 0;
        if (m_autoSize)
            maxWidth = m_maximumTextWidth;
        else
        {
            if (getSize().x > m_bordersCached.left + m_bordersCached.right + m_paddingCached.left + m_paddingCached.right)
                maxWidth = getSize().x - m_bordersCached.left - m_bordersCached.right - m_paddingCached.left - m_paddingCached.right;
            else // There is no room for text
                return;
        }

        // Fit the text in the available space
        sf::String string = Text::wordWrap(maxWidth, m_string, m_fontCached, m_textSize, m_textStyleCached & sf::Text::Bold);

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
            m_lines.back().setFont(m_fontCached);
            m_lines.back().setStyle(m_textStyleCached);
            m_lines.back().setColor(m_textColorCached);
            m_lines.back().setOpacity(m_opacityCached);

            if (newLinePos != sf::String::InvalidPos)
                m_lines.back().setString(string.substring(searchPosStart, newLinePos - searchPosStart));
            else
                m_lines.back().setString(string.substring(searchPosStart));

            if (m_lines.back().getSize().x > width)
                width = m_lines.back().getSize().x;

            searchPosStart = newLinePos + 1;
        }

        Outline outline = m_paddingCached + m_bordersCached;

        // Update the size of the label
        if (m_autoSize)
        {
            Widget::setSize({std::max(width, maxWidth) + outline.left + outline.right,
                            (std::max<std::size_t>(m_lines.size(), 1) * m_fontCached.getLineSpacing(m_textSize)) + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize, m_textStyleCached) + outline.top + outline.bottom});
        }

        // Update the line positions
        {
            if ((getSize().x <= outline.left + outline.right) || (getSize().y <= outline.top + outline.bottom))
                return;

            sf::Vector2f pos{outline.left, outline.top};

            if (m_verticalAlignment != VerticalAlignment::Top)
            {
                float totalHeight = getSize().y - outline.top - outline.bottom;
                float totalTextHeight = m_lines.size() * m_fontCached.getLineSpacing(m_textSize);

                if (m_verticalAlignment == VerticalAlignment::Center)
                    pos.y += (totalHeight - totalTextHeight) / 2.f;
                else if (m_verticalAlignment == VerticalAlignment::Bottom)
                    pos.y += totalHeight - totalTextHeight;
            }

            if (m_horizontalAlignment == HorizontalAlignment::Left)
            {
                float lineSpacing = m_fontCached.getLineSpacing(m_textSize);
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

                    pos.y += m_fontCached.getLineSpacing(m_textSize);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(std::round(getPosition().x), std::round(getPosition().y));

        sf::Vector2f innerSize = {getSize().x - m_bordersCached.left - m_bordersCached.right, getSize().y - m_bordersCached.top - m_bordersCached.bottom};

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.left, m_bordersCached.top});
        }

        // Draw the background
        if (m_backgroundColorCached != sf::Color::Transparent)
            drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        // Apply clipping when needed
        std::unique_ptr<Clipping> clipping;
        if (!m_autoSize)
        {
            innerSize.x -= m_paddingCached.left + m_paddingCached.right;
            innerSize.y -= m_paddingCached.top + m_paddingCached.bottom;

            clipping = std::make_unique<Clipping>(target, states, sf::Vector2f{m_paddingCached.left, m_paddingCached.top}, innerSize);
        }

        // Draw the text
        for (const auto& line : m_lines)
            line.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
