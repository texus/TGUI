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


#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Clipping.hpp>

/// TODO: Same font for all lines?

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox()
    {
        m_type = "ChatBox";
        m_callback.widgetType = "ChatBox";
        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ChatBoxRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders({2});
        getRenderer()->setPadding({2, 0, 0, 0});
        getRenderer()->setBackgroundColor({245, 245, 245});

        setSize({200, 126});
        setTextSize(m_textSize);
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

        getRenderer()->getTextureBackground().setSize(getInnerSize());

        updateRendering();
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
        line.text.setColor(color);
        line.text.setOpacity(getRenderer()->getOpacity());
        line.text.setCharacterSize(textSize);
        line.text.setString(text);
        line.text.setFont(font != nullptr ? font : getRenderer()->getFont());

        recalculateLineText(line);

        if (m_newLinesBelowOthers)
            m_lines.push_back(std::move(line));
        else
            m_lines.push_front(std::move(line));

        recalculateFullTextHeight();
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
            return m_lines[lineIndex].text.getFont();
        }
        else // Index too high
            return getRenderer()->getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(std::size_t lineIndex)
    {
        if (lineIndex < m_lines.size())
        {
            m_lines.erase(m_lines.begin() + lineIndex);

            recalculateFullTextHeight();
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
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineLimit()
    {
        return m_maxLines;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_textSize = size;

        // There is a minimum text size
        if (m_textSize < 8)
            m_textSize = 8;

        m_scroll.setScrollAmount(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextColor(Color color)
    {
        m_textColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChatBox::getTextColor() const
    {
        return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLinesStartFromTop(bool startFromTop)
    {
        if (m_linesStartFromTop != startFromTop)
            m_linesStartFromTop = startFromTop;
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

    bool ChatBox::mouseOnWidget(float x, float y) const
    {
        return sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag to true
        m_mouseDown = true;

        // Pass the event to the scrollbar
        if (m_scroll.mouseOnWidget(x - m_scroll.getPosition().x, y - m_scroll.getPosition().y))
            m_scroll.leftMousePressed(x - m_scroll.getPosition().x, y - m_scroll.getPosition().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(float x, float y)
    {
        if (m_scroll.isMouseDown())
            m_scroll.leftMouseReleased(x - m_scroll.getPosition().x, y - m_scroll.getPosition().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Pass the event to the scrollbar when the mouse is on top of it or when we are dragging its thumb
        if (((m_scroll.isMouseDown()) && (m_scroll.isMouseDownOnThumb())) || m_scroll.mouseOnWidget(x, y))
            m_scroll.mouseMoved(x - m_scroll.getPosition().x, y - m_scroll.getPosition().y);
        else
            m_scroll.mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_scroll.mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();
        m_scroll.mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseWheelMoved(int delta, int, int)
    {
        if (m_scroll.getLowValue() < m_scroll.getMaximum())
            m_scroll.mouseWheelMoved(delta, 0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateLineText(Line& line)
    {
        line.text.setString("");

        // Find the maximum width of one line
        Padding padding = getRenderer()->getPadding();
        float maxWidth = getInnerSize().x - m_scroll.getSize().x - padding.left - padding.right;
        if (maxWidth < 0)
            return;

        line.text.setString(Text::wordWrap(maxWidth, line.string, line.text.getFont(), line.text.getCharacterSize(), false));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateAllLines()
    {
        for (auto& line : m_lines)
            recalculateLineText(line);

        recalculateFullTextHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateFullTextHeight()
    {
        m_fullTextHeight = 0;
        for (auto& line : m_lines)
            m_fullTextHeight += line.text.getSize().y;

        // Update the maximum of the scrollbar
        unsigned int oldMaximum = m_scroll.getMaximum();
        m_scroll.setMaximum(static_cast<unsigned int>(m_fullTextHeight));

        // Scroll down to the last item when there is a scrollbar and it is at the bottom
        if (m_newLinesBelowOthers)
        {
            if (((oldMaximum >= m_scroll.getLowValue()) && (m_scroll.getValue() == oldMaximum - m_scroll.getLowValue()))
             || ((oldMaximum <= m_scroll.getLowValue()) && (m_scroll.getMaximum() > m_scroll.getLowValue())))
            {
                m_scroll.setValue(m_scroll.getMaximum() - m_scroll.getLowValue());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateRendering()
    {
        Padding padding = getRenderer()->getPadding();
        m_scroll.setSize({m_scroll.getSize().x, getInnerSize().y - padding.top - padding.bottom});
        m_scroll.setLowValue(static_cast<unsigned int>(getInnerSize().y - padding.top - padding.bottom));

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if ((property == "borders") || (property == "padding"))
        {
            updateSize();
        }
        else if (property == "texturebackground")
        {
            value.getTexture().setSize(getInnerSize());
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if (property == "scrollbar")
        {
            m_scroll.setRenderer(value.getRenderer());
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();

            getRenderer()->getTextureBackground().setOpacity(opacity);
            m_scroll.getRenderer()->setOpacity(opacity);

            for (auto& line : m_lines)
                line.text.setOpacity(opacity);
        }
        else if (property == "font")
        {
            Font font = value.getFont();
            if (font != nullptr)
            {
                // Look for lines that did not have a font yet and give them this font
                bool lineChanged = false;
                for (auto& line : m_lines)
                {
                    if (line.text.getFont() == nullptr)
                    {
                        line.text.setFont(font);
                        lineChanged = true;
                    }
                }

                if (lineChanged)
                    recalculateAllLines();
            }
        }
        else if ((property != "bordercolor")
              && (property != "backgroundcolor"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());
            states.transform.translate({borders.left, borders.top});
        }

        // Draw the background
        if (getRenderer()->getTextureBackground().isLoaded())
            getRenderer()->getTextureBackground().draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), getRenderer()->getBackgroundColor());

        Padding padding = getRenderer()->getPadding();
        states.transform.translate({padding.left, padding.top});

        // Draw the scrollbar
        states.transform.translate(getInnerSize().x - padding.left - padding.right - m_scroll.getSize().x, 0);
        target.draw(m_scroll, states);
        states.transform.translate(-(getInnerSize().x - padding.left - padding.right - m_scroll.getSize().x), 0);

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        Clipping clipping{target, states, {}, {getInnerSize().x - padding.left - padding.right - m_scroll.getSize().x, getInnerSize().y - padding.top - padding.bottom}};

        states.transform.translate({0, -static_cast<float>(m_scroll.getValue())});

        // Put the lines at the bottom of the chat box if needed
        if (!m_linesStartFromTop && (m_fullTextHeight < getInnerSize().y - padding.top - padding.bottom))
            states.transform.translate(0, getInnerSize().y - padding.top - padding.bottom - m_fullTextHeight);

        for (auto& line : m_lines)
        {
            line.text.draw(target, states);
            states.transform.translate(0, line.text.getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
