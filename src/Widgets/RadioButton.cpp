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


#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Container.hpp>

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char RadioButton::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false}
    {
        m_text.setFont(m_fontCached);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<RadioButtonRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setSize({m_text.getLineHeight() + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                     m_text.getLineHeight() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::create()
    {
        return std::make_shared<RadioButton>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::copy(const RadioButton::ConstPtr& radioButton)
    {
        if (radioButton)
            return std::static_pointer_cast<RadioButton>(radioButton->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonRenderer* RadioButton::getSharedRenderer()
    {
        return aurora::downcast<RadioButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const RadioButtonRenderer* RadioButton::getSharedRenderer() const
    {
        return aurora::downcast<const RadioButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonRenderer* RadioButton::getRenderer()
    {
        return aurora::downcast<RadioButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        updateTextSize();
        updateTextureSizes();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getFullSize() const
    {
        if (getText().empty())
            return getSize();
        else
            return {getSize().x + (getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getWidgetOffset() const
    {
        if (getText().empty() || (getSize().y >= m_text.getSize().y))
            return {0, 0};
        else
            return {0, -(m_text.getSize().y - getSize().y) / 2};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setEnabled(bool enabled)
    {
        ClickableWidget::setEnabled(enabled);
        updateTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setChecked(bool checked)
    {
        if (m_checked == checked)
            return;

        if (checked)
        {
            if (m_parent)
            {
                // Loop through all radio buttons in our parent and uncheck them
                for (auto& widget : m_parent->getWidgets())
                {
                    if (widget->getWidgetType() == U"RadioButton")
                        std::static_pointer_cast<RadioButton>(widget)->setChecked(false);
                }
            }

            m_checked = true;
            onCheck.emit(this, true);
            onChange.emit(this, true);
        }
        else
        {
            m_checked = false;
            onUncheck.emit(this, false);
            onChange.emit(this, false);
        }

        updateTextColor();
        if (m_checked && m_textStyleCheckedCached.isSet())
            m_text.setStyle(m_textStyleCheckedCached);
        else
            m_text.setStyle(m_textStyleCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setText(const String& text)
    {
        m_text.setString(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& RadioButton::getText() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextClickable(bool acceptTextClick)
    {
        m_allowTextClick = acceptTextClick;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::isTextClickable() const
    {
        return m_allowTextClick;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::updateTextSize()
    {
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            m_textSizeCached = Text::findBestTextSize(m_fontCached, getSize().y * 0.8f);

        m_text.setCharacterSize(m_textSizeCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::isMouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        if (m_allowTextClick && !getText().empty())
        {
            // Check if the mouse is on top of the image or the small gap between image and text
            if (FloatRect{0, 0, getSize().x + getSize().x * m_textDistanceRatioCached, getSize().y}.contains(pos))
                return true;

            // Check if the mouse is on top of the text
            if (FloatRect{0, 0, m_text.getSize().x, m_text.getSize().y}.contains({pos.x - (getSize().x + (getSize().x * m_textDistanceRatioCached)),
                                                                                  pos.y - ((getSize().y - m_text.getSize().y) / 2.0f)}))
                return true;
        }
        else // You are not allowed to click on the text
        {
            // Check if the mouse is on top of the image
            if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            {
                if (!m_transparentTextureCached || !m_spriteUnchecked.isSet()
                 || !m_spriteUnchecked.isTransparentPixel(pos - m_bordersCached.getOffset()))
                    return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        // Check the radio button if we clicked on the radio button (not just mouse release)
        if (mouseDown)
            setChecked(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::keyPressed(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Space) || (event.code == Event::KeyboardKey::Enter))
            setChecked(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::canHandleKeyPress(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Space) || (event.code == Event::KeyboardKey::Enter))
            return true;
        else
            return ClickableWidget::canHandleKeyPress(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();
        updateTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();
        updateTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& RadioButton::getSignal(String signalName)
    {
        if (signalName == onCheck.getName())
            return onCheck;
        else if (signalName == onUncheck.getName())
            return onUncheck;
        else if (signalName == onChange.getName())
            return onChange;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            m_bordersCached.updateParentSize(getSize());
            updateTextureSizes();
        }
        else if ((property == U"TextColor") || (property == U"TextColorHover") || (property == U"TextColorDisabled")
              || (property == U"TextColorChecked") || (property == U"TextColorCheckedHover") || (property == U"TextColorCheckedDisabled"))
        {
            updateTextColor();
        }
        else if (property == U"TextStyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();

            if (m_checked && m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);
        }
        else if (property == U"TextStyleChecked")
        {
            m_textStyleCheckedCached = getSharedRenderer()->getTextStyleChecked();

            if (m_checked && m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);
        }
        else if (property == U"TextureUnchecked")
        {
            m_spriteUnchecked.setTexture(getSharedRenderer()->getTextureUnchecked());
            updateTextureSizes();
        }
        else if (property == U"TextureChecked")
        {
            m_spriteChecked.setTexture(getSharedRenderer()->getTextureChecked());
            updateTextureSizes();
        }
        else if (property == U"TextureUncheckedHover")
        {
            m_spriteUncheckedHover.setTexture(getSharedRenderer()->getTextureUncheckedHover());
        }
        else if (property == U"TextureCheckedHover")
        {
            m_spriteCheckedHover.setTexture(getSharedRenderer()->getTextureCheckedHover());
        }
        else if (property == U"TextureUncheckedDisabled")
        {
            m_spriteUncheckedDisabled.setTexture(getSharedRenderer()->getTextureUncheckedDisabled());
        }
        else if (property == U"TextureCheckedDisabled")
        {
            m_spriteCheckedDisabled.setTexture(getSharedRenderer()->getTextureCheckedDisabled());
        }
        else if (property == U"TextureUncheckedFocused")
        {
            m_spriteUncheckedFocused.setTexture(getSharedRenderer()->getTextureUncheckedFocused());
        }
        else if (property == U"TextureCheckedFocused")
        {
            m_spriteCheckedFocused.setTexture(getSharedRenderer()->getTextureCheckedFocused());
        }
        else if (property == U"CheckColor")
        {
            m_checkColorCached = getSharedRenderer()->getCheckColor();
        }
        else if (property == U"CheckColorHover")
        {
            m_checkColorHoverCached = getSharedRenderer()->getCheckColorHover();
        }
        else if (property == U"CheckColorDisabled")
        {
            m_checkColorDisabledCached = getSharedRenderer()->getCheckColorDisabled();
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BorderColorHover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == U"BorderColorDisabled")
        {
            m_borderColorDisabledCached = getSharedRenderer()->getBorderColorDisabled();
        }
        else if (property == U"BorderColorFocused")
        {
            m_borderColorFocusedCached = getSharedRenderer()->getBorderColorFocused();
        }
        else if (property == U"BorderColorChecked")
        {
            m_borderColorCheckedCached = getSharedRenderer()->getBorderColorChecked();
        }
        else if (property == U"BorderColorCheckedHover")
        {
            m_borderColorCheckedHoverCached = getSharedRenderer()->getBorderColorCheckedHover();
        }
        else if (property == U"BorderColorCheckedDisabled")
        {
            m_borderColorCheckedDisabledCached = getSharedRenderer()->getBorderColorCheckedDisabled();
        }
        else if (property == U"BorderColorCheckedFocused")
        {
            m_borderColorCheckedFocusedCached = getSharedRenderer()->getBorderColorCheckedFocused();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"BackgroundColorHover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == U"BackgroundColorDisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == U"BackgroundColorChecked")
        {
            m_backgroundColorCheckedCached = getSharedRenderer()->getBackgroundColorChecked();
        }
        else if (property == U"BackgroundColorCheckedHover")
        {
            m_backgroundColorCheckedHoverCached = getSharedRenderer()->getBackgroundColorCheckedHover();
        }
        else if (property == U"BackgroundColorCheckedDisabled")
        {
            m_backgroundColorCheckedDisabledCached = getSharedRenderer()->getBackgroundColorCheckedDisabled();
        }
        else if (property == U"TextDistanceRatio")
        {
            m_textDistanceRatioCached = getSharedRenderer()->getTextDistanceRatio();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteUnchecked.setOpacity(m_opacityCached);
            m_spriteChecked.setOpacity(m_opacityCached);
            m_spriteUncheckedHover.setOpacity(m_opacityCached);
            m_spriteCheckedHover.setOpacity(m_opacityCached);
            m_spriteUncheckedDisabled.setOpacity(m_opacityCached);
            m_spriteCheckedDisabled.setOpacity(m_opacityCached);
            m_spriteUncheckedFocused.setOpacity(m_opacityCached);
            m_spriteCheckedFocused.setOpacity(m_opacityCached);

            m_text.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);

            m_text.setFont(m_fontCached);
            updateTextSize();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> RadioButton::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!getText().empty())
            node->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));
        if (m_checked)
            node->propertyValuePairs[U"Checked"] = std::make_unique<DataIO::ValueNode>("true");
        if (!isTextClickable())
            node->propertyValuePairs[U"TextClickable"] = std::make_unique<DataIO::ValueNode>("false");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"Text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Text"]->value).getString());
        if (node->propertyValuePairs[U"TextClickable"])
            setTextClickable(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"TextClickable"]->value).getBool());
        if (node->propertyValuePairs[U"Checked"])
            setChecked(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"Checked"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& RadioButton::getCurrentCheckColor() const
    {
        if (!m_enabled && m_checkColorDisabledCached.isSet())
            return m_checkColorDisabledCached;
        else if (m_mouseHover && m_checkColorHoverCached.isSet())
            return m_checkColorHoverCached;
        else
            return m_checkColorCached;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& RadioButton::getCurrentBackgroundColor() const
    {
        if (m_checked)
        {
            if (!m_enabled && m_backgroundColorCheckedDisabledCached.isSet())
                return m_backgroundColorCheckedDisabledCached;
            else if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                return m_backgroundColorDisabledCached;
            else if (m_mouseHover)
            {
                if (m_backgroundColorCheckedHoverCached.isSet())
                    return m_backgroundColorCheckedHoverCached;
                else if (m_backgroundColorCheckedCached.isSet())
                    return m_backgroundColorCheckedCached;
                else if (m_backgroundColorHoverCached.isSet())
                    return m_backgroundColorHoverCached;
                else
                    return m_backgroundColorCached;
            }
            else
            {
                if (m_backgroundColorCheckedCached.isSet())
                    return m_backgroundColorCheckedCached;
                else
                    return m_backgroundColorCached;
            }
        }
        else
        {
            if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                return m_backgroundColorDisabledCached;
            else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
                return m_backgroundColorHoverCached;
            else
                return m_backgroundColorCached;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& RadioButton::getCurrentBorderColor() const
    {
        if (m_checked)
        {
            if (!m_enabled && m_borderColorCheckedDisabledCached.isSet())
                return m_borderColorCheckedDisabledCached;
            else if (!m_enabled && m_borderColorDisabledCached.isSet())
                return m_borderColorDisabledCached;
            else if (m_mouseHover)
            {
                if (m_borderColorCheckedHoverCached.isSet())
                    return m_borderColorCheckedHoverCached;
                else if (m_borderColorCheckedCached.isSet())
                    return m_borderColorCheckedCached;
                else if (m_focused && m_borderColorCheckedFocusedCached.isSet())
                    return m_borderColorCheckedFocusedCached;
                else if (m_borderColorHoverCached.isSet())
                    return m_borderColorHoverCached;
                else if (m_focused && m_borderColorFocusedCached.isSet())
                    return m_borderColorFocusedCached;
                else
                    return m_borderColorCached;
            }
            else
            {
                if (m_focused && m_borderColorCheckedFocusedCached.isSet())
                    return m_borderColorCheckedFocusedCached;
                else if (m_borderColorCheckedCached.isSet())
                    return m_borderColorCheckedCached;
                else if (m_focused && m_borderColorFocusedCached.isSet())
                    return m_borderColorFocusedCached;
                else
                    return m_borderColorCached;
            }
        }
        else
        {
            if (!m_enabled && m_borderColorDisabledCached.isSet())
                return m_borderColorDisabledCached;
            else if (m_mouseHover && m_borderColorHoverCached.isSet())
                return m_borderColorHoverCached;
            else if (m_focused && m_borderColorFocusedCached.isSet())
                return m_borderColorFocusedCached;
            else
                return m_borderColorCached;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::updateTextureSizes()
    {
        const Vector2f& size = getSize();
        m_spriteUnchecked.setSize(size);
        m_spriteChecked.setSize(size);
        m_spriteUncheckedHover.setSize(size);
        m_spriteCheckedHover.setSize(size);
        m_spriteUncheckedDisabled.setSize(size);
        m_spriteCheckedDisabled.setSize(size);
        m_spriteUncheckedFocused.setSize(size);
        m_spriteCheckedFocused.setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::updateTextColor()
    {
        if (m_checked)
        {
            if (!m_enabled && getSharedRenderer()->getTextColorCheckedDisabled().isSet())
                m_text.setColor(getSharedRenderer()->getTextColorCheckedDisabled());
            else if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
                m_text.setColor(getSharedRenderer()->getTextColorDisabled());
            else if (m_mouseHover)
            {
                if (getSharedRenderer()->getTextColorCheckedHover().isSet())
                    m_text.setColor(getSharedRenderer()->getTextColorCheckedHover());
                else if (getSharedRenderer()->getTextColorChecked().isSet())
                    m_text.setColor(getSharedRenderer()->getTextColorChecked());
                else if (getSharedRenderer()->getTextColorHover().isSet())
                    m_text.setColor(getSharedRenderer()->getTextColorHover());
                else
                    m_text.setColor(getSharedRenderer()->getTextColor());
            }
            else
            {
                if (getSharedRenderer()->getTextColorChecked().isSet())
                    m_text.setColor(getSharedRenderer()->getTextColorChecked());
                else
                    m_text.setColor(getSharedRenderer()->getTextColor());
            }
        }
        else
        {
            if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
                m_text.setColor(getSharedRenderer()->getTextColorDisabled());
            else if (m_mouseHover && getSharedRenderer()->getTextColorHover().isSet())
                m_text.setColor(getSharedRenderer()->getTextColorHover());
            else
                m_text.setColor(getSharedRenderer()->getTextColor());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::draw(BackendRenderTarget& target, RenderStates states) const
    {
        if (!m_checked && m_spriteUnchecked.isSet())
        {
            if (!m_enabled && m_spriteUncheckedDisabled.isSet())
                target.drawSprite(states, m_spriteUncheckedDisabled);
            else if (m_mouseHover && m_spriteUncheckedHover.isSet())
                target.drawSprite(states, m_spriteUncheckedHover);
            else if (m_focused && m_spriteUncheckedFocused.isSet())
                target.drawSprite(states, m_spriteUncheckedFocused);
            else
                target.drawSprite(states, m_spriteUnchecked);
        }
        else if (m_checked && m_spriteChecked.isSet())
        {
            if (!m_enabled && m_spriteCheckedDisabled.isSet())
                target.drawSprite(states, m_spriteCheckedDisabled);
            else if (m_mouseHover && m_spriteCheckedHover.isSet())
                target.drawSprite(states, m_spriteCheckedHover);
            else if (m_focused && m_spriteCheckedFocused.isSet())
                target.drawSprite(states, m_spriteCheckedFocused);
            else
                target.drawSprite(states, m_spriteChecked);
        }
        else // There are no images
        {
            const float borderThickness = m_bordersCached.getLeft();
            const float innerSize = std::min(getInnerSize().x, getInnerSize().y);
            if (borderThickness != 0)
            {
                states.transform.translate({borderThickness, borderThickness});
                target.drawCircle(states, innerSize, Color::applyOpacity(getCurrentBackgroundColor(), m_opacityCached),
                    borderThickness, Color::applyOpacity(getCurrentBorderColor(), m_opacityCached));
            }
            else
                target.drawCircle(states, innerSize, Color::applyOpacity(getCurrentBackgroundColor(), m_opacityCached));

            // Draw the check if the radio button is checked
            if (m_checked)
            {
                const float checkSize = innerSize * 0.4f;
                const float checkOffset = (innerSize - checkSize) / 2.f;
                states.transform.translate({checkOffset, checkOffset});
                target.drawCircle(states, checkSize, Color::applyOpacity(getCurrentCheckColor(), m_opacityCached));
                states.transform.translate({-checkOffset, -checkOffset});
            }

            if (borderThickness != 0)
                states.transform.translate({-borderThickness, -borderThickness});
        }

        if (!getText().empty())
        {
            states.transform.translate({(1 + m_textDistanceRatioCached) * getSize().x, (getSize().y - m_text.getSize().y) / 2.0f});
            target.drawText(states, m_text);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr RadioButton::clone() const
    {
        return std::make_shared<RadioButton>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
