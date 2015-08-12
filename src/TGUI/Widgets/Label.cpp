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
        m_animatedWidget = true;

        addSignal<sf::String>("DoubleClicked");

        m_renderer = std::make_shared<LabelRenderer>(this);
        reload();

        setTextSize(18);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Ptr Label::copy(Label::ConstPtr label)
    {
        if (label)
            return std::make_shared<Label>(*label);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        m_background.setPosition(getPosition());

        m_text.setPosition(std::floor(getPosition().x + getRenderer()->getPadding().left - m_text.getLocalBounds().left + 0.5f),
                           std::floor(getPosition().y + getRenderer()->getPadding().top - m_text.getLocalBounds().top + 0.5f));
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

    void Label::setText(const sf::String& string)
    {
        m_string = string;

        rearrangeText();
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_font = font;

        if (font != nullptr)
            m_text.setFont(*font);

        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextSize(unsigned int size)
    {
        if (size != m_text.getCharacterSize())
        {
            m_text.setCharacterSize(size);

            updatePosition();

            rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextColor(const sf::Color& color)
    {
        m_text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextStyle(sf::Uint32 style)
    {
        m_text.setStyle(style);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Uint32 Label::getTextStyle() const
    {
        return m_text.getStyle();
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

    float Label::getMaximumTextWidth()
    {
        if (m_autoSize)
            return m_maximumTextWidth;
        else
            return getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::initialize(Container *const parent)
    {
        bool autoSize = getAutoSize();
        Widget::initialize(parent);
        setAutoSize(autoSize);

        if (!getFont() && m_parent->getGlobalFont())
            setTextFont(m_parent->getGlobalFont());
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

                m_callback.text = m_text.getString();
                sendSignal("DoubleClicked", m_text.getString());
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
        if (m_theme && primary != "")
        {
            Widget::reload(primary, secondary, force);
        }
        else // Load white theme
        {
            getRenderer()->setBackgroundColor(sf::Color::Transparent);
            getRenderer()->setTextColor({60, 60, 60});
            getRenderer()->setBorderColor({0, 0, 0});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::update()
    {
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
        if (getFont() == nullptr)
            return;

        // Only rearrange the text when a maximum width was given
        if (!m_autoSize || m_maximumTextWidth > 0)
        {
            // Find the maximum width of one line
            float maxWidth = 0;
            if (m_autoSize)
                maxWidth = m_maximumTextWidth;
            else if (getSize().x > getRenderer()->getPadding().left + getRenderer()->getPadding().right)
                maxWidth = getSize().x - getRenderer()->getPadding().left - getRenderer()->getPadding().right;

            m_text.setString("");
            unsigned int index = 0;
            while (index < m_string.getSize())
            {
                unsigned int oldIndex = index;

                float width = 0;
                sf::Uint32 prevChar = 0;
                for (unsigned int i = index; i < m_string.getSize(); ++i)
                {
                    float charWidth;
                    sf::Uint32 curChar = m_string[i];
                    if (curChar == '\n')
                    {
                        index++;
                        break;
                    }
                    else if (curChar == '\t')
                        charWidth = static_cast<float>(getFont()->getGlyph(' ', m_text.getCharacterSize(), false).textureRect.width) * 4;
                    else
                        charWidth = static_cast<float>(getFont()->getGlyph(curChar, m_text.getCharacterSize(), false).textureRect.width);

                    float kerning = static_cast<float>(getFont()->getKerning(prevChar, curChar, m_text.getCharacterSize()));
                    if (width + charWidth + kerning <= maxWidth)
                    {
                        if (curChar == '\t')
                            charWidth = static_cast<float>(getFont()->getGlyph(' ', m_text.getCharacterSize(), false).advance) * 4;
                        else
                            charWidth = static_cast<float>(getFont()->getGlyph(curChar, m_text.getCharacterSize(), false).advance);

                        width += charWidth + kerning;
                        index++;
                    }
                    else
                        break;

                    prevChar = curChar;
                }

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

                if ((index < m_string.getSize()) && (m_string[index-1] != '\n'))
                    m_text.setString(m_text.getString() + m_string.substring(oldIndex, index - oldIndex) + "\n");
                else
                    m_text.setString(m_text.getString() + m_string.substring(oldIndex, index - oldIndex));

                // If the next line starts with just a space, then the space need not be visible
                if ((index < m_string.getSize()) && (m_string[index] == ' '))
                {
                    if ((index == 0) || (!isWhitespace(m_string[index-1])))
                    {
                        if (((index + 1 < m_string.getSize()) && (!isWhitespace(m_string[index + 1]))) || (index + 1 == m_string.getSize()))
                            index++;
                    }
                }
            }
        }
        else // There is no maximum width, so the text should not be changed
            m_text.setString(m_string);

        if (m_autoSize)
        {
            m_size = {m_text.getLocalBounds().width + getRenderer()->getPadding().left + getRenderer()->getPadding().right,
                      m_text.getLocalBounds().height + getRenderer()->getPadding().top + getRenderer()->getPadding().bottom};

            m_background.setSize(getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // When there is no text then there is nothing to draw
        if (m_text.getString().isEmpty())
            return;

        if (m_autoSize)
        {
            // Draw the background
            if (m_background.getFillColor() != sf::Color::Transparent)
                target.draw(m_background, states);

            // Draw the text
            target.draw(m_text, states);
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
            target.draw(m_text, states);

            // Reset the old clipping area
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }

        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);
        if (property == toLower("TextColor"))
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("BackgroundColor"))
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("BorderColor"))
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("Borders"))
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == toLower("Padding"))
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
            if (property == toLower("Borders"))
                setBorders(value.getBorders());
            else if (property == toLower("Padding"))
                setPadding(value.getBorders());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == toLower("TextColor"))
                setTextColor(value.getColor());
            else if (property == toLower("BackgroundColor"))
                setBackgroundColor(value.getColor());
            else if (property == toLower("BorderColor"))
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

        if (property == toLower("Borders"))
            return m_borders;
        else if (property == toLower("Padding"))
            return m_padding;
        else if (property == toLower("TextColor"))
            return m_label->m_text.getColor();
        else if (property == toLower("BackgroundColor"))
            return m_label->m_background.getFillColor();
        else if (property == toLower("BorderColor"))
            return m_borderColor;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> LabelRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();
        pairs["TextColor"] = m_label->m_text.getColor();
        pairs["BackgroundColor"] = m_label->m_background.getFillColor();
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

            m_label->updatePosition();
            m_label->rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setTextColor(const sf::Color& color)
    {
        m_label->m_text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setBackgroundColor(const sf::Color& color)
    {
        m_label->m_background.setFillColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LabelRenderer::setBorderColor(const sf::Color& color)
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
            border.setFillColor(m_borderColor);
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
        auto renderer = std::shared_ptr<LabelRenderer>(new LabelRenderer{*this});
        renderer->m_label = static_cast<Label*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
