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
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/RadioButton.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    const float textDistanceRatio = 0.25f;
}

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

        setSize({24, 24});
        getRenderer()->setBorders({3});
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
/**
    void RadioButton::setPosition(const Layout2d& position)
    {
        ClickableWidget::setPosition(position);

        getRenderer()->getTextureUnchecked().setPosition(getPosition());
        getRenderer()->getTextureChecked().setPosition(getPosition().x, getPosition().y + getSize().y - getRenderer()->getTextureChecked().getSize().y);
        getRenderer()->getTextureUncheckedHover().setPosition(getPosition());
        getRenderer()->getTextureCheckedHover().setPosition(getPosition());
        getRenderer()->getTextureFocused().setPosition(getPosition());

        m_text.setPosition(getPosition().x + getSize().x + getSize().x * textDistanceRatio,
                           getPosition().y + ((getSize().y - m_text.getSize().y) / 2.0f));
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // If the text is auto sized then recalculate the size
        if (m_textSize == 0)
            setText(getText());

        updateTextureSizes();
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///TODO: Borders around image
    sf::Vector2f RadioButton::getFullSize() const
    {
        if (getText().isEmpty())
        {
            if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
                return getRenderer()->getTextureChecked().getSize();
            else
                return getSize();
        }
        else
        {
            if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
                return {getSize().x + (getSize().x * textDistanceRatio) + m_text.getSize().x, getRenderer()->getTextureChecked().getSize().y};
            else
                return {getSize().x + (getSize().x * textDistanceRatio) + m_text.getSize().x, getSize().y};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getWidgetOffset() const
    {
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
            return {0, getRenderer()->getTextureUnchecked().getSize().y - getRenderer()->getTextureChecked().getSize().y};
        else
            return {0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setFont(const Font& font)
    {
        Widget::setFont(font);
        m_text.setFont(font.getFont());
        setText(getText());
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
            m_text.setTextSize(findBestTextSize(getFont(), getSize().y * 0.85f));
        else
            m_text.setTextSize(m_textSize);

        // Reposition the text
        updatePosition();
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

        // Call setText to reposition the text
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::allowTextClick(bool acceptTextClick)
    {
        m_allowTextClick = acceptTextClick;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    void RadioButton::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        getRenderer()->getTextureUnchecked().setColor({255, 255, 255, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->getTextureChecked().setColor({255, 255, 255, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->getTextureUncheckedHover().setColor({255, 255, 255, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->getTextureCheckedHover().setColor({255, 255, 255, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->getTextureFocused().setColor({255, 255, 255, static_cast<sf::Uint8>(m_opacity * 255)});

        if (m_mouseHover)
            m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColorHover, getOpacity()));
        else
            m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColorNormal, getOpacity()));
    }
*/
///TODO: Don't forget to pass opacity to label
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::mouseOnWidget(float x, float y) const
    {
        if (m_allowTextClick)
        {
            // Check if the mouse is on top of the image or the small gap between image and text
            if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getSize().x * textDistanceRatio, getSize().y}.contains(x, y))
                return true;

            // Check if the mouse is on top of the text
            if (sf::FloatRect{getPosition().x, getPosition().y, m_text.getSize().x, m_text.getSize().y}.contains(x - (getSize().x + getSize().x * textDistanceRatio), y - ((getSize().y - m_text.getSize().y) / 2.0f)))
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

        m_text.getRenderer()->setTextColor(getRenderer()->getTextColorHover());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();

        m_text.getRenderer()->setTextColor(getRenderer()->getTextColorNormal());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    void RadioButton::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setPadding({3, 3, 3, 3});
        getRenderer()->setTextColorNormal({60, 60, 60});
        getRenderer()->setTextColorHover({0, 0, 0});
        getRenderer()->setBackgroundColorNormal({60,  60,  60});
        getRenderer()->setBackgroundColorHover({0, 0, 0});
        getRenderer()->setForegroundColorNormal({245, 245, 245});
        getRenderer()->setForegroundColorHover({255, 255, 255});
        getRenderer()->setCheckColorNormal({60,  60,  60});
        getRenderer()->setCheckColorHover({0, 0, 0});
        getRenderer()->setUncheckedTexture({});
        getRenderer()->setCheckedTexture({});
        getRenderer()->setUncheckedHoverTexture({});
        getRenderer()->setCheckedHoverTexture({});
        getRenderer()->setFocusedTexture({});

        if (m_theme && primary != "")
        {
            Widget::reload(primary, secondary, force);

            // The widget can only be focused when there is an image available for this phase
            if (getRenderer()->getTextureFocused().isLoaded())
                m_allowFocus = true;

            if (force)
            {
                if (getRenderer()->getTextureUnchecked().isLoaded())
                    setSize(getRenderer()->getTextureUnchecked().getImageSize());
            }
        }
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
/**
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
        {
            if (m_checked)
            {
                if (m_mouseHover && getRenderer()->getTextureCheckedHover().isLoaded())
                    getRenderer()->getTextureCheckedHover()->draw(target, states);
                else
                    getRenderer()->getTextureChecked()->draw(target, states);
            }
            else
            {
                if (m_mouseHover && getRenderer()->getTextureUncheckedHover().isLoaded())
                    getRenderer()->getTextureUncheckedHover()->draw(target, states);
                else
                    getRenderer()->getTextureUnchecked()->draw(target, states);
            }

            // When the radio button is focused then draw an extra image
            if (m_focused && getRenderer()->getTextureFocused().isLoaded())
                getRenderer()->getTextureFocused().draw(target, states);
        }
        else // There are no images
        {
            float foregroundSize = std::min(getSize().x - m_padding.left - m_padding.right,
                                            getSize().y - m_padding.top - m_padding.bottom);

            sf::CircleShape circle{std::min(getSize().x / 2.0f, getSize().y / 2.0f)};
            circle.setPosition(getPosition());
            circle.setOutlineThickness(-m_padding.left);

            if (m_mouseHover)
            {
                circle.setFillColor(calcColorOpacity(m_foregroundColorHover, getOpacity()));
                circle.setOutlineColor(calcColorOpacity(m_backgroundColorHover, getOpacity()));
            }
            else
            {
                circle.setFillColor(calcColorOpacity(m_foregroundColorNormal, getRenderer()->getOpacity()));
                circle.setOutlineColor(calcColorOpacity(m_backgroundColorNormal, getRenderer()->getOpacity()));
            }

            target.draw(circle, states);

            // Draw the check if the radio button is checked
            if (m_checked)
            {
                sf::CircleShape check{foregroundSize * 7/24};
                check.setPosition(getPosition().x + m_padding.left + ((foregroundSize / 2.0f) - check.getRadius()),
                                  getPosition().y + m_padding.top + ((foregroundSize / 2.0f) - check.getRadius()));

                if (m_mouseHover)
                    check.setFillColor(calcColorOpacity(m_checkColorHover, getRenderer()->getOpacity()));
                else
                    check.setFillColor(calcColorOpacity(m_checkColorNormal, getRenderer()->getOpacity()));

                target.draw(check, states);
            }
        }
*/

        if (!getText().isEmpty())
        {
            states.transform.translate({getSize().x + getSize().x * textDistanceRatio, (getSize().y - m_text.getSize().y) / 2.0f});
            target.draw(m_text, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::updateTextureSizes()
    {
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
        {
            getRenderer()->getTextureFocused().setSize(getSize());
            getRenderer()->getTextureUnchecked().setSize(getSize());
            getRenderer()->getTextureChecked().setSize(
                {getSize().x + ((getRenderer()->getTextureChecked().getImageSize().x - getRenderer()->getTextureUnchecked().getImageSize().x) * (getSize().x / getRenderer()->getTextureUnchecked().getImageSize().x)),
                 getSize().y + ((getRenderer()->getTextureChecked().getImageSize().y - getRenderer()->getTextureUnchecked().getImageSize().y) * (getSize().y / getRenderer()->getTextureUnchecked().getImageSize().y))}
            );

            getRenderer()->getTextureUncheckedHover().setSize(getSize());
            getRenderer()->getTextureCheckedHover().setSize(getRenderer()->getTextureChecked().getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    void RadioButtonRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);
        if (property == "padding")
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolornormal")
            setTextColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolorhover")
            setTextColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolornormal")
            setBackgroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolorhover")
            setBackgroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "foregroundcolor")
            setForegroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "foregroundcolornormal")
            setForegroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "foregroundcolorhover")
            setForegroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "checkcolor")
            setCheckColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "checkcolornormal")
            setCheckColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "checkcolorhover")
            setCheckColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "uncheckedimage")
            setUncheckedTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "checkedimage")
            setCheckedTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "uncheckedhoverimage")
            setUncheckedHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "checkedhoverimage")
            setCheckedHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "focusedimage")
            setFocusedTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "padding")
                setPadding(value.getBorders());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "textcolornormal")
                setTextColorNormal(value.getColor());
            else if (property == "textcolorhover")
                setTextColorHover(value.getColor());
            else if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "backgroundcolornormal")
                setBackgroundColorNormal(value.getColor());
            else if (property == "backgroundcolorhover")
                setBackgroundColorHover(value.getColor());
            else if (property == "foregroundcolor")
                setForegroundColor(value.getColor());
            else if (property == "foregroundcolornormal")
                setForegroundColorNormal(value.getColor());
            else if (property == "foregroundcolorhover")
                setForegroundColorHover(value.getColor());
            else if (property == "checkcolor")
                setCheckColor(value.getColor());
            else if (property == "checkcolornormal")
                setCheckColorNormal(value.getColor());
            else if (property == "checkcolorhover")
                setCheckColorHover(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "uncheckedimage")
                setUncheckedTexture(value.getTexture());
            else if (property == "checkedimage")
                setCheckedTexture(value.getTexture());
            else if (property == "uncheckedhoverimage")
                setUncheckedHoverTexture(value.getTexture());
            else if (property == "checkedhoverimage")
                setCheckedHoverTexture(value.getTexture());
            else if (property == "focusedimage")
                setFocusedTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter RadioButtonRenderer::getProperty(std::string property) const
    {
        property = toLower(property);
        if (property == "padding")
            return m_padding;
        else if (property == "textcolor")
            return m_textColorNormal;
        else if (property == "textcolornormal")
            return m_textColorNormal;
        else if (property == "textcolorhover")
            return m_textColorHover;
        else if (property == "backgroundcolor")
            return m_backgroundColorNormal;
        else if (property == "backgroundcolornormal")
            return m_backgroundColorNormal;
        else if (property == "backgroundcolorhover")
            return m_backgroundColorHover;
        else if (property == "foregroundcolor")
            return m_foregroundColorNormal;
        else if (property == "foregroundcolornormal")
            return m_foregroundColorNormal;
        else if (property == "foregroundcolorhover")
            return m_foregroundColorHover;
        else if (property == "checkcolor")
            return m_checkColorNormal;
        else if (property == "checkcolornormal")
            return m_checkColorNormal;
        else if (property == "checkcolorhover")
            return m_checkColorHover;
        else if (property == "uncheckedimage")
            return m_textureUnchecked;
        else if (property == "checkedimage")
            return m_textureChecked;
        else if (property == "uncheckedhoverimage")
            return m_textureUncheckedHover;
        else if (property == "checkedhoverimage")
            return m_textureCheckedHover;
        else if (property == "focusedimage")
            return m_textureFocused;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> RadioButtonRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureUnchecked.isLoaded() && m_textureChecked.isLoaded())
        {
            pairs["UncheckedImage"] = m_textureUnchecked;
            pairs["CheckedImage"] = m_textureChecked;
            if (m_textureUncheckedHover.isLoaded())
                pairs["UncheckedHoverImage"] = m_textureUncheckedHover;
            if (m_textureCheckedHover.isLoaded())
                pairs["CheckedHoverImage"] = m_textureCheckedHover;
            if (m_textureFocused.isLoaded())
                pairs["FocusedImage"] = m_textureFocused;
        }
        else
        {
            pairs["BackgroundColorNormal"] = m_backgroundColorNormal;
            pairs["BackgroundColorHover"] = m_backgroundColorHover;
            pairs["ForegroundColorNormal"] = m_foregroundColorNormal;
            pairs["ForegroundColorHover"] = m_foregroundColorHover;
            pairs["CheckColorNormal"] = m_checkColorNormal;
            pairs["CheckColorHover"] = m_checkColorHover;
        }

        pairs["TextColorNormal"] = m_textColorNormal;
        pairs["TextColorHover"] = m_textColorHover;
        pairs["Padding"] = m_padding;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColor(const Color& color)
    {
        setTextColorNormal(color);
        setTextColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColorNormal(const Color& color)
    {
        m_textColorNormal = color;

        if (!m_radioButton->m_mouseHover)
            m_radioButton->m_text.setTextColor(calcColorOpacity(m_textColorNormal, m_radioButton->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColorHover(const Color& color)
    {
        m_textColorHover = color;

        if (m_radioButton->m_mouseHover)
            m_radioButton->m_text.setTextColor(calcColorOpacity(m_textColorHover, m_radioButton->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColor(const Color& color)
    {
        setBackgroundColorNormal(color);
        setBackgroundColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColorNormal(const Color& color)
    {
        m_backgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColorHover(const Color& color)
    {
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColor(const Color& color)
    {
        setForegroundColorNormal(color);
        setForegroundColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColorNormal(const Color& color)
    {
        m_foregroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColorHover(const Color& color)
    {
        m_foregroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColor(const Color& color)
    {
        setCheckColorNormal(color);
        setCheckColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColorNormal(const Color& color)
    {
        m_checkColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColorHover(const Color& color)
    {
        m_checkColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setUncheckedTexture(const Texture& texture)
    {
        m_textureUnchecked = texture;
        if (m_textureUnchecked.isLoaded())
        {
            m_textureUnchecked.setColor({255, 255, 255, static_cast<sf::Uint8>(m_radioButton->getOpacity() * 255)});

            if (m_textureUnchecked.isLoaded() && m_textureChecked.isLoaded())
                m_radioButton->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckedTexture(const Texture& texture)
    {
        m_textureChecked = texture;
        if (m_textureChecked.isLoaded())
        {
            m_textureChecked.setColor({255, 255, 255, static_cast<sf::Uint8>(m_radioButton->getOpacity() * 255)});

            if (m_textureUnchecked.isLoaded() && m_textureChecked.isLoaded())
                m_radioButton->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setUncheckedHoverTexture(const Texture& texture)
    {
        m_textureUncheckedHover = texture;
        if (m_textureUncheckedHover.isLoaded())
        {
            m_textureUncheckedHover.setColor({255, 255, 255, static_cast<sf::Uint8>(m_radioButton->getOpacity() * 255)});

            if (m_textureUnchecked.isLoaded() && m_textureChecked.isLoaded())
                m_radioButton->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckedHoverTexture(const Texture& texture)
    {
        m_textureCheckedHover = texture;
        if (m_textureCheckedHover.isLoaded())
        {
            m_textureCheckedHover.setColor({255, 255, 255, static_cast<sf::Uint8>(m_radioButton->getOpacity() * 255)});

            if (m_textureUnchecked.isLoaded() && m_textureChecked.isLoaded())
                m_radioButton->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setFocusedTexture(const Texture& texture)
    {
        m_textureFocused = texture;
        if (m_textureFocused.isLoaded())
        {
            m_textureFocused.setSize(m_radioButton->getSize());
            m_textureFocused.setPosition(m_radioButton->getPosition());
            m_textureFocused.setColor({255, 255, 255, static_cast<sf::Uint8>(m_radioButton->getOpacity() * 255)});
        }
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    void RadioButtonRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_textureUnchecked.isLoaded() && m_textureChecked.isLoaded())
        {
            if (m_radioButton->m_checked)
            {
                if (m_radioButton->m_mouseHover && m_textureCheckedHover.isLoaded())
                    target.draw(m_textureCheckedHover, states);
                else
                    target.draw(m_textureChecked, states);
            }
            else
            {
                if (m_radioButton->m_mouseHover && m_textureUncheckedHover.isLoaded())
                    target.draw(m_textureUncheckedHover, states);
                else
                    target.draw(m_textureUnchecked, states);
            }

            // When the radio button is focused then draw an extra image
            if (m_radioButton->m_focused && m_textureFocused.isLoaded())
                target.draw(m_textureFocused, states);
        }
        else // There are no images
        {
            float foregroundSize = std::min(m_radioButton->getSize().x - m_padding.left - m_padding.right,
                                            m_radioButton->getSize().y - m_padding.top - m_padding.bottom);

            sf::CircleShape circle{std::min(m_radioButton->getSize().x / 2.0f, m_radioButton->getSize().y / 2.0f)};
            circle.setPosition(m_radioButton->getPosition());
            circle.setOutlineThickness(-m_padding.left);

            if (m_radioButton->m_mouseHover)
            {
                circle.setFillColor(calcColorOpacity(m_foregroundColorHover, m_radioButton->getOpacity()));
                circle.setOutlineColor(calcColorOpacity(m_backgroundColorHover, m_radioButton->getOpacity()));
            }
            else
            {
                circle.setFillColor(calcColorOpacity(m_foregroundColorNormal, m_radioButton->getOpacity()));
                circle.setOutlineColor(calcColorOpacity(m_backgroundColorNormal, m_radioButton->getOpacity()));
            }

            target.draw(circle, states);

            // Draw the check if the radio button is checked
            if (m_radioButton->m_checked)
            {
                sf::CircleShape check{foregroundSize * 7/24};
                check.setPosition(m_radioButton->getPosition().x + m_padding.left + ((foregroundSize / 2.0f) - check.getRadius()),
                                  m_radioButton->getPosition().y + m_padding.top + ((foregroundSize / 2.0f) - check.getRadius()));

                if (m_radioButton->m_mouseHover)
                    check.setFillColor(calcColorOpacity(m_checkColorHover, m_radioButton->getOpacity()));
                else
                    check.setFillColor(calcColorOpacity(m_checkColorNormal, m_radioButton->getOpacity()));

                target.draw(check, states);
            }
        }
    }
*/

    void RadioButtonRenderer::setTextColor(const Color& color)
    {
        setTextColorNormal(color);
        setTextColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    void RadioButtonRenderer::setBorderColor(const Color& color)
    {
        setBorderColorNormal(color);
        setBorderColorHover(color);
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_RENDERER_PROPERTY_OUTLINE(RadioButtonRenderer, Borders, Borders(0))

    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorNormal, Color(60, 60, 60))
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorHover, sf::Color::Black)
//    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorNormal, Color(60, 60, 60))
//    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorHover, sf::Color::Black)

//    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUnchecked)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUncheckedHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureChecked)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureCheckedHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureFocused)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
