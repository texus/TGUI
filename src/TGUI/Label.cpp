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
#include <TGUI/Label.hpp>

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

        m_background.setFillColor(sf::Color::Transparent);
        m_text.setColor({60, 60, 60});

        setTextSize(18);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Ptr Label::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto label = std::make_shared<Label>();

        if (themeFileFilename != "")
        {
            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                if (it->first == "textcolor")
                    label->setTextColor(extractColorFromString(it->first, it->second));
                else
                    throw Exception{"Unrecognized property '" + it->first + "' in section '" + section + "' in " + loadedThemeFile + "."};
            }
        }

        return label;
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

        m_text.setPosition(std::floor(getPosition().x + m_padding.left - m_text.getLocalBounds().left + 0.5f),
                           std::floor(getPosition().y + m_padding.top - m_text.getLocalBounds().top + 0.5f));
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

    void Label::setPadding(const Padding& padding)
    {
        if (padding != getPadding())
        {
            WidgetPadding::setPadding(padding);

            updatePosition();
            rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
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
        if (getTextFont() == nullptr)
            return;

        // Only rearrange the text when a maximum width was given
        if (!m_autoSize || m_maximumTextWidth > 0)
        {
            // Find the maximum width of one line
            float maxWidth = 0;
            if (m_autoSize)
                maxWidth = m_maximumTextWidth;
            else if (getSize().x > m_padding.left + m_padding.right)
                maxWidth = getSize().x - m_padding.left - m_padding.right;

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
                        charWidth = static_cast<float>(getTextFont()->getGlyph(' ', m_text.getCharacterSize(), false).advance) * 4;
                    else
                        charWidth = static_cast<float>(getTextFont()->getGlyph(curChar, m_text.getCharacterSize(), false).advance);

                    float kerning = static_cast<float>(getTextFont()->getKerning(prevChar, curChar, m_text.getCharacterSize()));
                    if (width + charWidth + kerning <= maxWidth)
                    {
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
            m_size = {m_text.getLocalBounds().width + m_padding.left + m_padding.right, m_text.getLocalBounds().height + m_padding.top + m_padding.bottom};
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
            sf::Vector2f topLeftPosition = {((getAbsolutePosition().x + m_padding.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                            ((getAbsolutePosition().y + m_padding.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
            sf::Vector2f bottomRightPosition = {(getAbsolutePosition().x + getSize().x - m_padding.right - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                (getAbsolutePosition().y + getSize().y - m_padding.bottom - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

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

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            // Draw left border
            sf::RectangleShape border({m_borders.left, getSize().y + m_borders.top});
            border.setPosition(getPosition().x - m_borders.left, getPosition().y - m_borders.top);
            border.setFillColor(m_borderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize({getSize().x + m_borders.right, m_borders.top});
            border.setPosition(getPosition().x, getPosition().y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, getSize().y + m_borders.bottom});
            border.setPosition(getPosition().x + getSize().x, getPosition().y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({getSize().x + m_borders.left, m_borders.bottom});
            border.setPosition(getPosition().x - m_borders.left, getPosition().y + getSize().y);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
