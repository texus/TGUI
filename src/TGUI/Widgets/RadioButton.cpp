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


#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Container.hpp>
#include <SFML/Graphics/CircleShape.hpp>

namespace tgui
{
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{3}},
                {"bordercolor", Color{60, 60, 60}},
                {"bordercolorhover", sf::Color::Black},
                {"textcolor", Color{60, 60, 60}},
                {"textcolorhover", sf::Color::Black},
                {"backgroundcolor", Color{245, 245, 245}},
                {"backgroundcolorhover", sf::Color::White},
                {"checkcolor", Color{60, 60, 60}},
                {"checkcolorhover", sf::Color::Black},
                {"textdistanceratio", 0.2f}
                ///TODO: Define default disabled colors
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton()
    {
        m_callback.widgetType = "RadioButton";
        m_type = "RadioButton";

        addSignal<int>("Checked");
        addSignal<int>("Unchecked");

        m_renderer = aurora::makeCopied<RadioButtonRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));

        setSize({24, 24});
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

    sf::Vector2f RadioButton::getFullSize() const
    {
        if (getText().isEmpty())
            return getSize();
        else
            return {getSize().x + (getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getWidgetOffset() const
    {
        if (getText().isEmpty() || (getSize().y >= m_text.getSize().y))
            return {0, 0};
        else
            return {0, -(m_text.getSize().y - getSize().y) / 2};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::check()
    {
        if (!m_checked)
        {
            // Tell our parent that all the radio buttons should be unchecked
            if (m_parent)
                m_parent->uncheckRadioButtons();

            // Check this radio button
            m_checked = true;

            updateTextColor();
            if (m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);

            m_callback.checked = true;
            sendSignal("Checked", static_cast<int>(m_checked));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::uncheck()
    {
        if (m_checked)
        {
            m_checked = false;

            updateTextColor();
            m_text.setStyle(m_textStyleCached);

            m_callback.checked = false;
            sendSignal("Unchecked", static_cast<int>(m_checked));
        }
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

    bool RadioButton::mouseOnWidget(sf::Vector2f pos) const
    {
        pos -= getPosition();

        if (m_allowTextClick && !getText().isEmpty())
        {
            // Check if the mouse is on top of the image or the small gap between image and text
            if (sf::FloatRect{0, 0, getSize().x + getSize().x * m_textDistanceRatioCached, getSize().y}.contains(pos))
                return true;

            // Check if the mouse is on top of the text
            if (sf::FloatRect{0, 0, m_text.getSize().x, m_text.getSize().y}.contains(pos.x - (getSize().x + (getSize().x * m_textDistanceRatioCached)),
                                                                                     pos.y - ((getSize().y - m_text.getSize().y) / 2.0f)))
                return true;
        }
        else // You are not allowed to click on the text
        {
            // Check if the mouse is on top of the image
            if (sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::leftMouseReleased(sf::Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        // Check the radio button if we clicked on the radio button (not just mouse release)
        if (mouseDown)
            check();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::keyPressed(const sf::Event::KeyEvent& event)
    {
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            check();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::widgetFocused()
    {
        // We can't be focused when we don't have a focus image
        if (m_spriteFocused.isSet())
            Widget::widgetFocused();
        else
            unfocus();
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

    void RadioButton::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
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
            m_textStyleCached = getRenderer()->getTextStyle();

            if (m_checked && m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);
        }
        else if (property == "textstylechecked")
        {
            m_textStyleCheckedCached = getRenderer()->getTextStyleChecked();

            if (m_checked && m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);
        }
        else if (property == "textureunchecked")
        {
            m_spriteUnchecked.setTexture(getRenderer()->getTextureUnchecked());
            updateTextureSizes();
        }
        else if (property == "texturechecked")
        {
            m_spriteChecked.setTexture(getRenderer()->getTextureChecked());
            updateTextureSizes();
        }
        else if (property == "textureuncheckedhover")
        {
            m_spriteUncheckedHover.setTexture(getRenderer()->getTextureUncheckedHover());
        }
        else if (property == "texturecheckedhover")
        {
            m_spriteCheckedHover.setTexture(getRenderer()->getTextureCheckedHover());
        }
        else if (property == "textureuncheckeddisabled")
        {
            m_spriteUncheckedDisabled.setTexture(getRenderer()->getTextureUncheckedDisabled());
        }
        else if (property == "texturecheckeddisabled")
        {
            m_spriteCheckedDisabled.setTexture(getRenderer()->getTextureCheckedDisabled());
        }
        else if (property == "texturefocused")
        {
            m_spriteFocused.setTexture(getRenderer()->getTextureFocused());
            m_allowFocus = m_spriteFocused.isSet();
        }
        else if (property == "checkcolor")
        {
            m_checkColorCached = getRenderer()->getCheckColor();
        }
        else if (property == "checkcolorhover")
        {
            m_checkColorHoverCached = getRenderer()->getCheckColorHover();
        }
        else if (property == "checkcolordisabled")
        {
            m_checkColorDisabledCached = getRenderer()->getCheckColorDisabled();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getRenderer()->getBorderColorHover();
        }
        else if (property == "bordercolordisabled")
        {
            m_borderColorDisabledCached = getRenderer()->getBorderColorDisabled();
        }
        else if (property == "bordercolorchecked")
        {
            m_borderColorCheckedCached = getRenderer()->getBorderColorChecked();
        }
        else if (property == "bordercolorcheckedhover")
        {
            m_borderColorCheckedHoverCached = getRenderer()->getBorderColorCheckedHover();
        }
        else if (property == "bordercolorcheckeddisabled")
        {
            m_borderColorCheckedDisabledCached = getRenderer()->getBorderColorCheckedDisabled();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getRenderer()->getBackgroundColorHover();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "backgroundcolorchecked")
        {
            m_backgroundColorCheckedCached = getRenderer()->getBackgroundColorChecked();
        }
        else if (property == "backgroundcolorcheckedhover")
        {
            m_backgroundColorCheckedHoverCached = getRenderer()->getBackgroundColorCheckedHover();
        }
        else if (property == "backgroundcolorcheckeddisabled")
        {
            m_backgroundColorCheckedDisabledCached = getRenderer()->getBackgroundColorCheckedDisabled();
        }
        else if (property == "textdistanceratio")
        {
            m_textDistanceRatioCached = getRenderer()->getTextDistanceRatio();
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
            m_spriteFocused.setOpacity(m_opacityCached);

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

    sf::Vector2f RadioButton::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color RadioButton::getCurrentCheckColor() const
    {
        if (!m_enabled && getRenderer()->getCheckColorDisabled().isSet())
            return getRenderer()->getCheckColorDisabled();
        else if (m_mouseHover && getRenderer()->getCheckColorHover().isSet())
            return getRenderer()->getCheckColorHover();
        else
            return getRenderer()->getCheckColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color RadioButton::getCurrentBackgroundColor() const
    {
        if (m_checked)
        {
            if (!m_enabled && getRenderer()->getBackgroundColorCheckedDisabled().isSet())
                return getRenderer()->getBackgroundColorCheckedDisabled();
            else if (!m_enabled && getRenderer()->getBackgroundColorDisabled().isSet())
                return getRenderer()->getBackgroundColorDisabled();
            else if (m_mouseHover)
            {
                if (getRenderer()->getBackgroundColorCheckedHover().isSet())
                    return getRenderer()->getBackgroundColorCheckedHover();
                else if (getRenderer()->getBackgroundColorChecked().isSet())
                    return getRenderer()->getBackgroundColorChecked();
                else if (getRenderer()->getBackgroundColorHover().isSet())
                    return getRenderer()->getBackgroundColorHover();
                else
                    return getRenderer()->getBackgroundColor();
            }
            else
            {
                if (getRenderer()->getBackgroundColorChecked().isSet())
                    return getRenderer()->getBackgroundColorChecked();
                else
                    return getRenderer()->getBackgroundColor();
            }
        }
        else
        {
            if (!m_enabled && getRenderer()->getBackgroundColorDisabled().isSet())
                return getRenderer()->getBackgroundColorDisabled();
            else if (m_mouseHover && getRenderer()->getBackgroundColorHover().isSet())
                return getRenderer()->getBackgroundColorHover();
            else
                return getRenderer()->getBackgroundColor();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color RadioButton::getCurrentBorderColor() const
    {
        if (m_checked)
        {
            if (!m_enabled && getRenderer()->getBorderColorCheckedDisabled().isSet())
                return getRenderer()->getBorderColorCheckedDisabled();
            else if (!m_enabled && getRenderer()->getBorderColorDisabled().isSet())
                return getRenderer()->getBorderColorDisabled();
            else if (m_mouseHover)
            {
                if (getRenderer()->getBorderColorCheckedHover().isSet())
                    return getRenderer()->getBorderColorCheckedHover();
                else if (getRenderer()->getBorderColorChecked().isSet())
                    return getRenderer()->getBorderColorChecked();
                else if (getRenderer()->getBorderColorHover().isSet())
                    return getRenderer()->getBorderColorHover();
                else
                    return getRenderer()->getBorderColor();
            }
            else
            {
                if (getRenderer()->getBorderColorChecked().isSet())
                    return getRenderer()->getBorderColorChecked();
                else
                    return getRenderer()->getBorderColor();
            }
        }
        else
        {
            if (!m_enabled && getRenderer()->getBorderColorDisabled().isSet())
                return getRenderer()->getBorderColorDisabled();
            else if (m_mouseHover && getRenderer()->getBorderColorHover().isSet())
                return getRenderer()->getBorderColorHover();
            else
                return getRenderer()->getBorderColor();
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
        m_spriteFocused.setSize(getInnerSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::updateTextColor()
    {
        if (m_checked)
        {
            if (!m_enabled && getRenderer()->getTextColorCheckedDisabled().isSet())
                m_text.setColor(getRenderer()->getTextColorCheckedDisabled());
            else if (!m_enabled && getRenderer()->getTextColorDisabled().isSet())
                m_text.setColor(getRenderer()->getTextColorDisabled());
            else if (m_mouseHover)
            {
                if (getRenderer()->getTextColorCheckedHover().isSet())
                    m_text.setColor(getRenderer()->getTextColorCheckedHover());
                else if (getRenderer()->getTextColorChecked().isSet())
                    m_text.setColor(getRenderer()->getTextColorChecked());
                else if (getRenderer()->getTextColorHover().isSet())
                    m_text.setColor(getRenderer()->getTextColorHover());
                else
                    m_text.setColor(getRenderer()->getTextColor());
            }
            else
            {
                if (getRenderer()->getTextColorChecked().isSet())
                    m_text.setColor(getRenderer()->getTextColorChecked());
                else
                    m_text.setColor(getRenderer()->getTextColor());
            }
        }
        else
        {
            if (!m_enabled && getRenderer()->getTextColorDisabled().isSet())
                m_text.setColor(getRenderer()->getTextColorDisabled());
            else if (m_mouseHover && getRenderer()->getTextColorHover().isSet())
                m_text.setColor(getRenderer()->getTextColorHover());
            else
                m_text.setColor(getRenderer()->getTextColor());
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
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(Color::calcColorOpacity(getCurrentBorderColor(), m_opacityCached));
            target.draw(circle, states);
        }

        // Draw the box
        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getLeft()});
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet())
        {
            if (m_checked)
            {
                if (!m_enabled && m_spriteCheckedDisabled.isSet())
                    m_spriteCheckedDisabled.draw(target, states);
                else if (m_mouseHover && m_spriteCheckedHover.isSet())
                    m_spriteCheckedHover.draw(target, states);
                else
                    m_spriteChecked.draw(target, states);
            }
            else
            {
                if (!m_enabled && m_spriteUncheckedDisabled.isSet())
                    m_spriteUncheckedDisabled.draw(target, states);
                else if (m_mouseHover && m_spriteUncheckedHover.isSet())
                    m_spriteUncheckedHover.draw(target, states);
                else
                    m_spriteUnchecked.draw(target, states);
            }

            // When the radio button is focused then draw an extra image
            if (m_focused && m_spriteFocused.isSet())
                m_spriteFocused.draw(target, states);
        }
        else // There are no images
        {
            sf::CircleShape circle{innerRadius};
            circle.setFillColor(Color::calcColorOpacity(getCurrentBackgroundColor(), m_opacityCached));
            target.draw(circle, states);

            // Draw the check if the radio button is checked
            if (m_checked)
            {
                sf::CircleShape checkShape{innerRadius * .6f};
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
