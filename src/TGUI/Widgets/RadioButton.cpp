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


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/RadioButton.hpp>

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton()
    {
        m_callback.widgetType = "RadioButton";
        m_type = "RadioButton";

        addSignal<int>("Checked");
        addSignal<int>("Unchecked");

        m_renderer = aurora::makeCopied<RadioButtonRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders({3});
        getRenderer()->setTextColor({60, 60, 60});
        getRenderer()->setTextColorHover(sf::Color::Black);
        getRenderer()->setBorderColor({60,  60,  60});
        getRenderer()->setBorderColorHover(sf::Color::Black);
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setBackgroundColorHover(sf::Color::White);
        getRenderer()->setCheckColor({60,  60,  60});
        getRenderer()->setCheckColorHover(sf::Color::Black);
        ///TODO: Disabled colors

        setSize({24, 24});
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
            return {getSize().x + (getSize().x * getRenderer()->getTextDistanceRatio()) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
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
            if (getRenderer()->getTextStyleChecked().isSet())
                m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyleChecked());
            else
                m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyle());

            m_callback.checked = true;
            sendSignal("Checked", m_checked);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::uncheck()
    {
        if (m_checked)
        {
            m_checked = false;

            updateTextColor();
            m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyle());

            m_callback.checked = false;
            sendSignal("Unchecked", m_checked);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setText(const sf::String& text)
    {
        // Set the new text
        m_text.setText(text);

        // Set the text size
        if (m_textSize == 0)
            m_text.setTextSize(findBestTextSize(getRenderer()->getFont(), getSize().y * 0.85f));
        else
            m_text.setTextSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& RadioButton::getText() const
    {
        return m_text.getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;
        setText(getText());
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

    bool RadioButton::mouseOnWidget(float x, float y) const
    {
        if (m_allowTextClick && !getText().isEmpty())
        {
            // Check if the mouse is on top of the image or the small gap between image and text
            if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getSize().x * getRenderer()->getTextDistanceRatio(), getSize().y}.contains(x, y))
                return true;

            // Check if the mouse is on top of the text
            if (sf::FloatRect{getPosition().x, getPosition().y, m_text.getSize().x, m_text.getSize().y}.contains(x - (getSize().x + getSize().x * getRenderer()->getTextDistanceRatio()),
                                                                                                                 y - ((getSize().y - m_text.getSize().y) / 2.0f)))
                return true;
        }
        else // You are not allowed to click on the text
        {
            // Check if the mouse is on top of the image
            if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::leftMouseReleased(float x, float y)
    {
        bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(x, y);

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
        if (getRenderer()->getTextureFocused().isLoaded())
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

    void RadioButton::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if (property == "borders")
        {
            updateTextureSizes();
        }
        else if ((property == "textcolor") || (property == "textcolorhover") || (property == "textcolordisabled")
              || (property == "textcolorchecked") || (property == "textcolorcheckedhover") || (property == "textcolorcheckeddisabled"))
        {
            updateTextColor();
        }
        else if ((property == "textstyle") || (property == "textstylechecked"))
        {
            if (m_checked && getRenderer()->getTextStyleChecked().isSet())
                m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyleChecked());
            else
                m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyle());
        }
        else if ((property == "textureunchecked") || (property == "textureuncheckedhover") || (property == "textureuncheckeddisabled")
              || (property == "texturechecked") || (property == "texturecheckedhover") || (property == "texturecheckeddisabled") || (property == "texturefocused"))
        {
            updateTextureSizes();

            value.getTexture().setOpacity(getRenderer()->getOpacity());

            if (property == "texturefocused")
                m_allowFocus = value.getTexture().isLoaded();
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();

            getRenderer()->getTextureUnchecked().setOpacity(opacity);
            getRenderer()->getTextureChecked().setOpacity(opacity);
            getRenderer()->getTextureUncheckedHover().setOpacity(opacity);
            getRenderer()->getTextureCheckedHover().setOpacity(opacity);
            getRenderer()->getTextureUncheckedDisabled().setOpacity(opacity);
            getRenderer()->getTextureCheckedDisabled().setOpacity(opacity);
            getRenderer()->getTextureFocused().setOpacity(opacity);

            m_text.getRenderer()->setOpacity(opacity);
        }
        else if (property == "font")
        {
            m_text.getRenderer()->setFont(value.getFont());
            setText(getText());
        }
        else if ((property != "checkcolor")
              && (property != "checkcolorhover")
              && (property != "checkcolordisabled")
              && (property != "backgroundcolor")
              && (property != "backgroundcolorhover")
              && (property != "backgroundcolordisabled")
              && (property != "backgroundcolorchecked")
              && (property != "backgroundcolorcheckedhover")
              && (property != "backgroundcolorcheckeddisabled")
              && (property != "bordercolor")
              && (property != "bordercolorhover")
              && (property != "bordercolordisabled")
              && (property != "bordercolorchecked")
              && (property != "bordercolorcheckedhover")
              && (property != "bordercolorcheckeddisabled")
              && (property != "textdistanceratio"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
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
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
        {
            getRenderer()->getTextureUnchecked().setSize(getInnerSize());
            getRenderer()->getTextureChecked().setSize(getInnerSize());
            getRenderer()->getTextureUncheckedHover().setSize(getInnerSize());
            getRenderer()->getTextureCheckedHover().setSize(getInnerSize());
            getRenderer()->getTextureUncheckedDisabled().setSize(getInnerSize());
            getRenderer()->getTextureCheckedDisabled().setSize(getInnerSize());
            getRenderer()->getTextureFocused().setSize(getInnerSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::updateTextColor()
    {
        if (m_checked)
        {
            if (!m_enabled && getRenderer()->getTextColorCheckedDisabled().isSet())
                m_text.getRenderer()->setTextColor(getRenderer()->getTextColorCheckedDisabled());
            else if (!m_enabled && getRenderer()->getTextColorDisabled().isSet())
                m_text.getRenderer()->setTextColor(getRenderer()->getTextColorDisabled());
            else if (m_mouseHover)
            {
                if (getRenderer()->getTextColorCheckedHover().isSet())
                    m_text.getRenderer()->setTextColor(getRenderer()->getTextColorCheckedHover());
                else if (getRenderer()->getTextColorChecked().isSet())
                    m_text.getRenderer()->setTextColor(getRenderer()->getTextColorChecked());
                else if (getRenderer()->getTextColorHover().isSet())
                    m_text.getRenderer()->setTextColor(getRenderer()->getTextColorHover());
                else
                    m_text.getRenderer()->setTextColor(getRenderer()->getTextColor());
            }
            else
            {
                if (getRenderer()->getTextColorChecked().isSet())
                    m_text.getRenderer()->setTextColor(getRenderer()->getTextColorChecked());
                else
                    m_text.getRenderer()->setTextColor(getRenderer()->getTextColor());
            }
        }
        else
        {
            if (!m_enabled && getRenderer()->getTextColorDisabled().isSet())
                m_text.getRenderer()->setTextColor(getRenderer()->getTextColorDisabled());
            else if (m_mouseHover && getRenderer()->getTextColorHover().isSet())
                m_text.getRenderer()->setTextColor(getRenderer()->getTextColorHover());
            else
                m_text.getRenderer()->setTextColor(getRenderer()->getTextColor());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        float innerRadius = std::min(getInnerSize().x, getInnerSize().y) / 2;
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            sf::CircleShape circle{innerRadius + borders.left};
            circle.setOutlineThickness(-borders.left);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(calcColorOpacity(getCurrentBorderColor(), getRenderer()->getOpacity()));
            target.draw(circle, states);
        }

        // Draw the box
        states.transform.translate({borders.left, borders.left});
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
        {
            if (m_checked)
            {
                if (!m_enabled && getRenderer()->getTextureCheckedDisabled().isLoaded())
                    getRenderer()->getTextureCheckedDisabled().draw(target, states);
                else if (m_mouseHover && getRenderer()->getTextureCheckedHover().isLoaded())
                    getRenderer()->getTextureCheckedHover().draw(target, states);
                else
                    getRenderer()->getTextureChecked().draw(target, states);
            }
            else
            {
                if (!m_enabled && getRenderer()->getTextureUncheckedDisabled().isLoaded())
                    getRenderer()->getTextureUncheckedDisabled().draw(target, states);
                else if (m_mouseHover && getRenderer()->getTextureUncheckedHover().isLoaded())
                    getRenderer()->getTextureUncheckedHover().draw(target, states);
                else
                    getRenderer()->getTextureUnchecked().draw(target, states);
            }

            // When the radio button is focused then draw an extra image
            if (m_focused && getRenderer()->getTextureFocused().isLoaded())
                getRenderer()->getTextureFocused().draw(target, states);
        }
        else // There are no images
        {
            sf::CircleShape circle{innerRadius};
            circle.setFillColor(calcColorOpacity(getCurrentBackgroundColor(), getRenderer()->getOpacity()));
            target.draw(circle, states);

            // Draw the check if the radio button is checked
            if (m_checked)
            {
                sf::CircleShape check{innerRadius * .6f};
                check.setFillColor(calcColorOpacity(getCurrentCheckColor(), getRenderer()->getOpacity()));
                check.setPosition({innerRadius - check.getRadius(), innerRadius - check.getRadius()});
                target.draw(check, states);
            }
        }
        states.transform.translate({-borders.left, -borders.left});

        if (!getText().isEmpty())
        {
            states.transform.translate({(1 + getRenderer()->getTextDistanceRatio()) * getSize().x, (getSize().y - m_text.getSize().y) / 2.0f});
            target.draw(m_text, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
