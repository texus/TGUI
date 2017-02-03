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


#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Clipping.hpp>

#include <cassert>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox()
    {
        m_callback.widgetType = "ChatBox";
        m_draggableWidget = true;

        m_renderer = std::make_shared<ChatBoxRenderer>(this);
        reload();

        setSize({200, 126});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox(const ChatBox& chatBoxToCopy) :
        Widget               {chatBoxToCopy},
        m_textSize           {chatBoxToCopy.m_textSize},
        m_textColor          {chatBoxToCopy.m_textColor},
        m_maxLines           {chatBoxToCopy.m_maxLines},
        m_fullTextHeight     {chatBoxToCopy.m_fullTextHeight},
        m_linesStartFromTop  {chatBoxToCopy.m_linesStartFromTop},
        m_newLinesBelowOthers{chatBoxToCopy.m_newLinesBelowOthers},
        m_scroll             {Scrollbar::copy(chatBoxToCopy.m_scroll)},
        m_lines              (chatBoxToCopy.m_lines) // Did not compile in VS2013 when using braces
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox& ChatBox::operator= (const ChatBox& right)
    {
        if (this != &right)
        {
            ChatBox temp{right};
            Widget::operator=(right);

            std::swap(m_textSize,            temp.m_textSize);
            std::swap(m_textColor,           temp.m_textColor);
            std::swap(m_maxLines,            temp.m_maxLines);
            std::swap(m_fullTextHeight,      temp.m_fullTextHeight);
            std::swap(m_linesStartFromTop,   temp.m_linesStartFromTop);
            std::swap(m_newLinesBelowOthers, temp.m_newLinesBelowOthers);
            std::swap(m_scroll,              temp.m_scroll);
            std::swap(m_lines,               temp.m_lines);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::create()
    {
        return std::make_shared<ChatBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::copy(ChatBox::ConstPtr chatBox)
    {
        if (chatBox)
            return std::static_pointer_cast<ChatBox>(chatBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->m_backgroundTexture.setSize(getSize());

        updateRendering();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getFullSize() const
    {
        return {getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text)
    {
        addLine(text, m_textColor, m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, const sf::Color& color)
    {
        addLine(text, color, m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, unsigned int textSize)
    {
        addLine(text, m_textColor, textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, const sf::Color& color, unsigned int textSize, const Font& font)
    {
        // Remove the oldest line if you exceed the maximum
        if ((m_maxLines > 0) && (m_maxLines == m_lines.size()))
        {
            if (m_newLinesBelowOthers)
                removeLine(0);
            else
                removeLine(m_maxLines-1);
        }

        Line line;
        line.string = text;
        line.font = font.getFont();
        if (line.font == nullptr)
            line.font = getFont();


#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        line.text.setFillColor(color);
#else
        line.text.setColor(color);
#endif
        line.text.setCharacterSize(textSize);
        line.text.setString(text);
        if (line.font != nullptr)
            line.text.setFont(*line.font);

        recalculateLineText(line);

        if (m_newLinesBelowOthers)
            m_lines.push_back(std::move(line));
        else
            m_lines.push_front(std::move(line));

        recalculateFullTextHeight();
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ChatBox::getLine(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].string;
        }
        else // Index too high
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color ChatBox::getLineColor(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].text.getColor();
        }
        else // Index too high
            return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getLineTextSize(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].text.getCharacterSize();
        }
        else // Index too high
            return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<sf::Font> ChatBox::getLineFont(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].font;
        }
        else // Index too high
            return getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(std::size_t lineIndex)
    {
        if (lineIndex < m_lines.size())
        {
            m_lines.erase(m_lines.begin() + lineIndex);

            recalculateFullTextHeight();
            updateDisplayedText();
            return true;
        }
        else // Index too high
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeAllLines()
    {
        m_lines.clear();

        recalculateFullTextHeight();
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineAmount()
    {
        return m_lines.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLineLimit(std::size_t maxLines)
    {
        m_maxLines = maxLines;

        // Remove the oldest lines if there are too many lines
        if ((m_maxLines > 0) && (m_maxLines < m_lines.size()))
        {
            if (m_newLinesBelowOthers)
                m_lines.erase(m_lines.begin(), m_lines.begin() + m_lines.size() - m_maxLines);
            else
                m_lines.erase(m_lines.begin() + m_maxLines, m_lines.end());

            recalculateFullTextHeight();
            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineLimit()
    {
        return m_maxLines;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setFont(const Font& font)
    {
        Widget::setFont(font);

        if (font.getFont() != nullptr)
        {
            // Look for lines that did not have a font yet and give them this font
            bool lineChanged = false;
            for (auto& line : m_lines)
            {
                if (line.font == nullptr)
                {
                    line.font = font.getFont();
                    line.text.setFont(*font.getFont());
                    lineChanged = true;
                }
            }

            if (lineChanged)
                recalculateAllLines();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_textSize = size;

        // There is a minimum text size
        if (m_textSize < 8)
            m_textSize = 8;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextColor(const Color& color)
    {
        m_textColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setScrollbar(Scrollbar::Ptr scrollbar)
    {
        m_scroll = scrollbar;

        if (m_scroll)
        {
            Padding padding = getRenderer()->getScaledPadding();
            m_scroll->setSize({m_scroll->getSize().x, getSize().y - padding.top - padding.bottom});
            m_scroll->setLowValue(static_cast<unsigned int>(getSize().y - padding.top - padding.bottom));
        }

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr ChatBox::getScrollbar() const
    {
        return m_scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLinesStartFromTop(bool startFromTop)
    {
        if (m_linesStartFromTop != startFromTop)
        {
            m_linesStartFromTop = startFromTop;

            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getLinesStartFromTop() const
    {
        return m_linesStartFromTop;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setNewLinesBelowOthers(bool newLinesBelowOthers)
    {
        m_newLinesBelowOthers = newLinesBelowOthers;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getNewLinesBelowOthers() const
    {
        return m_newLinesBelowOthers;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        for (auto& line : m_lines)
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
            line.text.setFillColor({line.text.getFillColor().r, line.text.getFillColor().g, line.text.getFillColor().b, static_cast<sf::Uint8>(opacity * 255)});
#else
            line.text.setColor({line.text.getColor().r, line.text.getColor().g, line.text.getColor().b, static_cast<sf::Uint8>(opacity * 255)});
#endif

        if (m_scroll != nullptr)
            m_scroll->setOpacity(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setParent(Container* parent)
    {
        Widget::setParent(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseOnWidget(float x, float y) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag to true
        m_mouseDown = true;

        // If there is a scrollbar then pass the event
        if (m_scroll)
        {
            x -= getPosition().x;
            y -= getPosition().y;

            // Remember the old scrollbar value
            unsigned int oldValue = m_scroll->getValue();

            // Pass the event
            if (m_scroll->mouseOnWidget(x, y))
                m_scroll->leftMousePressed(x, y);

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_scroll->getValue())
                updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_scroll && getFont())
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_scroll->m_mouseDown)
            {
                x -= getPosition().x;
                y -= getPosition().y;

                // Remember the old scrollbar value
                unsigned int oldValue = m_scroll->getValue();

                // Pass the event
                m_scroll->leftMouseReleased(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_scroll->getValue())
                {
                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_scroll->getValue() == oldValue + 1)
                    {
                        // Decrement the value
                        m_scroll->setValue(m_scroll->getValue()-1);

                        // Scroll down with the whole item height instead of with a single pixel
                        m_scroll->setValue(static_cast<unsigned int>(m_scroll->getValue() + getFont()->getLineSpacing(m_textSize) - (std::fmod(m_scroll->getValue(), getFont()->getLineSpacing(m_textSize)))));
                    }
                    else if (m_scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_scroll->setValue(m_scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (std::fmod(m_scroll->getValue(), getFont()->getLineSpacing(m_textSize)) > 0)
                            m_scroll->setValue(static_cast<unsigned int>(m_scroll->getValue() - std::fmod(m_scroll->getValue(), getFont()->getLineSpacing(m_textSize))));
                        else
                            m_scroll->setValue(static_cast<unsigned int>(m_scroll->getValue() - getFont()->getLineSpacing(m_textSize)));
                    }

                    updateDisplayedText();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            x -= getPosition().x;
            y -= getPosition().y;

            // Check if you are dragging the thumb of the scrollbar
            if ((m_scroll->m_mouseDown) && (m_scroll->m_mouseDownOnThumb))
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_scroll->getValue();

                // Pass the event, even when the mouse is not on top of the scrollbar
                m_scroll->mouseMoved(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_scroll->getValue())
                    updateDisplayedText();
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_scroll->mouseOnWidget(x, y))
                    m_scroll->mouseMoved(x, y);
                else
                    m_scroll->mouseNoLongerOnWidget();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_scroll != nullptr)
            m_scroll->m_mouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();

        if (m_scroll != nullptr)
            m_scroll->m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseWheelMoved(int delta, int x, int y)
    {
        // Only do something when there is a scrollbar
        if (m_scroll != nullptr)
        {
            if (m_scroll->getLowValue() < m_scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_scroll->setValue(static_cast<unsigned int>(m_scroll->getValue() + ((-delta) * getFont()->getLineSpacing(m_textSize))));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta * getFont()->getLineSpacing(m_textSize));

                    // Scroll up
                    if (change < m_scroll->getValue())
                        m_scroll->setValue(m_scroll->getValue() - change);
                    else
                        m_scroll->setValue(0);
                }

                updateDisplayedText();
                mouseMoved(static_cast<float>(x), static_cast<float>(y));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateLineText(Line& line)
    {
        line.text.setString("");
        line.sublines = 0;

        if (!line.font)
            return;

        // Find the maximum width of one line
        Padding padding = getRenderer()->getScaledPadding();
        float maxWidth = getSize().x - padding.left - padding.right;
        if (m_scroll)
            maxWidth -= m_scroll->getSize().x;
        if (maxWidth < 0)
            return;

        unsigned int index = 0;
        while (index < line.string.getSize())
        {
            line.sublines++;
            unsigned int oldIndex = index;

            float width = 0;
            sf::Uint32 prevChar = 0;
            for (std::size_t i = index; i < line.string.getSize(); ++i)
            {
                float charWidth;
                sf::Uint32 curChar = line.string[i];
                if (curChar == '\n')
                {
                    index++;
                    break;
                }
                else if (curChar == '\t')
                    charWidth = static_cast<float>(line.font->getGlyph(' ', line.text.getCharacterSize(), false).textureRect.width) * 4;
                else
                    charWidth = static_cast<float>(line.font->getGlyph(curChar, line.text.getCharacterSize(), false).textureRect.width);

                float kerning = static_cast<float>(line.font->getKerning(prevChar, curChar, line.text.getCharacterSize()));
                if ((maxWidth == 0) || (width + charWidth + kerning <= maxWidth))
                {
                    if (curChar == '\t')
                        width += (static_cast<float>(line.font->getGlyph(' ', line.text.getCharacterSize(), false).advance) * 4) + kerning;
                    else
                        width += static_cast<float>(line.font->getGlyph(curChar, line.text.getCharacterSize(), false).advance) + kerning;

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
            if (line.string[index-1] != '\n')
            {
                unsigned int indexWithoutWordWrap = index;
                if ((index < line.string.getSize()) && (!isWhitespace(line.string[index])))
                {
                    unsigned int wordWrapCorrection = 0;
                    while ((index > oldIndex) && (!isWhitespace(line.string[index - 1])))
                    {
                        wordWrapCorrection++;
                        index--;
                    }

                    // The word can't be split but there is no other choice, it does not fit on the line
                    if ((index - oldIndex) <= wordWrapCorrection)
                        index = indexWithoutWordWrap;
                }
            }

            if ((index < line.string.getSize()) && (line.string[index-1] != '\n'))
                line.text.setString(line.text.getString() + line.string.substring(oldIndex, index - oldIndex) + "\n");
            else
                line.text.setString(line.text.getString() + line.string.substring(oldIndex, index - oldIndex));

            // If the next line starts with just a space, then the space need not be visible
            if ((index < line.string.getSize()) && (line.string[index] == ' '))
            {
                if ((index == 0) || (!isWhitespace(line.string[index-1])))
                {
                    // But two or more spaces indicate that it is not a normal text and the spaces should not be ignored
                    if (((index + 1 < line.string.getSize()) && (!isWhitespace(line.string[index + 1]))) || (index + 1 == line.string.getSize()))
                        index++;
                }
            }
        }

        // There is always at least one line
        line.sublines = std::max(1u, line.sublines);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateAllLines()
    {
        for (auto& line : m_lines)
            recalculateLineText(line);

        recalculateFullTextHeight();
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateFullTextHeight()
    {
        m_fullTextHeight = 0;
        for (auto& line : m_lines)
        {
            if (line.font)
                m_fullTextHeight += line.sublines * line.font->getLineSpacing(line.text.getCharacterSize());
        }

        // Set the maximum of the scrollbar when there is one
        if (m_scroll != nullptr)
        {
            unsigned int oldMaximum = m_scroll->getMaximum();
            m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));

            // Scroll down to the last item when there is a scrollbar and it is at the bottom
            if (m_newLinesBelowOthers)
            {
                if (((oldMaximum >= m_scroll->getLowValue()) && (m_scroll->getValue() == oldMaximum - m_scroll->getLowValue()))
                 || ((oldMaximum <= m_scroll->getLowValue()) && (m_scroll->getMaximum() > m_scroll->getLowValue())))
                {
                    m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getLowValue());
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateDisplayedText()
    {
        if (!m_lines.empty())
        {
            Padding padding = getRenderer()->getScaledPadding();

            sf::Vector2f pos = {padding.left, padding.top};
            if (m_scroll)
                pos.y -= static_cast<float>(m_scroll->getValue());

            // Display the last lines when there is no scrollbar
            if (!m_scroll)
            {
                if (m_fullTextHeight > getSize().y - padding.top - padding.bottom)
                    pos.y -= m_fullTextHeight - getSize().y - padding.top - padding.bottom;
            }

            // Put the lines at the bottom of the chat box if needed
            if (!m_linesStartFromTop && (m_fullTextHeight < getSize().y - padding.top - padding.bottom))
                pos.y += getSize().y - padding.top - padding.bottom - m_fullTextHeight;

            // Actually set the position for all the lines
            for (auto& line : m_lines)
            {
                if (line.font)
                {
                    line.text.setPosition(std::round(pos.x), std::floor(pos.y - getTextVerticalCorrection(line.font, line.text.getCharacterSize())));
                    pos.y += line.sublines * line.font->getLineSpacing(line.text.getCharacterSize());
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateRendering()
    {
        Padding padding = getRenderer()->getScaledPadding();

        if (m_scroll)
        {
            m_scroll->setSize({m_scroll->getSize().x, getSize().y - padding.top - padding.bottom});
            m_scroll->setLowValue(static_cast<unsigned int>(getSize().y - padding.top - padding.bottom));
            m_scroll->setPosition({getSize().x - m_scroll->getSize().x - padding.right, padding.top});
        }

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setPadding({2, 2, 2, 2});
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setBackgroundTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            getRenderer()->setPadding({0, 0, 0, 0});

            Widget::reload(primary, secondary, force);

            if (force)
            {
                if (getRenderer()->m_backgroundTexture.isLoaded())
                    setSize(getRenderer()->m_backgroundTexture.getImageSize());
            }

            updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the background
        getRenderer()->draw(target, states);

        {
            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            Padding padding = getRenderer()->getScaledPadding();
            Clipping clipping{target, states, {padding.left, padding.top}, {getSize().x - padding.left - padding.right, getSize().y - padding.top - padding.bottom}};

            // Draw the text
            for (auto& line : m_lines)
                target.draw(line.text, states);
        }

        // Draw the scrollbar if there is one
        if (m_scroll != nullptr)
            target.draw(*m_scroll, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "padding")
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundimage")
            setBackgroundTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "scrollbar")
        {
            if (toLower(value) == "none")
                m_chatBox->setScrollbar(nullptr);
            else
            {
                if (m_chatBox->getTheme() == nullptr)
                    throw Exception{"Failed to load scrollbar, ChatBox has no connected theme to load the scrollbar with"};

                m_chatBox->setScrollbar(m_chatBox->getTheme()->internalLoad(m_chatBox->m_primaryLoadingParameter,
                                                                            Deserializer::deserialize(ObjectConverter::Type::String, value).getString()));
            }
        }
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else if (property == "padding")
                setPadding(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "backgroundimage")
                setBackgroundTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::String)
        {
            if (property == "scrollbar")
            {
                if (toLower(value.getString()) == "none")
                    m_chatBox->setScrollbar(nullptr);
                else
                {
                    if (m_chatBox->getTheme() == nullptr)
                        throw Exception{"Failed to load scrollbar, ChatBox has no connected theme to load the scrollbar with"};

                    m_chatBox->setScrollbar(m_chatBox->getTheme()->internalLoad(m_chatBox->getPrimaryLoadingParameter(), value.getString()));
                }
            }
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter ChatBoxRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "padding")
            return m_padding;
        else if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "backgroundimage")
            return m_backgroundTexture;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> ChatBoxRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_backgroundTexture.isLoaded())
            pairs["BackgroundImage"] = m_backgroundTexture;
        else
            pairs["BackgroundColor"] = m_backgroundColor;

        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["Padding"] = m_padding;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setBorderColor(const Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setBackgroundColor(const Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setBackgroundTexture(const Texture& texture)
    {
        m_backgroundTexture = texture;
        if (m_backgroundTexture.isLoaded())
        {
            m_backgroundTexture.setSize(m_chatBox->getSize());
            m_backgroundTexture.setColor({m_backgroundTexture.getColor().r, m_backgroundTexture.getColor().g, m_backgroundTexture.getColor().b, static_cast<sf::Uint8>(m_chatBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);

        m_chatBox->updateRendering();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_backgroundTexture.isLoaded())
            target.draw(m_backgroundTexture, states);
        else
        {
            sf::RectangleShape background(m_chatBox->getSize());
            background.setFillColor(calcColorOpacity(m_backgroundColor, m_chatBox->getOpacity()));
            target.draw(background, states);
        }

        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position;
            sf::Vector2f size = m_chatBox->getSize();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition({position.x - m_borders.left, position.y - m_borders.top});
            border.setFillColor(calcColorOpacity(m_borderColor, m_chatBox->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition({position.x, position.y - m_borders.top});
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition({position.x + size.x, position.y});
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition({position.x - m_borders.left, position.y + size.y});
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding ChatBoxRenderer::getScaledPadding() const
    {
        Padding padding = getPadding();
        Padding scaledPadding = padding;

        auto& texture = m_backgroundTexture;
        if (texture.isLoaded())
        {
            switch (texture.getScalingType())
            {
            case Texture::ScalingType::Normal:
                if ((texture.getImageSize().x != 0) && (texture.getImageSize().y != 0))
                {
                    scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                    scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                }
                break;

            case Texture::ScalingType::Horizontal:
                if ((texture.getImageSize().x != 0) && (texture.getImageSize().y != 0))
                {
                    scaledPadding.left = padding.left * (texture.getSize().y / texture.getImageSize().y);
                    scaledPadding.right = padding.right * (texture.getSize().y / texture.getImageSize().y);
                    scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                    scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                }
                break;

            case Texture::ScalingType::Vertical:
                if ((texture.getImageSize().x != 0) && (texture.getImageSize().y != 0))
                {
                    scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.top = padding.top * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.bottom = padding.bottom * (texture.getSize().x / texture.getImageSize().x);
                }
                break;

            case Texture::ScalingType::NineSlice:
                break;
            }
        }

        return scaledPadding;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ChatBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<ChatBoxRenderer>(*this);
        renderer->m_chatBox = static_cast<ChatBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
