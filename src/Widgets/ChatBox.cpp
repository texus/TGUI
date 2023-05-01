/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char ChatBox::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox(const char* typeName, bool initRenderer) :
        Widget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ChatBoxRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setSize({Text::getLineHeight(m_fontCached, m_textSizeCached) * 18,
                     Text::getLineHeight(m_fontCached, m_textSizeCached) * 8
                     + Text::getExtraVerticalPadding(m_textSizeCached)
                     + m_paddingCached.getTop() + m_paddingCached.getBottom()
                     + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::create()
    {
        return std::make_shared<ChatBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::copy(const ChatBox::ConstPtr& chatBox)
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

    void ChatBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteBackground.setSize(getInnerSize());

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_scroll->setPosition(getSize().x - m_bordersCached.getRight() - m_scroll->getSize().x, m_bordersCached.getTop());
        m_scroll->setSize({m_scroll->getSize().x, getInnerSize().y});
        m_scroll->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const String& text)
    {
        addLine(text, m_textColor, m_textStyle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const String& text, Color color)
    {
        addLine(text, color, m_textStyle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const String& text, Color color, TextStyles style)
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
        line.text.setStyle(style);
        line.text.setOpacity(m_opacityCached);
        line.text.setCharacterSize(m_textSizeCached);
        line.text.setString(text);
        line.text.setFont(m_fontCached);

        recalculateLineText(line);

        if (m_newLinesBelowOthers)
            m_lines.push_back(std::move(line));
        else
            m_lines.push_front(std::move(line));

        recalculateFullTextHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String ChatBox::getLine(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].string;
        }
        else // Index too high
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color ChatBox::getLineColor(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].text.getColor();
        }
        else // Index too high
            return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyles ChatBox::getLineTextStyle(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
            return m_lines[lineIndex].text.getStyle();
        else // Index too high
            return m_textStyle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(std::size_t lineIndex)
    {
        if (lineIndex < m_lines.size())
        {
            m_lines.erase(m_lines.begin() + static_cast<std::ptrdiff_t>(lineIndex));

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
                m_lines.erase(m_lines.begin(), m_lines.begin() + static_cast<std::ptrdiff_t>(m_lines.size() - m_maxLines));
            else
                m_lines.erase(m_lines.begin() + static_cast<std::ptrdiff_t>(m_maxLines), m_lines.end());

            recalculateFullTextHeight();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineLimit()
    {
        return m_maxLines;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateTextSize()
    {
        m_scroll->setScrollAmount(m_textSizeCached);

        for (auto& line : m_lines)
            line.text.setCharacterSize(m_textSizeCached);

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextColor(Color color)
    {
        m_textColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& ChatBox::getTextColor() const
    {
        return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextStyle(TextStyles style)
    {
        m_textStyle = style;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyles ChatBox::getTextStyle() const
    {
        return m_textStyle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLinesStartFromTop(bool startFromTop)
    {
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

    void ChatBox::setScrollbarValue(unsigned int value)
    {
        m_scroll->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getScrollbarValue() const
    {
        return m_scroll->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::isMouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isSet() || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);

        // Pass the event to the scrollbar
        bool isDragging = false;
        if (m_scroll->isMouseOnWidget(pos - getPosition()))
            isDragging = m_scroll->leftMousePressed(pos - getPosition());

        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(Vector2f pos)
    {
        if (m_scroll->isMouseDown())
            m_scroll->leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(Vector2f pos)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Pass the event to the scrollbar when the mouse is on top of it or when we are dragging its thumb
        if (((m_scroll->isMouseDown()) && (m_scroll->isMouseDownOnThumb())) || m_scroll->isMouseOnWidget(pos - getPosition()))
            m_scroll->mouseMoved(pos - getPosition());
        else
            m_scroll->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_scroll->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();
        m_scroll->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::scrolled(float delta, Vector2f pos, bool touch)
    {
        if (m_scroll->getViewportSize() < m_scroll->getMaximum())
            return m_scroll->scrolled(delta, pos - getPosition(), touch);

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateLineText(Line& line)
    {
        line.text.setString("");

        // Find the maximum width of one line
        const float maxWidth = getInnerSize().x - m_scroll->getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (maxWidth < 0)
            return;

        line.text.setString(Text::wordWrap(maxWidth, line.string, m_fontCached, line.text.getCharacterSize(), false));
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
        const unsigned int oldMaximum = m_scroll->getMaximum();
        m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight + Text::getExtraVerticalPadding(m_textSizeCached)));

        // Scroll down to the last item when there is a scrollbar and it is at the bottom
        if (m_newLinesBelowOthers)
        {
            if (((oldMaximum >= m_scroll->getViewportSize()) && (m_scroll->getValue() == oldMaximum - m_scroll->getViewportSize()))
             || ((oldMaximum <= m_scroll->getViewportSize()) && (m_scroll->getMaximum() > m_scroll->getViewportSize())))
            {
                m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getViewportSize());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == U"Padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"Scrollbar")
        {
            m_scroll->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (getSharedRenderer()->getScrollbarWidth() == 0)
            {
                m_scroll->setSize({m_scroll->getDefaultWidth(), m_scroll->getSize().y});
                setSize(m_size);
            }
        }
        else if (property == U"ScrollbarWidth")
        {
            const float width = (getSharedRenderer()->getScrollbarWidth() != 0) ? getSharedRenderer()->getScrollbarWidth() : m_scroll->getDefaultWidth();
            m_scroll->setSize({width, m_scroll->getSize().y});
            setSize(m_size);
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_scroll->setInheritedOpacity(m_opacityCached);

            for (auto& line : m_lines)
                line.text.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);

            for (auto& line : m_lines)
                line.text.setFont(m_fontCached);

            recalculateAllLines();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ChatBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs[U"TextColor"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_textColor));

        if (m_textStyle != TextStyle::Regular)
            node->propertyValuePairs[U"TextStyle"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_textStyle));

        if (m_maxLines > 0)
            node->propertyValuePairs[U"LineLimit"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maxLines));

        if (m_linesStartFromTop)
            node->propertyValuePairs[U"LinesStartFromTop"] = std::make_unique<DataIO::ValueNode>("true");
        else
            node->propertyValuePairs[U"LinesStartFromTop"] = std::make_unique<DataIO::ValueNode>("false");

        if (m_newLinesBelowOthers)
            node->propertyValuePairs[U"NewLinesBelowOthers"] = std::make_unique<DataIO::ValueNode>("true");
        else
            node->propertyValuePairs[U"NewLinesBelowOthers"] = std::make_unique<DataIO::ValueNode>("false");

        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            auto lineNode = std::make_unique<DataIO::Node>();
            lineNode->name = "Line";

            lineNode->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getLine(i)));

            const Color lineTextColor = getLineColor(i);
            if (lineTextColor != m_textColor)
                lineNode->propertyValuePairs[U"Color"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(lineTextColor));

            const TextStyles lineTextStyle = getLineTextStyle(i);
            if (lineTextStyle != m_textStyle)
                lineNode->propertyValuePairs[U"Style"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(lineTextStyle));

            node->children.push_back(std::move(lineNode));
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"TextColor"])
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, node->propertyValuePairs[U"TextColor"]->value).getColor());
        if (node->propertyValuePairs[U"TextStyle"])
            setTextStyle(Deserializer::deserialize(ObjectConverter::Type::TextStyle, node->propertyValuePairs[U"TextStyle"]->value).getTextStyle());
        if (node->propertyValuePairs[U"LineLimit"])
            setLineLimit(node->propertyValuePairs[U"LineLimit"]->value.toUInt());

        for (const auto& childNode : node->children)
        {
            if (childNode->name == U"Line")
            {
                Color lineTextColor = getTextColor();
                if (childNode->propertyValuePairs[U"Color"])
                    lineTextColor = Deserializer::deserialize(ObjectConverter::Type::Color, childNode->propertyValuePairs[U"Color"]->value).getColor();

                TextStyles lineTextStyle = getTextStyle();
                if (childNode->propertyValuePairs[U"Style"])
                    lineTextStyle = Deserializer::deserialize(ObjectConverter::Type::TextStyle, childNode->propertyValuePairs[U"Style"]->value).getTextStyle();

                if (childNode->propertyValuePairs[U"Text"])
                    addLine(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs[U"Text"]->value).getString(), lineTextColor, lineTextStyle);
            }
        }
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
                                        [](const std::unique_ptr<DataIO::Node>& child){ return child->name == U"Line"; }), node->children.end());

        if (node->propertyValuePairs[U"LinesStartFromTop"])
            setLinesStartFromTop(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"LinesStartFromTop"]->value).getBool());

        // This has to be parsed after the lines have been added
        if (node->propertyValuePairs[U"NewLinesBelowOthers"])
            setNewLinesBelowOthers(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"NewLinesBelowOthers"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChatBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const RenderStates scrollbarStates = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else
            target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached));

        // Draw the scrollbar
        m_scroll->draw(target, scrollbarStates);

        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});
        target.addClippingLayer(states, {{}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - m_scroll->getSize().x,
                                              getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}});

        states.transform.translate({Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached), -static_cast<float>(m_scroll->getValue())});

        // Put the lines at the bottom of the chat box if needed
        if (!m_linesStartFromTop && (m_fullTextHeight + Text::getExtraVerticalPadding(m_textSizeCached) < getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()))
            states.transform.translate({0, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - m_fullTextHeight - Text::getExtraVerticalPadding(m_textSizeCached)});

        for (const auto& line : m_lines)
        {
            target.drawText(states, line.text);
            states.transform.translate({0, line.text.getSize().y});
        }

        target.removeClippingLayer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ChatBox::clone() const
    {
        return std::make_shared<ChatBox>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
