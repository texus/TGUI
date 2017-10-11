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
        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ChatBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize({200, 126});
        setTextSize(m_textSize);
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

    ChatBoxRenderer* ChatBox::getSharedRenderer()
    {
        return aurora::downcast<ChatBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChatBoxRenderer* ChatBox::getSharedRenderer() const
    {
        return aurora::downcast<const ChatBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBoxRenderer* ChatBox::getRenderer()
    {
        return aurora::downcast<ChatBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChatBoxRenderer* ChatBox::getRenderer() const
    {
        return aurora::downcast<const ChatBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        m_scroll.setPosition(getSize().x - m_bordersCached.getRight() - m_paddingCached.getRight() - m_scroll.getSize().x,
                             m_bordersCached.getTop() + m_paddingCached.getTop());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteBackground.setSize(getInnerSize());

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        updateRendering();
        setPosition(m_position);
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
        line.text.setOpacity(m_opacityCached);
        line.text.setCharacterSize(textSize);
        line.text.setString(text);
        line.text.setFont(font != nullptr ? font : m_fontCached);

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
            return m_fontCached;
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

    bool ChatBox::mouseOnWidget(sf::Vector2f pos) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMousePressed(sf::Vector2f pos)
    {
        // Set the mouse down flag to true
        m_mouseDown = true;

        // Pass the event to the scrollbar
        if (m_scroll.mouseOnWidget(pos - getPosition()))
            m_scroll.leftMousePressed(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(sf::Vector2f pos)
    {
        if (m_scroll.isMouseDown())
            m_scroll.leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(sf::Vector2f pos)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Pass the event to the scrollbar when the mouse is on top of it or when we are dragging its thumb
        if (((m_scroll.isMouseDown()) && (m_scroll.isMouseDownOnThumb())) || m_scroll.mouseOnWidget(pos - getPosition()))
            m_scroll.mouseMoved(pos - getPosition());
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

    void ChatBox::mouseWheelScrolled(float delta, sf::Vector2f pos)
    {
        if (m_scroll.getLowValue() < m_scroll.getMaximum())
            m_scroll.mouseWheelScrolled(delta, pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateLineText(Line& line)
    {
        line.text.setString("");

        // Find the maximum width of one line
        const float maxWidth = getInnerSize().x - m_scroll.getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
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
        for (const auto& line : m_lines)
            m_fullTextHeight += line.text.getSize().y;

        // Update the maximum of the scrollbar
        const unsigned int oldMaximum = m_scroll.getMaximum();
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
        m_scroll.setSize({m_scroll.getSize().x, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()});
        m_scroll.setLowValue(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "scrollbar")
        {
            m_scroll.setRenderer(getSharedRenderer()->getScrollbar());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_scroll.setInheritedOpacity(m_opacityCached);

            for (auto& line : m_lines)
                line.text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            if (m_fontCached != nullptr)
            {
                // Look for lines that did not have a font yet and give them this font
                bool lineChanged = false;
                for (auto& line : m_lines)
                {
                    if (line.text.getFont() == nullptr)
                    {
                        line.text.setFont(m_fontCached);
                        lineChanged = true;
                    }
                }

                if (lineChanged)
                    recalculateAllLines();
            }
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ChatBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["TextSize"] = make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["TextColor"] = make_unique<DataIO::ValueNode>(Serializer::serialize(m_textColor));

        if (m_maxLines > 0)
            node->propertyValuePairs["LineLimit"] = make_unique<DataIO::ValueNode>(to_string(m_maxLines));

        if (m_linesStartFromTop)
            node->propertyValuePairs["LinesStartFromTop"] = make_unique<DataIO::ValueNode>("true");
        else
            node->propertyValuePairs["LinesStartFromTop"] = make_unique<DataIO::ValueNode>("false");

        if (m_newLinesBelowOthers)
            node->propertyValuePairs["NewLinesBelowOthers"] = make_unique<DataIO::ValueNode>("true");
        else
            node->propertyValuePairs["NewLinesBelowOthers"] = make_unique<DataIO::ValueNode>("false");

        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            const unsigned int lineTextSize = getLineTextSize(i);
            const sf::Color lineTextColor = getLineColor(i);

            auto lineNode = make_unique<DataIO::Node>();
            lineNode->name = "Line";

            lineNode->propertyValuePairs["Text"] = make_unique<DataIO::ValueNode>(Serializer::serialize(getLine(i)));
            if (lineTextSize != m_textSize)
                lineNode->propertyValuePairs["TextSize"] = make_unique<DataIO::ValueNode>(to_string(lineTextSize));
            if (lineTextColor != m_textColor)
                lineNode->propertyValuePairs["Color"] = make_unique<DataIO::ValueNode>(Serializer::serialize(lineTextColor));

            node->children.push_back(std::move(lineNode));
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const sf::RenderStates scrollbarStates = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

        // Draw the scrollbar
        m_scroll.draw(target, scrollbarStates);

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        const Clipping clipping{target, states, {}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - m_scroll.getSize().x, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

        states.transform.translate({0, -static_cast<float>(m_scroll.getValue())});

        // Put the lines at the bottom of the chat box if needed
        if (!m_linesStartFromTop && (m_fullTextHeight < getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()))
            states.transform.translate(0, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - m_fullTextHeight);

        for (const auto& line : m_lines)
        {
            line.text.draw(target, states);
            states.transform.translate(0, line.text.getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
