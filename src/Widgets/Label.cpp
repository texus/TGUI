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


#include <TGUI/Widgets/Label.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <numeric> // accumulate
    #include <algorithm>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char Label::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<LabelRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        m_scrollbar->setVisible(false);
        setTextSize(getGlobalTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Ptr Label::create(const String& text)
    {
        auto label = std::make_shared<Label>();

        if (!text.empty())
            label->setText(text);

        return label;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Ptr Label::copy(const Label::ConstPtr& label)
    {
        if (label)
            return std::static_pointer_cast<Label>(label->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LabelRenderer* Label::getSharedRenderer()
    {
        return aurora::downcast<LabelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const LabelRenderer* Label::getSharedRenderer() const
    {
        return aurora::downcast<const LabelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LabelRenderer* Label::getRenderer()
    {
        return aurora::downcast<LabelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize({getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()});

        // You are no longer auto-sizing
        m_autoSize = false;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setText(const String& string)
    {
        m_string = string;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& Label::getText() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::updateTextSize()
    {
        rearrangeText();
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

    void Label::setScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_scrollbarPolicy = policy;

        // The policy only has an effect when not auto-sizing
        if (!m_autoSize)
            rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy Label::getScrollbarPolicy() const
    {
        return m_scrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setScrollbarValue(unsigned int value)
    {
        m_scrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Label::getScrollbarValue() const
    {
        return m_scrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setAutoSize(bool autoSize)
    {
        if (m_autoSize == autoSize)
            return;

        m_autoSize = autoSize;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setMaximumTextWidth(float maximumWidth)
    {
        if (m_maximumTextWidth == maximumWidth)
            return;

        m_maximumTextWidth = maximumWidth;
        rearrangeText();
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

    void Label::ignoreMouseEvents(bool ignore)
    {
        m_ignoringMouseEvents = ignore;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::isIgnoringMouseEvents() const
    {
        return m_ignoringMouseEvents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setParent(Container* parent)
    {
        const bool autoSize = getAutoSize();
        Widget::setParent(parent);
        setAutoSize(autoSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::isMouseOnWidget(Vector2f pos) const
    {
        if (m_ignoringMouseEvents)
            return false;

        return ClickableWidget::isMouseOnWidget(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::leftMousePressed(Vector2f pos)
    {
        bool isDragging = false;
        if (m_scrollbar->isShown() && m_scrollbar->isMouseOnWidget(pos - getPosition()))
        {
            m_mouseDown = true;
            isDragging = m_scrollbar->leftMousePressed(pos - getPosition());
        }
        else
            ClickableWidget::leftMousePressed(pos);

        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::leftMouseReleased(Vector2f pos)
    {
        if (!m_scrollbar->isShown() || !m_scrollbar->isMouseDown())
        {
            const bool mouseDown = m_mouseDown;
            ClickableWidget::leftMouseReleased(pos);

            if (mouseDown)
            {
                // Check if you double-clicked
                if (m_possibleDoubleClick)
                {
                    m_possibleDoubleClick = false;
                    onDoubleClick.emit(this, m_string);
                }
                else // This is the first click
                {
                    m_animationTimeElapsed = {};
                    m_possibleDoubleClick = true;
                }
            }
            else // Mouse didn't go down on the label, so this isn't considered a click
                m_possibleDoubleClick = false;
        }
        else
            m_mouseDown = false;

        if (m_scrollbar->isShown())
            m_scrollbar->leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::mouseMoved(Vector2f pos)
    {
        if (m_scrollbar->isShown() && ((m_scrollbar->isMouseDown() && m_scrollbar->isMouseDownOnThumb()) || m_scrollbar->isMouseOnWidget(pos - getPosition())))
            m_scrollbar->mouseMoved(pos - getPosition());
        else
        {
            ClickableWidget::mouseMoved(pos);

            if (m_scrollbar->isShown())
                m_scrollbar->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::scrolled(float delta, Vector2f pos, bool touch)
    {
        if (!m_autoSize && m_scrollbar->isShown())
            return m_scrollbar->scrolled(delta, pos - getPosition(), touch);

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::mouseNoLongerOnWidget()
    {
        ClickableWidget::mouseNoLongerOnWidget();
        m_scrollbar->mouseNoLongerOnWidget();
        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::leftMouseButtonNoLongerDown()
    {
        ClickableWidget::leftMouseButtonNoLongerDown();
        m_scrollbar->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Label::getSignal(String signalName)
    {
        if (signalName == onDoubleClick.getName())
            return onDoubleClick;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            m_bordersCached.updateParentSize(getSize());
            m_spriteBackground.setSize({getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                        getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()});
            rearrangeText();
        }
        else if (property == U"Padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            m_paddingCached.updateParentSize(getSize());
            rearrangeText();
        }
        else if (property == U"TextStyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();
            rearrangeText();
        }
        else if (property == U"TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            for (auto& line : m_lines)
            {
                for (auto& textPiece : line)
                    textPiece.setColor(m_textColorCached);
            }
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"TextOutlineThickness")
        {
            m_textOutlineThicknessCached = getSharedRenderer()->getTextOutlineThickness();
            rearrangeText();
        }
        else if (property == U"TextOutlineColor")
        {
            m_textOutlineColorCached = getSharedRenderer()->getTextOutlineColor();
            for (auto& line : m_lines)
            {
                for (auto& textPiece : line)
                    textPiece.setOutlineColor(m_textOutlineColorCached);
            }
        }
        else if (property == U"Scrollbar")
        {
            m_scrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (getSharedRenderer()->getScrollbarWidth() == 0)
            {
                m_scrollbar->setSize({m_scrollbar->getDefaultWidth(), m_scrollbar->getSize().y});
                rearrangeText();
            }
        }
        else if (property == U"ScrollbarWidth")
        {
            const float width = (getSharedRenderer()->getScrollbarWidth() != 0) ? getSharedRenderer()->getScrollbarWidth() : m_scrollbar->getDefaultWidth();
            m_scrollbar->setSize({width, m_scrollbar->getSize().y});
            rearrangeText();
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);
            rearrangeText();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_scrollbar->setInheritedOpacity(m_opacityCached);

            for (auto& line : m_lines)
            {
                for (auto& textPiece : line)
                    textPiece.setOpacity(m_opacityCached);
            }
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Label::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (m_horizontalAlignment == Label::HorizontalAlignment::Center)
            node->propertyValuePairs[U"HorizontalAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_horizontalAlignment == Label::HorizontalAlignment::Right)
            node->propertyValuePairs[U"HorizontalAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        if (m_verticalAlignment == Label::VerticalAlignment::Center)
            node->propertyValuePairs[U"VerticalAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_verticalAlignment == Label::VerticalAlignment::Bottom)
            node->propertyValuePairs[U"VerticalAlignment"] = std::make_unique<DataIO::ValueNode>("Bottom");

        if (!m_string.empty())
            node->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_string));
        if (m_maximumTextWidth > 0)
            node->propertyValuePairs[U"MaximumTextWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximumTextWidth));
        if (m_autoSize)
            node->propertyValuePairs[U"AutoSize"] = std::make_unique<DataIO::ValueNode>("true");
        if (m_ignoringMouseEvents)
            node->propertyValuePairs[U"IgnoreMouseEvents"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_ignoringMouseEvents));

        if (m_scrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_scrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs[U"ScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_scrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs[U"ScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"HorizontalAlignment"])
        {
            String alignment = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"HorizontalAlignment"]->value).getString();
            if (alignment == U"Right")
                setHorizontalAlignment(Label::HorizontalAlignment::Right);
            else if (alignment == U"Center")
                setHorizontalAlignment(Label::HorizontalAlignment::Center);
            else if (alignment != U"Left")
                throw Exception{U"Failed to parse HorizontalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs[U"VerticalAlignment"])
        {
            String alignment = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"VerticalAlignment"]->value).getString();
            if (alignment == U"Bottom")
                setVerticalAlignment(Label::VerticalAlignment::Bottom);
            else if (alignment == U"Center")
                setVerticalAlignment(Label::VerticalAlignment::Center);
            else if (alignment != U"Top")
                throw Exception{U"Failed to parse VerticalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs[U"ScrollbarPolicy"])
        {
            String policy = node->propertyValuePairs[U"ScrollbarPolicy"]->value.trim();
            if (policy == U"Automatic")
                setScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == U"Always")
                setScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == U"Never")
                setScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{U"Failed to parse ScrollbarPolicy property, found unknown value '" + policy + U"'."};
        }

        if (node->propertyValuePairs[U"Text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Text"]->value).getString());
        if (node->propertyValuePairs[U"MaximumTextWidth"])
            setMaximumTextWidth(node->propertyValuePairs[U"MaximumTextWidth"]->value.toFloat());
        if (node->propertyValuePairs[U"AutoSize"])
            setAutoSize(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"AutoSize"]->value).getBool());

        if (node->propertyValuePairs[U"IgnoreMouseEvents"])
            ignoreMouseEvents(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"IgnoreMouseEvents"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::updateTime(Duration elapsedTime)
    {
        const bool screenRefreshRequired = Widget::updateTime(elapsedTime);

        if (m_animationTimeElapsed >= getDoubleClickTime())
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::rearrangeText()
    {
        m_lines.clear();

        if (m_fontCached == nullptr)
            return;

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached);

        // Show or hide the scrollbar
        if (m_autoSize)
            m_scrollbar->setVisible(false);
        else
        {
            if (m_scrollbarPolicy == Scrollbar::Policy::Always)
            {
                m_scrollbar->setVisible(true);
                m_scrollbar->setAutoHide(false);
            }
            else if (m_scrollbarPolicy == Scrollbar::Policy::Never)
            {
                m_scrollbar->setVisible(false);
            }
            else // Scrollbar::Policy::Automatic
            {
                m_scrollbar->setVisible(true);
                m_scrollbar->setAutoHide(true);
            }
        }

        // Find the maximum width of one line
        float maxWidth;
        if (m_autoSize)
            maxWidth = std::max(0.f, m_maximumTextWidth - 2*textOffset);
        else
        {
            maxWidth = getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight() - 2*textOffset;

            // If the scrollbar is always visible then we take it into account, otherwise we assume there is no scrollbar
            if (m_scrollbarPolicy == Scrollbar::Policy::Always)
                maxWidth -= m_scrollbar->getSize().x;

            if (maxWidth <= 0)
                return;
        }

        // Fit the text in the available space
        Optional<String> wordWrappedString = (maxWidth > 0)
            ? Text::wordWrap(maxWidth, m_string, m_fontCached, m_textSizeCached, m_textStyleCached & TextStyle::Bold)
            : Optional<String>();
        String* stringPtr = wordWrappedString.has_value() ? &wordWrappedString.value() : &m_string;

        const Outline outline = {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                                 m_paddingCached.getTop() + m_bordersCached.getTop(),
                                 m_paddingCached.getRight() + m_bordersCached.getRight(),
                                 m_paddingCached.getBottom() + m_bordersCached.getBottom()};

        const auto lineCount = std::count(stringPtr->begin(), stringPtr->end(), U'\n') + 1;
        float requiredTextHeight = (lineCount - 1) * m_fontCached.getLineSpacing(m_textSizeCached)
                                 + std::max(m_fontCached.getFontHeight(m_textSizeCached), m_fontCached.getLineSpacing(m_textSizeCached))
                                 + Text::getExtraVerticalPadding(m_textSizeCached);

        // Check if a scrollbar should be added
        if (!m_autoSize)
        {
            // If the text doesn't fit in the label then we need to run the word-wrap again, but this time taking the scrollbar into account
            if ((m_scrollbarPolicy == Scrollbar::Policy::Automatic) && (requiredTextHeight > getSize().y - outline.getTop() - outline.getBottom()))
            {
                maxWidth -= m_scrollbar->getSize().x;
                if (maxWidth <= 0)
                    return;

                wordWrappedString = Text::wordWrap(maxWidth, m_string, m_fontCached, m_textSizeCached, m_textStyleCached & TextStyle::Bold);
                stringPtr = &wordWrappedString.value();

                const auto newLineCount = std::count(stringPtr->begin(), stringPtr->end(), U'\n') + 1;
                requiredTextHeight = (newLineCount - 1) * m_fontCached.getLineSpacing(m_textSizeCached)
                                   + std::max(m_fontCached.getFontHeight(m_textSizeCached), m_fontCached.getLineSpacing(m_textSizeCached))
                                   + Text::getExtraVerticalPadding(m_textSizeCached);
            }

            m_scrollbar->setSize(m_scrollbar->getSize().x, static_cast<unsigned int>(getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()));
            m_scrollbar->setViewportSize(static_cast<unsigned int>(getSize().y - outline.getTop() - outline.getBottom()));
            m_scrollbar->setMaximum(static_cast<unsigned int>(requiredTextHeight));
            m_scrollbar->setPosition({getSize().x - m_bordersCached.getRight() - m_scrollbar->getSize().x, m_bordersCached.getTop()});
            m_scrollbar->setScrollAmount(m_textSizeCached);
        }

        // Split the string in multiple lines
        float width = 0;
        std::size_t searchPosStart = 0;
        std::size_t newLinePos = 0;
        while (newLinePos != String::npos)
        {
            newLinePos = stringPtr->find('\n', searchPosStart);

            TGUI_EMPLACE_BACK(line, m_lines)
            TGUI_EMPLACE_BACK(textPiece, line)
            textPiece.setCharacterSize(getTextSize());
            textPiece.setFont(m_fontCached);
            textPiece.setStyle(m_textStyleCached);
            textPiece.setColor(m_textColorCached);
            textPiece.setOpacity(m_opacityCached);
            textPiece.setOutlineColor(m_textOutlineColorCached);
            textPiece.setOutlineThickness(m_textOutlineThicknessCached);

            if (newLinePos != String::npos)
                textPiece.setString(stringPtr->substr(searchPosStart, newLinePos - searchPosStart));
            else
                textPiece.setString(stringPtr->substr(searchPosStart));

            if (textPiece.getSize().x > width)
                width = textPiece.getSize().x;

            searchPosStart = newLinePos + 1;
        }

        // Update the size of the label
        if (m_autoSize)
        {
            m_autoLayout = AutoLayout::Manual;
            Widget::setSize({std::max(width, maxWidth) + outline.getLeft() + outline.getRight() + 2*textOffset, requiredTextHeight + outline.getTop() + outline.getBottom()});
            m_bordersCached.updateParentSize(getSize());
            m_paddingCached.updateParentSize(getSize());

            m_spriteBackground.setSize({getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                        getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()});
        }

        updateTextPiecePositions((maxWidth > 0) ? maxWidth : width);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::updateTextPiecePositions(float maxWidth)
    {
        TGUI_ASSERT(!m_lines.empty(), "Label::updateTextPiecePositions requires that m_lines contains at least one line");

        const Outline outline = {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                                 m_paddingCached.getTop() + m_bordersCached.getTop(),
                                 m_paddingCached.getRight() + m_bordersCached.getRight(),
                                 m_paddingCached.getBottom() + m_bordersCached.getBottom()};

        if ((getSize().x <= outline.getLeft() + outline.getRight()) || (getSize().y <= outline.getTop() + outline.getBottom()))
            return;

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached);

        Vector2f pos{m_paddingCached.getLeft() + textOffset, m_paddingCached.getTop()};

        // Calculate the line spacing for each line
        std::vector<float> lineSpacings;
        lineSpacings.reserve(m_lines.size());
        const float defaultLineSpacing = m_fontCached.getLineSpacing(m_textSizeCached);
        for (auto& line : m_lines)
        {
            if (line.empty())
                lineSpacings.push_back(0);
            else if (line.size() == 1 && line[0].getCharacterSize() == m_textSizeCached)
                lineSpacings.push_back(defaultLineSpacing);
            else
            {
                unsigned int lineMaxTextSize = 0;
                for (const auto& textPiece : line)
                    lineMaxTextSize = std::max(lineMaxTextSize, textPiece.getCharacterSize());

                lineSpacings.push_back(m_fontCached.getLineSpacing(lineMaxTextSize));
            }
        }

        if (m_verticalAlignment != VerticalAlignment::Top)
        {
            const float totalHeight = getSize().y - outline.getTop() - outline.getBottom();
            float totalTextHeight = std::accumulate(lineSpacings.begin(), lineSpacings.end(), 0.f);

            unsigned int lastLineMaxTextSize = 0;
            for (const auto& textPiece : m_lines.back())
                lastLineMaxTextSize = std::max(lastLineMaxTextSize, textPiece.getCharacterSize());

            // Add some extra space below the last line, to not cut of low letters
            const float lastLineFontHeight = m_fontCached.getFontHeight(lastLineMaxTextSize);
            const float lastLineLineSpacing = m_fontCached.getLineSpacing(lastLineMaxTextSize);
            totalTextHeight += Text::getExtraVerticalPadding(m_textSizeCached);
            if (lastLineFontHeight > lastLineLineSpacing)
                totalTextHeight += (lastLineFontHeight - lastLineLineSpacing);

            if (!m_scrollbar->isShown() || (totalTextHeight < totalHeight))
            {
                if (m_verticalAlignment == VerticalAlignment::Bottom)
                    pos.y += totalHeight - totalTextHeight;
                else // if (m_verticalAlignment == VerticalAlignment::Center)
                    pos.y += (totalHeight - totalTextHeight) / 2.f;
            }
        }

        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            auto& line = m_lines[i];

            float maxHeight = 0;
            for (std::size_t j = 0; j < line.size(); ++j)
                maxHeight = std::max(maxHeight, line[j].getSize().y);

            Vector2f piecePos = pos;
            if ((m_horizontalAlignment != HorizontalAlignment::Left) && !line.empty())
            {
                // If the line ends with whitespace then remove them from the line width for aligning horizontally
                const auto& lastTextPiece = line.back();
                std::size_t charsToUse = lastTextPiece.getString().length();
                while (charsToUse > 0 && isWhitespace(lastTextPiece.getString()[charsToUse-1]))
                    charsToUse--;

                float whitespaceOffset = 0;
                if (charsToUse != lastTextPiece.getString().length())
                    whitespaceOffset = lastTextPiece.getSize().x - lastTextPiece.findCharacterPos(charsToUse).x;

                float textWidth = lastTextPiece.getSize().x - whitespaceOffset;
                for (std::size_t j = line.size() - 1; j > 0; --j)
                {
                    textWidth += line[j-1].getSize().x;

                    // Take kerning into account
                    if (!line[j-1].getString().empty() && !line[j].getString().empty())
                    {
                        const bool bold = ((line[j-1].getStyle() & TextStyle::Bold) != 0) && ((line[j].getStyle() & TextStyle::Bold) != 0);
                        const unsigned int characterSize = std::min(line[j-1].getCharacterSize(), line[j].getCharacterSize());
                        textWidth += m_fontCached.getKerning(line[j-1].getString().back(), line[j].getString().front(), characterSize, bold);
                    }
                }

                if (m_horizontalAlignment == HorizontalAlignment::Right)
                    piecePos.x = pos.x + maxWidth - textWidth;
                else // if (m_horizontalAlignment == HorizontalAlignment::Center)
                    piecePos.x = pos.x + ((maxWidth - textWidth) / 2.f);
            }

            for (std::size_t j = 0; j < line.size(); ++j)
            {
                // If pieces have a different text size then we align their bottom position.
                // This isn't ideal, we should align the baseline, but it is at least better than aligning the top.
                piecePos.y = pos.y + (maxHeight - line[j].getSize().y);

                // Take kerning into account
                if (j > 0 && !line[j-1].getString().empty() && !line[j].getString().empty())
                {
                    const bool bold = ((line[j-1].getStyle() & TextStyle::Bold) != 0) && ((line[j].getStyle() & TextStyle::Bold) != 0);
                    const unsigned int characterSize = std::min(line[j-1].getCharacterSize(), line[j].getCharacterSize());
                    piecePos.x += m_fontCached.getKerning(line[j-1].getString().back(), line[j].getString().front(), characterSize, bold);
                }

                line[j].setPosition(piecePos);
                piecePos.x += line[j].getSize().x;
            }

            pos.y += lineSpacings[i];
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const RenderStates statesForScrollbar = states;

        Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                              getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else if (m_backgroundColorCached.isSet() && (m_backgroundColorCached != Color::Transparent))
            target.drawFilledRect(states, innerSize, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));

        // Draw the scrollbar
        if (m_scrollbar->isVisible())
            m_scrollbar->draw(target, statesForScrollbar);

        // Draw the text
        if (m_autoSize)
        {
            for (const auto& line : m_lines)
            {
                for (const auto& text : line)
                    target.drawText(states, text);
            }
        }
        else
        {
            innerSize.x -= m_paddingCached.getLeft() + m_paddingCached.getRight();
            innerSize.y -= m_paddingCached.getTop() + m_paddingCached.getBottom();

            target.addClippingLayer(states, {{m_paddingCached.getLeft(), m_paddingCached.getTop()}, innerSize});

            if (m_scrollbar->isShown())
                states.transform.translate({0, -static_cast<float>(m_scrollbar->getValue())});

            for (const auto& line : m_lines)
            {
                for (const auto& text : line)
                    target.drawText(states, text);
            }

            target.removeClippingLayer();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Label::clone() const
    {
        return std::make_shared<Label>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
