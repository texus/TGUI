/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifdef TGUI_USE_CPP17
    #include <optional>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label()
    {
        m_type = "Label";

        m_renderer = aurora::makeCopied<LabelRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
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

    void Label::leftMouseReleased(Vector2f pos)
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Label::getSignal(std::string signalName)
    {
        if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            m_bordersCached.updateParentSize(getSize());
            rearrangeText();
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            m_paddingCached.updateParentSize(getSize());
            rearrangeText();
        }
        else if (property == "textstyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();
            rearrangeText();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            for (auto& line : m_lines)
                line.setColor(m_textColorCached);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
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

        if (!m_string.isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_string));
        if (m_maximumTextWidth > 0)
            node->propertyValuePairs["MaximumTextWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximumTextWidth));
        if (m_autoSize)
            node->propertyValuePairs["AutoSize"] = std::make_unique<DataIO::ValueNode>("true");
        if (m_ignoringMouseEvents)
            node->propertyValuePairs["IgnoreMouseEvents"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_ignoringMouseEvents));

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["horizontalalignment"])
        {
            std::string alignment = toLower(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["horizontalalignment"]->value).getString());
            if (alignment == "right")
                setHorizontalAlignment(Label::HorizontalAlignment::Right);
            else if (alignment == "center")
                setHorizontalAlignment(Label::HorizontalAlignment::Center);
            else if (alignment != "left")
                throw Exception{"Failed to parse HorizontalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs["verticalalignment"])
        {
            std::string alignment = toLower(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["verticalalignment"]->value).getString());
            if (alignment == "bottom")
                setVerticalAlignment(Label::VerticalAlignment::Bottom);
            else if (alignment == "center")
                setVerticalAlignment(Label::VerticalAlignment::Center);
            else if (alignment != "top")
                throw Exception{"Failed to parse VerticalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumtextwidth"])
            setMaximumTextWidth(tgui::stof(node->propertyValuePairs["maximumtextwidth"]->value));
        if (node->propertyValuePairs["autosize"])
            setAutoSize(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autosize"]->value).getBool());

        if (node->propertyValuePairs["ignoremouseevents"])
            ignoreMouseEvents(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["ignoremouseevents"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::rearrangeText()
    {
        m_lines.clear();

        if (m_fontCached == nullptr)
            return;

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize, m_textStyleCached);

        // Find the maximum width of one line
        float maxWidth;
        if (m_autoSize)
            maxWidth = std::max(0.f, m_maximumTextWidth - 2*textOffset);
        else
        {
            maxWidth = getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight() - 2*textOffset;
            if (maxWidth <= 0)
                return;
        }

        // Fit the text in the available space
        const sf::String string = Text::wordWrap(maxWidth, m_string, m_fontCached, m_textSize, m_textStyleCached & sf::Text::Bold);

        // Split the string in multiple lines
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

        const Outline outline = {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                                 m_paddingCached.getTop() + m_bordersCached.getTop(),
                                 m_paddingCached.getRight() + m_bordersCached.getRight(),
                                 m_paddingCached.getBottom() + m_bordersCached.getBottom()};

        // Update the size of the label
        if (m_autoSize)
        {
            Widget::setSize({std::max(width, maxWidth) + outline.getLeft() + outline.getRight() + 2*textOffset,
                            (std::max<std::size_t>(m_lines.size(), 1) * m_fontCached.getLineSpacing(m_textSize))
                             + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize, m_textStyleCached)
                             + Text::getExtraVerticalPadding(m_textSize) + outline.getTop() + outline.getBottom()});

            m_bordersCached.updateParentSize(getSize());
            m_paddingCached.updateParentSize(getSize());
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

                if (m_verticalAlignment == VerticalAlignment::Center)
                    pos.y += (totalHeight - totalTextHeight) / 2.f;
                else if (m_verticalAlignment == VerticalAlignment::Bottom)
                    pos.y += totalHeight - totalTextHeight;
            }

            if (m_horizontalAlignment == HorizontalAlignment::Left)
            {
                const float lineSpacing = m_fontCached.getLineSpacing(m_textSize);
                for (auto& line : m_lines)
                {
                    line.setPosition(pos.x, pos.y);
                    pos.y += lineSpacing;
                }
            }
            else // Center or Right alignment
            {
                const float totalWidth = getSize().x - outline.getLeft() - outline.getRight() - 2*textOffset;

                for (auto& line : m_lines)
                {
                    std::size_t lastChar = line.getString().getSize();
                    while (lastChar > 0 && isWhitespace(line.getString()[lastChar-1]))
                        lastChar--;

                    const float textWidth = line.findCharacterPos(lastChar).x;

                    if (m_horizontalAlignment == HorizontalAlignment::Center)
                        line.setPosition(pos.x + ((totalWidth - textWidth) / 2.f), pos.y);
                    else // if (m_horizontalAlignment == HorizontalAlignment::Right)
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

        Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                              getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        if (m_backgroundColorCached.isSet() && (m_backgroundColorCached != Color::Transparent))
            drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        // Apply clipping when needed
    #ifdef TGUI_USE_CPP17
        std::optional<Clipping> clipping;
    #else
        std::unique_ptr<Clipping> clipping;
    #endif
        if (!m_autoSize)
        {
            innerSize.x -= m_paddingCached.getLeft() + m_paddingCached.getRight();
            innerSize.y -= m_paddingCached.getTop() + m_paddingCached.getBottom();

        #ifdef TGUI_USE_CPP17
            clipping.emplace(target, states, Vector2f{m_paddingCached.getLeft(), m_paddingCached.getTop()}, innerSize);
        #else
            clipping = std::make_unique<Clipping>(target, states, Vector2f{m_paddingCached.getLeft(), m_paddingCached.getTop()}, innerSize);
        #endif
        }

        // Draw the text
        for (const auto& line : m_lines)
            line.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
