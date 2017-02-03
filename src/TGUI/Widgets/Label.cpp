/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Clipping.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label()
    {
        m_callback.widgetType = "Label";

        addSignal<sf::String>("DoubleClicked");

        m_renderer = std::make_shared<LabelRenderer>(this);
        reload();

        setTextSize(m_textSize);
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

    sf::Vector2f Label::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
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

    void Label::setTextColor(const Color& color)
    {
        getRenderer()->setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Label::getTextColor() const
    {
        return getRenderer()->m_textColor;
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

    void Label::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        sf::Color textColor = calcColorOpacity(getRenderer()->m_textColor, getOpacity());
        for (auto& line : m_lines)
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
            line.setFillColor(textColor);
#else
            line.setColor(textColor);
#endif

        m_background.setFillColor(calcColorOpacity(getRenderer()->m_backgroundColor, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Label::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
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

    void Label::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBackgroundColor(sf::Color::Transparent);
        getRenderer()->setTextColor({60, 60, 60});
        getRenderer()->setBorderColor({0, 0, 0});

        if (m_theme && primary != "")
            Widget::reload(primary, secondary, force);
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
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
            m_lines.back().setFillColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
#else
            m_lines.back().setColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
#endif

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
            // Draw the background
            if (m_background.getFillColor() != sf::Color::Transparent)
                target.draw(m_background, states);

            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            Padding padding = getRenderer()->getPadding();
            Clipping clipping{target, states, {getPosition().x + padding.left, getPosition().y + padding.top}, {getSize().x - padding.left - padding.right, getSize().y - padding.top - padding.bottom}};

            // Draw the text
            for (auto& line : m_lines)
                target.draw(line, states);
        }

        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);
        if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "padding")
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else if (property == "padding")
                setPadding(value.getBorders());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter LabelRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "padding")
            return m_padding;
        else if (property == "textcolor")
            return m_textColor;
        else if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "bordercolor")
            return m_borderColor;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> LabelRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();
        pairs["TextColor"] = m_textColor;
        pairs["BackgroundColor"] = m_backgroundColor;
        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["Padding"] = m_padding;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setPadding(const Padding& padding)
    {
        if (padding != getPadding())
        {
            WidgetPadding::setPadding(padding);
            m_label->rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setTextColor(const Color& color)
    {
        m_textColor = color;

        sf::Color textColor = calcColorOpacity(m_textColor, m_label->getOpacity());
		for (auto& line : m_label->m_lines)
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
            line.setFillColor(textColor);
#else
            line.setColor(textColor);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setBackgroundColor(const Color& color)
    {
        m_backgroundColor = color;
        m_label->m_background.setFillColor(calcColorOpacity(m_backgroundColor, m_label->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setBorderColor(const Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the borders around the button
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position = m_label->getPosition();
            sf::Vector2f size = m_label->getSize();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(calcColorOpacity(m_borderColor, m_label->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> LabelRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<LabelRenderer>(*this);
        renderer->m_label = static_cast<Label*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
