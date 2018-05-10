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


#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Container.hpp>
#include <SFML/Graphics/CircleShape.hpp>

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton()
    {
        m_type = "RadioButton";
        m_text.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<RadioButtonRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_text) + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                 Text::getLineHeight(m_text) + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::create()
    {
        return std::make_shared<RadioButton>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::copy(RadioButton::ConstPtr radioButton)
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

    const RadioButtonRenderer* RadioButton::getRenderer() const
    {
        return aurora::downcast<const RadioButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        // If the text is auto sized then recalculate the size
        if (m_textSize == 0)
            setText(getText());

        updateTextureSizes();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getFullSize() const
    {
        if (getText().isEmpty())
            return getSize();
        else
            return {getSize().x + (getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getWidgetOffset() const
    {
        if (getText().isEmpty() || (getSize().y >= m_text.getSize().y))
            return {0, 0};
        else
            return {0, -(m_text.getSize().y - getSize().y) / 2};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setChecked(bool checked)
    {
        if (m_checked == checked)
            return;

        if (checked)
        {
            // Tell our parent that all the radio buttons should be unchecked
            if (m_parent)
                m_parent->uncheckRadioButtons();

            m_checked = true;
            onCheck.emit(this, true);
        }
        else
        {
            m_checked = false;
            onUncheck.emit(this, false);
        }

        updateTextColor();
        if (m_checked && m_textStyleCheckedCached.isSet())
            m_text.setStyle(m_textStyleCheckedCached);
        else
            m_text.setStyle(m_textStyleCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setText(const sf::String& text)
    {
        // Set the new text
        m_text.setString(text);

        // Set the text size
        if (m_textSize == 0)
            m_text.setCharacterSize(Text::findBestTextSize(m_fontCached, getSize().y * 0.8f));
        else
            m_text.setCharacterSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& RadioButton::getText() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextSize(unsigned int size)
    {
        m_textSize = size;
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int RadioButton::getTextSize() const
    {
        return m_text.getCharacterSize();
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

    bool RadioButton::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        if (m_allowTextClick && !getText().isEmpty())
        {
            // Check if the mouse is on top of the image or the small gap between image and text
            if (FloatRect{0, 0, getSize().x + getSize().x * m_textDistanceRatioCached, getSize().y}.contains(pos))
                return true;

            // Check if the mouse is on top of the text
            if (FloatRect{0, 0, m_text.getSize().x, m_text.getSize().y}.contains(pos.x - (getSize().x + (getSize().x * m_textDistanceRatioCached)),
                                                                                 pos.y - ((getSize().y - m_text.getSize().y) / 2.0f)))
                return true;
        }
        else // You are not allowed to click on the text
        {
            // Check if the mouse is on top of the image
            if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            {
                if (!m_transparentTextureCached || !m_spriteUnchecked.isSet() || !m_spriteChecked.isSet()
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

    void RadioButton::keyPressed(const sf::Event::KeyEvent& event)
    {
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            setChecked(true);
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

    Signal& RadioButton::getSignal(std::string signalName)
    {
        if (signalName == toLower(onCheck.getName()))
            return onCheck;
        else if (signalName == toLower(onUncheck.getName()))
            return onUncheck;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            m_bordersCached.updateParentSize(getSize());
            updateTextureSizes();
        }
        else if ((property == "textcolor") || (property == "textcolorhover") || (property == "textcolordisabled")
              || (property == "textcolorchecked") || (property == "textcolorcheckedhover") || (property == "textcolorcheckeddisabled"))
        {
            updateTextColor();
        }
        else if (property == "textstyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();

            if (m_checked && m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);
        }
        else if (property == "textstylechecked")
        {
            m_textStyleCheckedCached = getSharedRenderer()->getTextStyleChecked();

            if (m_checked && m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);
        }
        else if (property == "textureunchecked")
        {
            m_spriteUnchecked.setTexture(getSharedRenderer()->getTextureUnchecked());
            updateTextureSizes();
        }
        else if (property == "texturechecked")
        {
            m_spriteChecked.setTexture(getSharedRenderer()->getTextureChecked());
            updateTextureSizes();
        }
        else if (property == "textureuncheckedhover")
        {
            m_spriteUncheckedHover.setTexture(getSharedRenderer()->getTextureUncheckedHover());
        }
        else if (property == "texturecheckedhover")
        {
            m_spriteCheckedHover.setTexture(getSharedRenderer()->getTextureCheckedHover());
        }
        else if (property == "textureuncheckeddisabled")
        {
            m_spriteUncheckedDisabled.setTexture(getSharedRenderer()->getTextureUncheckedDisabled());
        }
        else if (property == "texturecheckeddisabled")
        {
            m_spriteCheckedDisabled.setTexture(getSharedRenderer()->getTextureCheckedDisabled());
        }
        else if (property == "textureuncheckedfocused")
        {
            m_spriteUncheckedFocused.setTexture(getSharedRenderer()->getTextureUncheckedFocused());
        }
        else if (property == "texturecheckedfocused")
        {
            m_spriteCheckedFocused.setTexture(getSharedRenderer()->getTextureCheckedFocused());
        }
        else if (property == "checkcolor")
        {
            m_checkColorCached = getSharedRenderer()->getCheckColor();
        }
        else if (property == "checkcolorhover")
        {
            m_checkColorHoverCached = getSharedRenderer()->getCheckColorHover();
        }
        else if (property == "checkcolordisabled")
        {
            m_checkColorDisabledCached = getSharedRenderer()->getCheckColorDisabled();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == "bordercolordisabled")
        {
            m_borderColorDisabledCached = getSharedRenderer()->getBorderColorDisabled();
        }
        else if (property == "bordercolorfocused")
        {
            m_borderColorFocusedCached = getSharedRenderer()->getBorderColorFocused();
        }
        else if (property == "bordercolorchecked")
        {
            m_borderColorCheckedCached = getSharedRenderer()->getBorderColorChecked();
        }
        else if (property == "bordercolorcheckedhover")
        {
            m_borderColorCheckedHoverCached = getSharedRenderer()->getBorderColorCheckedHover();
        }
        else if (property == "bordercolorcheckeddisabled")
        {
            m_borderColorCheckedDisabledCached = getSharedRenderer()->getBorderColorCheckedDisabled();
        }
        else if (property == "bordercolorcheckedfocused")
        {
            m_borderColorCheckedFocusedCached = getSharedRenderer()->getBorderColorCheckedFocused();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "backgroundcolorchecked")
        {
            m_backgroundColorCheckedCached = getSharedRenderer()->getBackgroundColorChecked();
        }
        else if (property == "backgroundcolorcheckedhover")
        {
            m_backgroundColorCheckedHoverCached = getSharedRenderer()->getBackgroundColorCheckedHover();
        }
        else if (property == "backgroundcolorcheckeddisabled")
        {
            m_backgroundColorCheckedDisabledCached = getSharedRenderer()->getBackgroundColorCheckedDisabled();
        }
        else if (property == "textdistanceratio")
        {
            m_textDistanceRatioCached = getSharedRenderer()->getTextDistanceRatio();
        }
        else if (property == "opacity")
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
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_text.setFont(m_fontCached);
            setText(getText());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> RadioButton::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!getText().isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));
        if (m_checked)
            node->propertyValuePairs["Checked"] = std::make_unique<DataIO::ValueNode>("true");
        if (!isTextClickable())
            node->propertyValuePairs["TextClickable"] = std::make_unique<DataIO::ValueNode>("false");

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["textclickable"])
            setTextClickable(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["textclickable"]->value).getBool());
        if (node->propertyValuePairs["checked"])
            setChecked(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["checked"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
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
        m_spriteUnchecked.setSize(getInnerSize());
        m_spriteChecked.setSize(getInnerSize());
        m_spriteUncheckedHover.setSize(getInnerSize());
        m_spriteCheckedHover.setSize(getInnerSize());
        m_spriteUncheckedDisabled.setSize(getInnerSize());
        m_spriteCheckedDisabled.setSize(getInnerSize());
        m_spriteUncheckedFocused.setSize(getInnerSize());
        m_spriteCheckedFocused.setSize(getInnerSize());
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

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        const float innerRadius = std::min(getInnerSize().x, getInnerSize().y) / 2;
        if (m_bordersCached != Borders{0})
        {
            sf::CircleShape circle{innerRadius + m_bordersCached.getLeft()};
            circle.setOutlineThickness(-m_bordersCached.getLeft());
            circle.setFillColor(Color::Transparent);
            circle.setOutlineColor(Color::calcColorOpacity(getCurrentBorderColor(), m_opacityCached));
            target.draw(circle, states);
        }

        // Draw the box
        states.transform.translate(m_bordersCached.getOffset());
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet())
        {
            if (m_checked)
            {
                if (!m_enabled && m_spriteCheckedDisabled.isSet())
                    m_spriteCheckedDisabled.draw(target, states);
                else if (m_mouseHover && m_spriteCheckedHover.isSet())
                    m_spriteCheckedHover.draw(target, states);
                else if (m_focused && m_spriteCheckedFocused.isSet())
                    m_spriteCheckedFocused.draw(target, states);
                else
                    m_spriteChecked.draw(target, states);
            }
            else
            {
                if (!m_enabled && m_spriteUncheckedDisabled.isSet())
                    m_spriteUncheckedDisabled.draw(target, states);
                else if (m_mouseHover && m_spriteUncheckedHover.isSet())
                    m_spriteUncheckedHover.draw(target, states);
                else if (m_focused && m_spriteUncheckedFocused.isSet())
                    m_spriteUncheckedFocused.draw(target, states);
                else
                    m_spriteUnchecked.draw(target, states);
            }
        }
        else // There are no images
        {
            sf::CircleShape circle{innerRadius};
            circle.setFillColor(Color::calcColorOpacity(getCurrentBackgroundColor(), m_opacityCached));
            target.draw(circle, states);

            // Draw the check if the radio button is checked
            if (m_checked)
            {
                sf::CircleShape checkShape{innerRadius * 0.4f};
                checkShape.setFillColor(Color::calcColorOpacity(getCurrentCheckColor(), m_opacityCached));
                checkShape.setPosition({innerRadius - checkShape.getRadius(), innerRadius - checkShape.getRadius()});
                target.draw(checkShape, states);
            }
        }
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getLeft()});

        if (!getText().isEmpty())
        {
            states.transform.translate({(1 + m_textDistanceRatioCached) * getSize().x, (getSize().y - m_text.getSize().y) / 2.0f});
            m_text.draw(target, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
