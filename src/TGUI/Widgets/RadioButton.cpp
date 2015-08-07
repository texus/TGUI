/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton()
    {
        m_callback.widgetType = "RadioButton";

        addSignal<int>("Checked");
        addSignal<int>("Unchecked");

        m_renderer = std::make_shared<RadioButtonRenderer>(this);
        reload();

        m_text.setTextColor(getRenderer()->m_textColorNormal);
        setSize({24, 24});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::copy(RadioButton::ConstPtr radioButton)
    {
        if (radioButton)
            return std::make_shared<RadioButton>(*radioButton);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setPosition(const Layout2d& position)
    {
        ClickableWidget::setPosition(position);

        getRenderer()->m_textureUnchecked.setPosition(getPosition());
        getRenderer()->m_textureChecked.setPosition(getPosition().x, getPosition().y + getSize().y - getRenderer()->m_textureChecked.getSize().y);
        getRenderer()->m_textureUncheckedHover.setPosition(getPosition());
        getRenderer()->m_textureCheckedHover.setPosition(getPosition());
        getRenderer()->m_textureFocused.setPosition(getPosition());

        m_text.setPosition(getPosition().x + getSize().x * 11.0f / 10.0f,
                           getPosition().y + ((getSize().y - m_text.getSize().y) / 2.0f));
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // If the text is auto sized then recalculate the size
        if (m_textSize == 0)
            setText(m_text.getText());

        if (getRenderer()->m_textureUnchecked.isLoaded() && getRenderer()->m_textureChecked.isLoaded())
        {
            getRenderer()->m_textureFocused.setSize(getSize());
            getRenderer()->m_textureUnchecked.setSize(getSize());
            getRenderer()->m_textureChecked.setSize(
                {getSize().x + ((getRenderer()->m_textureChecked.getImageSize().x - getRenderer()->m_textureUnchecked.getImageSize().x) * (getSize().x / getRenderer()->m_textureUnchecked.getImageSize().x)),
                 getSize().y + ((getRenderer()->m_textureChecked.getImageSize().y - getRenderer()->m_textureUnchecked.getImageSize().y) * (getSize().y / getRenderer()->m_textureUnchecked.getImageSize().y))}
            );

            getRenderer()->m_textureUncheckedHover.setSize(getSize());
            getRenderer()->m_textureCheckedHover.setSize(getRenderer()->m_textureChecked.getSize());
        }

        // Reposition the text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getFullSize() const
    {
        if (m_text.getText().isEmpty())
            return getSize();
        else
            return {(getSize().x * 11.0f / 10.0f) + m_text.getSize().x, getSize().y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::check()
    {
        if (m_checked == false)
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
            m_text.setTextSize(static_cast<unsigned int>(getSize().y * 0.75f));
        else
            m_text.setTextSize(m_textSize);

        // Reposition the text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_text.getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::allowTextClick(bool acceptTextClick)
    {
        m_allowTextClick = acceptTextClick;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        getRenderer()->m_textureUnchecked.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureChecked.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureUncheckedHover.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureCheckedHover.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureFocused.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::mouseOnWidget(float x, float y)
    {
        if (m_allowTextClick)
        {
            // Check if the mouse is on top of the image or the small gap between image and text
            if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x * 11.0f / 10.0f, getSize().y}.contains(x, y))
                return true;

            // Check if the mouse is on top of the text
            if (sf::FloatRect{getPosition().x, getPosition().y, m_text.getSize().x, m_text.getSize().y}.contains(x - (getSize().x * 11.0f / 10.0f), y - ((getSize().y - m_text.getSize().y) / 2.0f)))
                return true;
        }
        else // You are not allowed to click on the text
        {
            // Check if the mouse is on top of the image
            if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
                return true;
        }

        if (m_mouseHover == true)
            mouseLeftWidget();

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
        if (getRenderer()->m_textureFocused.isLoaded())
            Widget::widgetFocused();
        else
            unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            getRenderer()->setTextFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        if (m_theme && primary != "")
        {
            m_theme->initWidget(this, primary, secondary);

            // The widget can only be focused when there is an image available for this phase
            if (getRenderer()->m_textureFocused.isLoaded())
                m_allowFocus = true;

            if (force)
            {
                if (getRenderer()->m_textureUnchecked.isLoaded())
                    setSize(getRenderer()->m_textureUnchecked.getImageSize());
            }
        }
        else // Load white theme
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
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();

        m_text.setTextColor(getRenderer()->m_textColorHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();

        m_text.setTextColor(getRenderer()->m_textColorNormal);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);

        // Draw the text
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);
        if (property == toLower("Padding"))
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == toLower("TextColor"))
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("TextColorNormal"))
            setTextColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("TextColorHover"))
            setTextColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("BackgroundColor"))
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("BackgroundColorNormal"))
            setBackgroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("BackgroundColorHover"))
            setBackgroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ForegroundColor"))
            setForegroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ForegroundColorNormal"))
            setForegroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("ForegroundColorHover"))
            setForegroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("CheckColor"))
            setCheckColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("CheckColorNormal"))
            setCheckColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("CheckColorHover"))
            setCheckColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == toLower("UncheckedImage"))
            setUncheckedTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("CheckedImage"))
            setCheckedTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("UncheckedHoverImage"))
            setUncheckedHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("CheckedHoverImage"))
            setCheckedHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == toLower("FocusedImage"))
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
            if (property == toLower("Padding"))
                setPadding(value.getBorders());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == toLower("TextColor"))
                setTextColor(value.getColor());
            else if (property == toLower("TextColorNormal"))
                setTextColorNormal(value.getColor());
            else if (property == toLower("TextColorHover"))
                setTextColorHover(value.getColor());
            else if (property == toLower("BackgroundColor"))
                setBackgroundColor(value.getColor());
            else if (property == toLower("BackgroundColorNormal"))
                setBackgroundColorNormal(value.getColor());
            else if (property == toLower("BackgroundColorHover"))
                setBackgroundColorHover(value.getColor());
            else if (property == toLower("ForegroundColor"))
                setForegroundColor(value.getColor());
            else if (property == toLower("ForegroundColorNormal"))
                setForegroundColorNormal(value.getColor());
            else if (property == toLower("ForegroundColorHover"))
                setForegroundColorHover(value.getColor());
            else if (property == toLower("CheckColor"))
                setCheckColor(value.getColor());
            else if (property == toLower("CheckColorNormal"))
                setCheckColorNormal(value.getColor());
            else if (property == toLower("CheckColorHover"))
                setCheckColorHover(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == toLower("UncheckedImage"))
                setUncheckedTexture(value.getTexture());
            else if (property == toLower("CheckedImage"))
                setCheckedTexture(value.getTexture());
            else if (property == toLower("UncheckedHoverImage"))
                setUncheckedHoverTexture(value.getTexture());
            else if (property == toLower("CheckedHoverImage"))
                setCheckedHoverTexture(value.getTexture());
            else if (property == toLower("FocusedImage"))
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
        if (property == toLower("Padding"))
            return m_padding;
        else if (property == toLower("TextColor"))
            return m_textColorNormal;
        else if (property == toLower("TextColorNormal"))
            return m_textColorNormal;
        else if (property == toLower("TextColorHover"))
            return m_textColorHover;
        else if (property == toLower("BackgroundColor"))
            return m_backgroundColorNormal;
        else if (property == toLower("BackgroundColorNormal"))
            return m_backgroundColorNormal;
        else if (property == toLower("BackgroundColorHover"))
            return m_backgroundColorHover;
        else if (property == toLower("ForegroundColor"))
            return m_foregroundColorNormal;
        else if (property == toLower("ForegroundColorNormal"))
            return m_foregroundColorNormal;
        else if (property == toLower("ForegroundColorHover"))
            return m_foregroundColorHover;
        else if (property == toLower("CheckColor"))
            return m_checkColorNormal;
        else if (property == toLower("CheckColorNormal"))
            return m_checkColorNormal;
        else if (property == toLower("CheckColorHover"))
            return m_checkColorHover;
        else if (property == toLower("UncheckedImage"))
            return m_textureUnchecked;
        else if (property == toLower("CheckedImage"))
            return m_textureChecked;
        else if (property == toLower("UncheckedHoverImage"))
            return m_textureUncheckedHover;
        else if (property == toLower("CheckedHoverImage"))
            return m_textureCheckedHover;
        else if (property == toLower("FocusedImage"))
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
            pairs.emplace("UncheckedImage", m_textureUnchecked);
            pairs.emplace("CheckedImage", m_textureChecked);
            if (m_textureUncheckedHover.isLoaded())
                pairs.emplace("UncheckedHoverImage", m_textureUncheckedHover);
            if (m_textureCheckedHover.isLoaded())
                pairs.emplace("CheckedHoverImage", m_textureCheckedHover);
            if (m_textureFocused.isLoaded())
                pairs.emplace("FocusedImage", m_textureFocused);
        }
        else
        {
            pairs.emplace("BackgroundColorNormal", m_backgroundColorNormal);
            pairs.emplace("BackgroundColorHover", m_backgroundColorHover);
            pairs.emplace("ForegroundColorNormal", m_foregroundColorNormal);
            pairs.emplace("ForegroundColorHover", m_foregroundColorHover);
            pairs.emplace("CheckColorNormal", m_checkColorNormal);
            pairs.emplace("CheckColorHover", m_checkColorHover);
        }

        pairs.emplace("TextColorNormal", m_textColorNormal);
        pairs.emplace("TextColorHover", m_textColorHover);
        pairs.emplace("Padding", m_padding);
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_radioButton->m_font = font;
        m_radioButton->m_text.setTextFont(font);

        // Recalculate the text position and size
        m_radioButton->setText(m_radioButton->getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColor(const sf::Color& color)
    {
        m_textColorNormal = color;
        m_textColorHover = color;

        m_radioButton->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColorNormal(const sf::Color& color)
    {
        m_textColorNormal = color;

        if (!m_radioButton->m_mouseHover)
            m_radioButton->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColorHover(const sf::Color& color)
    {
        m_textColorHover = color;

        if (m_radioButton->m_mouseHover)
            m_radioButton->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColor(const sf::Color& color)
    {
        m_backgroundColorNormal = color;
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColorNormal(const sf::Color& color)
    {
        m_backgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColorHover(const sf::Color& color)
    {
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColor(const sf::Color& color)
    {
        m_foregroundColorNormal = color;
        m_foregroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColorNormal(const sf::Color& color)
    {
        m_foregroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColorHover(const sf::Color& color)
    {
        m_foregroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColor(const sf::Color& color)
    {
        m_checkColorNormal = color;
        m_checkColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColorNormal(const sf::Color& color)
    {
        m_checkColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColorHover(const sf::Color& color)
    {
        m_checkColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setUncheckedTexture(const Texture& texture)
    {
        m_textureUnchecked = texture;
        m_textureUnchecked.setPosition(m_radioButton->getPosition());
        m_textureUnchecked.setSize(m_radioButton->getSize());
        m_textureUnchecked.setColor({255, 255, 255, m_radioButton->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckedTexture(const Texture& texture)
    {
        m_textureChecked = texture;
        m_textureChecked.setPosition(m_radioButton->getPosition());
        m_textureChecked.setSize(m_radioButton->getSize());
        m_textureChecked.setColor({255, 255, 255, m_radioButton->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setUncheckedHoverTexture(const Texture& texture)
    {
        m_textureUncheckedHover = texture;
        m_textureUncheckedHover.setPosition(m_radioButton->getPosition());
        m_textureUncheckedHover.setSize(m_radioButton->getSize());
        m_textureUncheckedHover.setColor({255, 255, 255, m_radioButton->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckedHoverTexture(const Texture& texture)
    {
        m_textureCheckedHover = texture;
        m_textureCheckedHover.setPosition(m_radioButton->getPosition());
        m_textureCheckedHover.setSize(m_radioButton->getSize());
        m_textureCheckedHover.setColor({255, 255, 255, m_radioButton->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setFocusedTexture(const Texture& texture)
    {
        m_textureFocused = texture;
        m_textureFocused.setPosition(m_radioButton->getPosition());
        m_textureFocused.setSize(m_radioButton->getSize());
        m_textureFocused.setColor({255, 255, 255, m_radioButton->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
            // Draw the background (borders) if needed
            if (m_padding != Padding{0, 0, 0, 0})
            {
                sf::CircleShape background{std::min(m_radioButton->getSize().x / 2.0f, m_radioButton->getSize().y / 2.0f)};
                background.setPosition(m_radioButton->getPosition());

                if (m_radioButton->m_mouseHover)
                    background.setFillColor(m_backgroundColorHover);
                else
                    background.setFillColor(m_backgroundColorNormal);

                target.draw(background, states);
            }

            float foregroundSize = std::min(m_radioButton->getSize().x - m_padding.left - m_padding.right,
                                            m_radioButton->getSize().y - m_padding.top - m_padding.bottom);

            // Draw the foreground
            {
                sf::CircleShape foreground{foregroundSize / 2.0f};
                foreground.setPosition(m_radioButton->getPosition().x + m_padding.left, m_radioButton->getPosition().y + m_padding.top);

                if (m_radioButton->m_mouseHover)
                    foreground.setFillColor(m_foregroundColorHover);
                else
                    foreground.setFillColor(m_foregroundColorNormal);

                target.draw(foreground, states);
            }

            // Draw the check if the radio button is checked
            if (m_radioButton->m_checked)
            {
                sf::CircleShape check{foregroundSize * 7/24};
                check.setPosition(m_radioButton->getPosition().x + m_padding.left + ((foregroundSize / 2.0f) - check.getRadius()),
                                  m_radioButton->getPosition().y + m_padding.top + ((foregroundSize / 2.0f) - check.getRadius()));

                if (m_radioButton->m_mouseHover)
                    check.setFillColor(m_checkColorHover);
                else
                    check.setFillColor(m_checkColorNormal);

                target.draw(check, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> RadioButtonRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<RadioButtonRenderer>(new RadioButtonRenderer{*this});
        renderer->m_radioButton = static_cast<RadioButton*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
