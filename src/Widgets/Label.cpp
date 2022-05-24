/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false}
    {
        m_draggableWidget = true;

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<LabelRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        m_scrollbar->setVisible(false);
        setTextSize(getGlobalTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Ptr Label::create(String text)
    {
        auto label = std::make_shared<Label>();

        if (!text.empty())
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

    const LabelRenderer* Label::getRenderer() const
    {
        return aurora::downcast<const LabelRenderer*>(Widget::getRenderer());
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

    void Label::setTextSize(unsigned int size)
    {
        if (size != m_textSize)
        {
            m_textSize = size;
            rearrangeText();
        }
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

    void Label::leftMousePressed(Vector2f pos)
    {
        if (m_scrollbar->isShown() && m_scrollbar->isMouseOnWidget(pos - getPosition()))
        {
            m_mouseDown = true;
            m_scrollbar->leftMousePressed(pos - getPosition());
        }
        else
            ClickableWidget::leftMousePressed(pos);
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

    bool Label::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (!m_autoSize && m_scrollbar->isShown())
        {
            m_scrollbar->mouseWheelScrolled(delta, pos - getPosition());
            return true;
        }

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
        if (property == "Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            m_bordersCached.updateParentSize(getSize());
            m_spriteBackground.setSize({getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                        getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()});
            rearrangeText();
        }
        else if (property == "Padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            m_paddingCached.updateParentSize(getSize());
            rearrangeText();
        }
        else if (property == "TextStyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();
            rearrangeText();
        }
        else if (property == "TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            for (auto& line : m_lines)
                line.setColor(m_textColorCached);
        }
        else if (property == "BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "TextOutlineThickness")
        {
            m_textOutlineThicknessCached = getSharedRenderer()->getTextOutlineThickness();
            for (auto& line : m_lines)
                line.setOutlineThickness(m_textOutlineThicknessCached);
        }
        else if (property == "TextOutlineColor")
        {
            m_textOutlineColorCached = getSharedRenderer()->getTextOutlineColor();
            for (auto& line : m_lines)
                line.setOutlineColor(m_textOutlineColorCached);
        }
        else if (property == "Scrollbar")
        {
            m_scrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (!getSharedRenderer()->getScrollbarWidth())
            {
                m_scrollbar->setSize({m_scrollbar->getDefaultWidth(), m_scrollbar->getSize().y});
                rearrangeText();
            }
        }
        else if (property == "ScrollbarWidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_scrollbar->getDefaultWidth();
            m_scrollbar->setSize({width, m_scrollbar->getSize().y});
            rearrangeText();
        }
        else if (property == "Font")
        {
            Widget::rendererChanged(property);
            rearrangeText();
        }
        else if ((property == "Opacity") || (property == "OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_scrollbar->setInheritedOpacity(m_opacityCached);

            for (auto& line : m_lines)
                line.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Label::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (m_horizontalAlignment == Label::HorizontalAlignment::Center)
            node->propertyValuePairs["HorizontalAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_horizontalAlignment == Label::HorizontalAlignment::Right)
            node->propertyValuePairs["HorizontalAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        if (m_verticalAlignment == Label::VerticalAlignment::Center)
            node->propertyValuePairs["VerticalAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_verticalAlignment == Label::VerticalAlignment::Bottom)
            node->propertyValuePairs["VerticalAlignment"] = std::make_unique<DataIO::ValueNode>("Bottom");

        if (!m_string.empty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_string));
        if (m_maximumTextWidth > 0)
            node->propertyValuePairs["MaximumTextWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximumTextWidth));
        if (m_autoSize)
            node->propertyValuePairs["AutoSize"] = std::make_unique<DataIO::ValueNode>("true");
        if (m_ignoringMouseEvents)
            node->propertyValuePairs["IgnoreMouseEvents"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_ignoringMouseEvents));

        if (m_scrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_scrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs["ScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_scrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs["ScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_textSize));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["HorizontalAlignment"])
        {
            String alignment = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["HorizontalAlignment"]->value).getString();
            if (alignment == "Right")
                setHorizontalAlignment(Label::HorizontalAlignment::Right);
            else if (alignment == "Center")
                setHorizontalAlignment(Label::HorizontalAlignment::Center);
            else if (alignment != "Left")
                throw Exception{"Failed to parse HorizontalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs["VerticalAlignment"])
        {
            String alignment = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["VerticalAlignment"]->value).getString();
            if (alignment == "Bottom")
                setVerticalAlignment(Label::VerticalAlignment::Bottom);
            else if (alignment == "Center")
                setVerticalAlignment(Label::VerticalAlignment::Center);
            else if (alignment != "Top")
                throw Exception{"Failed to parse VerticalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs["ScrollbarPolicy"])
        {
            String policy = node->propertyValuePairs["ScrollbarPolicy"]->value.trim();
            if (policy == "Automatic")
                setScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == "Always")
                setScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == "Never")
                setScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{"Failed to parse ScrollbarPolicy property, found unknown value '" + policy + "'."};
        }

        if (node->propertyValuePairs["Text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["Text"]->value).getString());
        if (node->propertyValuePairs["TextSize"])
            setTextSize(node->propertyValuePairs["TextSize"]->value.toInt());
        if (node->propertyValuePairs["MaximumTextWidth"])
            setMaximumTextWidth(node->propertyValuePairs["MaximumTextWidth"]->value.toFloat());
        if (node->propertyValuePairs["AutoSize"])
            setAutoSize(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["AutoSize"]->value).getBool());

        if (node->propertyValuePairs["IgnoreMouseEvents"])
            ignoreMouseEvents(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["IgnoreMouseEvents"]->value).getBool());
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

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize, m_textStyleCached);

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
        String string = Text::wordWrap(maxWidth, m_string, m_fontCached, m_textSize, m_textStyleCached & TextStyle::Bold);

        const Outline outline = {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                                 m_paddingCached.getTop() + m_bordersCached.getTop(),
                                 m_paddingCached.getRight() + m_bordersCached.getRight(),
                                 m_paddingCached.getBottom() + m_bordersCached.getBottom()};

        const auto lineCount = std::count(string.begin(), string.end(), U'\n') + 1;
        float requiredTextHeight = lineCount * m_fontCached.getLineSpacing(m_textSize)
                                   + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize, m_textStyleCached)
                                   + Text::getExtraVerticalPadding(m_textSize);

        // Check if a scrollbar should be added
        if (!m_autoSize)
        {
            // If the text doesn't fit in the label then we need to run the word-wrap again, but this time taking the scrollbar into account
            if ((m_scrollbarPolicy == Scrollbar::Policy::Automatic) && (requiredTextHeight > getSize().y - outline.getTop() - outline.getBottom()))
            {
                maxWidth -= m_scrollbar->getSize().x;
                if (maxWidth <= 0)
                    return;

                string = Text::wordWrap(maxWidth, m_string, m_fontCached, m_textSize, m_textStyleCached & TextStyle::Bold);

                const auto newLineCount = std::count(string.begin(), string.end(), U'\n') + 1;
                requiredTextHeight = newLineCount * m_fontCached.getLineSpacing(m_textSize)
                                     + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize, m_textStyleCached)
                                     + Text::getExtraVerticalPadding(m_textSize);
            }

            m_scrollbar->setSize(m_scrollbar->getSize().x, static_cast<unsigned int>(getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()));
            m_scrollbar->setViewportSize(static_cast<unsigned int>(getSize().y - outline.getTop() - outline.getBottom()));
            m_scrollbar->setMaximum(static_cast<unsigned int>(requiredTextHeight));
            m_scrollbar->setPosition({getSize().x - m_bordersCached.getRight() - m_scrollbar->getSize().x, m_bordersCached.getTop()});
            m_scrollbar->setScrollAmount(m_textSize);
        }

        // Split the string in multiple lines
        float width = 0;
        std::size_t searchPosStart = 0;
        std::size_t newLinePos = 0;
        while (newLinePos != String::npos)
        {
            newLinePos = string.find('\n', searchPosStart);

            TGUI_EMPLACE_BACK(line, m_lines)
            line.setCharacterSize(getTextSize());
            line.setFont(m_fontCached);
            line.setStyle(m_textStyleCached);
            line.setColor(m_textColorCached);
            line.setOpacity(m_opacityCached);
            line.setOutlineColor(m_textOutlineColorCached);
            line.setOutlineThickness(m_textOutlineThicknessCached);

            if (newLinePos != String::npos)
                line.setString(string.substr(searchPosStart, newLinePos - searchPosStart));
            else
                line.setString(string.substr(searchPosStart));

            if (line.getSize().x > width)
                width = line.getSize().x;

            searchPosStart = newLinePos + 1;
        }

        // Update the size of the label
        if (m_autoSize)
        {
            Widget::setSize({std::max(width, maxWidth) + outline.getLeft() + outline.getRight() + 2*textOffset, requiredTextHeight + outline.getTop() + outline.getBottom()});
            m_bordersCached.updateParentSize(getSize());
            m_paddingCached.updateParentSize(getSize());

            m_spriteBackground.setSize({getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                        getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()});
        }

        // Update the line positions
        {
            if ((getSize().x <= outline.getLeft() + outline.getRight()) || (getSize().y <= outline.getTop() + outline.getBottom()))
                return;

            Vector2f pos{m_paddingCached.getLeft() + textOffset, m_paddingCached.getTop()};

            if (m_verticalAlignment != VerticalAlignment::Top)
            {
                const float totalHeight = getSize().y - outline.getTop() - outline.getBottom();
                const float totalTextHeight = m_lines.size() * m_fontCached.getLineSpacing(m_textSize);

                if (!m_scrollbar->isShown() || (totalTextHeight < totalHeight))
                {
                    if (m_verticalAlignment == VerticalAlignment::Center)
                        pos.y += (totalHeight - totalTextHeight) / 2.f;
                    else if (m_verticalAlignment == VerticalAlignment::Bottom)
                        pos.y += totalHeight - totalTextHeight;
                }
            }

            const float lineSpacing = m_fontCached.getLineSpacing(m_textSize);
            if ((m_horizontalAlignment == HorizontalAlignment::Left) || (m_autoSize && (maxWidth == 0)))
            {
                for (auto& line : m_lines)
                {
                    line.setPosition(pos);
                    pos.y += lineSpacing;
                }
            }
            else // Center or Right alignment
            {
                for (auto& line : m_lines)
                {
                    std::size_t lastChar = line.getString().length();
                    while (lastChar > 0 && isWhitespace(line.getString()[lastChar-1]))
                        lastChar--;

                    const float textWidth = line.findCharacterPos(lastChar).x;

                    if (m_horizontalAlignment == HorizontalAlignment::Center)
                        line.setPosition({pos.x + ((maxWidth - textWidth) / 2.f), pos.y});
                    else // if (m_horizontalAlignment == HorizontalAlignment::Right)
                        line.setPosition({pos.x + maxWidth - textWidth, pos.y});

                    pos.y += lineSpacing;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(BackendRenderTargetBase& target, RenderStates states) const
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
                target.drawText(states, line);
        }
        else
        {
            innerSize.x -= m_paddingCached.getLeft() + m_paddingCached.getRight();
            innerSize.y -= m_paddingCached.getTop() + m_paddingCached.getBottom();

            target.addClippingLayer(states, {{m_paddingCached.getLeft(), m_paddingCached.getTop()}, innerSize});

            if (m_scrollbar->isShown())
                states.transform.translate({0, -static_cast<float>(m_scrollbar->getValue())});

            for (const auto& line : m_lines)
                target.drawText(states, line);

            target.removeClippingLayer();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
