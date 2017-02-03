/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Loading/Theme.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button()
    {
        m_callback.widgetType = "Button";

        addSignal<sf::String>("Pressed");

        m_renderer = std::make_shared<ButtonRenderer>(this);
        reload();

        setSize(120, 30);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::create(sf::String text)
    {
        auto button = std::make_shared<Button>();

        if (!text.isEmpty())
            button->setText(text);

        return button;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::copy(Button::ConstPtr button)
    {
        if (button)
            return std::static_pointer_cast<Button>(button->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        getRenderer()->m_textureDown.setPosition(getPosition());
        getRenderer()->m_textureHover.setPosition(getPosition());
        getRenderer()->m_textureNormal.setPosition(getPosition());
        getRenderer()->m_textureFocused.setPosition(getPosition());

        // Set the position of the text
        m_text.setPosition(getPosition().x + (getSize().x - m_text.getSize().x) * 0.5f,
                           getPosition().y + (getSize().y - m_text.getSize().y) * 0.5f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->m_textureDown.setSize(getSize());
        getRenderer()->m_textureHover.setSize(getSize());
        getRenderer()->m_textureNormal.setSize(getSize());
        getRenderer()->m_textureFocused.setSize(getSize());

        // Recalculate the text size when auto sizing
        if (m_textSize == 0)
            setText(m_string);

        // Recalculate the position of the images
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Button::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setFont(const Font& font)
    {
        Widget::setFont(font);
        m_text.setFont(font);
        setText(m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const sf::String& text)
    {
        m_string = text;
        m_callback.text = text;

        // Set the text size when the text has a fixed size
        if (m_textSize != 0)
            m_text.setTextSize(m_textSize);

        // If the height is much bigger than the width then the text should be vertical
        if (getSize().y > getSize().x * 2)
        {
            // The text is vertical
            if (!m_string.isEmpty())
            {
                m_text.setText(m_string[0]);

                for (unsigned int i = 1; i < m_string.getSize(); ++i)
                    m_text.setText(m_text.getText() + "\n" + m_string[i]);
            }

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                unsigned int textSize = findBestTextSize(getFont(), getSize().x * 0.85f);
                m_text.setTextSize(textSize);

                // Make the text smaller when it's too high
                if (m_text.getSize().y > (getSize().y * 0.85f))
                    m_text.setTextSize(static_cast<unsigned int>(textSize * getSize().y * 0.85f / m_text.getSize().y));
            }
        }
        else // The width of the button is big enough
        {
            m_text.setText(text);

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                unsigned int textSize = findBestTextSize(getFont(), getSize().y * 0.85f);
                m_text.setTextSize(textSize);

                // Make the text smaller when it's too width
                if (m_text.getSize().x > (getSize().x * 0.85f))
                    m_text.setTextSize(static_cast<unsigned int>(textSize * ((getSize().x * 0.85f) / m_text.getSize().x)));
            }
        }

        // Set the position of the text
        m_text.setPosition(getPosition().x + (getSize().x - m_text.getSize().x) * 0.5f,
                           getPosition().y + (getSize().y - m_text.getSize().y) * 0.5f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        getRenderer()->m_textureNormal.setColor({getRenderer()->m_textureNormal.getColor().r, getRenderer()->m_textureNormal.getColor().g, getRenderer()->m_textureNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureHover.setColor({getRenderer()->m_textureHover.getColor().r, getRenderer()->m_textureHover.getColor().g, getRenderer()->m_textureHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureDown.setColor({getRenderer()->m_textureDown.getColor().r, getRenderer()->m_textureDown.getColor().g, getRenderer()->m_textureDown.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureFocused.setColor({getRenderer()->m_textureFocused.getColor().r, getRenderer()->m_textureFocused.getColor().g, getRenderer()->m_textureFocused.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

        if (!m_mouseHover)
            m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColorNormal, getOpacity()));
        else if (m_mouseHover && !m_mouseDown)
            m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColorHover, getOpacity()));
        else if (m_mouseHover && m_mouseDown)
            m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColorDown, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Button::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMousePressed(float x, float y)
    {
        ClickableWidget::leftMousePressed(x, y);

        m_text.setTextColor(getRenderer()->m_textColorDown);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMouseReleased(float x, float y)
    {
        if (m_mouseDown)
            sendSignal("Pressed", m_text.getText());

        ClickableWidget::leftMouseReleased(x, y);

        if (m_mouseHover)
            m_text.setTextColor(getRenderer()->m_textColorHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::keyPressed(const sf::Event::KeyEvent& event)
    {
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            sendSignal("Pressed", m_text.getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::widgetFocused()
    {
        // We can't be focused when we don't have a focus image
        if (getRenderer()->m_textureFocused.isLoaded())
            Widget::widgetFocused();
        else
            unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setTextColorNormal({60, 60, 60});
        getRenderer()->setTextColorHover({0, 0, 0});
        getRenderer()->setTextColorDown({0, 0, 0});
        getRenderer()->setBackgroundColorNormal({245, 245, 245});
        getRenderer()->setBackgroundColorHover({255, 255, 255});
        getRenderer()->setBackgroundColorDown({255, 255, 255});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setNormalTexture({});
        getRenderer()->setHoverTexture({});
        getRenderer()->setDownTexture({});
        getRenderer()->setFocusTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);

            // The widget can only be focused when there is an image available for this phase
            if (getRenderer()->m_textureFocused.isLoaded())
                m_allowFocus = true;

            if (force)
            {
                if (getRenderer()->m_textureNormal.isLoaded())
                    setSize(getRenderer()->m_textureNormal.getImageSize());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();

        if (m_mouseDown)
            m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColorDown, getOpacity()));
        else
            m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColorHover, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();

        m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColorNormal, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background of the button
        getRenderer()->draw(target, states);

        // If the button has a text then also draw the text
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);
        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolornormal")
            setTextColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolorhover")
            setTextColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolordown")
            setTextColorDown(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolornormal")
            setBackgroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolorhover")
            setBackgroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolordown")
            setBackgroundColorDown(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "normalimage")
            setNormalTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "hoverimage")
            setHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "downimage")
            setDownTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "focusedimage")
            setFocusTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "textcolornormal")
                setTextColorNormal(value.getColor());
            else if (property == "textcolorhover")
                setTextColorHover(value.getColor());
            else if (property == "textcolordown")
                setTextColorDown(value.getColor());
            else if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "backgroundcolornormal")
                setBackgroundColorNormal(value.getColor());
            else if (property == "backgroundcolorhover")
                setBackgroundColorHover(value.getColor());
            else if (property == "backgroundcolordown")
                setBackgroundColorDown(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "normalimage")
                setNormalTexture(value.getTexture());
            else if (property == "hoverimage")
                setHoverTexture(value.getTexture());
            else if (property == "downimage")
                setDownTexture(value.getTexture());
            else if (property == "focusedimage")
                setFocusTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter ButtonRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "textcolor")
            return m_textColorNormal;
        else if (property == "textcolornormal")
            return m_textColorNormal;
        else if (property == "textcolorhover")
            return m_textColorHover;
        else if (property == "textcolordown")
            return m_textColorDown;
        else if (property == "backgroundcolor")
            return m_backgroundColorNormal;
        else if (property == "backgroundcolornormal")
            return m_backgroundColorNormal;
        else if (property == "backgroundcolorhover")
            return m_backgroundColorHover;
        else if (property == "backgroundcolordown")
            return m_backgroundColorDown;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "normalimage")
            return m_textureNormal;
        else if (property == "hoverimage")
            return m_textureHover;
        else if (property == "downimage")
            return m_textureDown;
        else if (property == "focusedimage")
            return m_textureFocused;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> ButtonRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureNormal.isLoaded())
        {
            pairs["NormalImage"] = m_textureNormal;
            if (m_textureHover.isLoaded())
                pairs["HoverImage"] = m_textureHover;
            if (m_textureDown.isLoaded())
                pairs["DownImage"] = m_textureDown;
            if (m_textureFocused.isLoaded())
                pairs["FocusedImage"] = m_textureFocused;
        }
        else
        {
            pairs["BackgroundColorNormal"] = m_backgroundColorNormal;
            pairs["BackgroundColorHover"] = m_backgroundColorHover;
            pairs["BackgroundColorDown"] = m_backgroundColorDown;
        }

        pairs["TextColorNormal"] = m_textColorNormal;
        pairs["TextColorHover"] = m_textColorHover;
        pairs["TextColorDown"] = m_textColorDown;
        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;

        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColor(const Color& color)
    {
        setTextColorNormal(color);
        setTextColorHover(color);
        setTextColorDown(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorNormal(const Color& color)
    {
        m_textColorNormal = color;

        if (!m_button->m_mouseHover)
            m_button->m_text.setTextColor(calcColorOpacity(m_textColorNormal, m_button->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorHover(const Color& color)
    {
        m_textColorHover = color;

        if (m_button->m_mouseHover && !m_button->m_mouseDown)
            m_button->m_text.setTextColor(calcColorOpacity(m_textColorHover, m_button->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorDown(const Color& color)
    {
        m_textColorDown = color;

        if (m_button->m_mouseHover && m_button->m_mouseDown)
            m_button->m_text.setTextColor(calcColorOpacity(m_textColorDown, m_button->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColor(const Color& color)
    {
        setBackgroundColorNormal(color);
        setBackgroundColorHover(color);
        setBackgroundColorDown(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColorNormal(const Color& color)
    {
        m_backgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColorHover(const Color& color)
    {
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColorDown(const Color& color)
    {
        m_backgroundColorDown = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBorderColor(const Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setNormalTexture(const Texture& texture)
    {
        m_textureNormal = texture;
        if (m_textureNormal.isLoaded())
        {
            m_textureNormal.setPosition(m_button->getPosition());
            m_textureNormal.setSize(m_button->getSize());
            m_textureNormal.setColor({m_textureNormal.getColor().r, m_textureNormal.getColor().g, m_textureNormal.getColor().b, static_cast<sf::Uint8>(m_button->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setHoverTexture(const Texture& texture)
    {
        m_textureHover = texture;
        if (m_textureHover.isLoaded())
        {
            m_textureHover.setPosition(m_button->getPosition());
            m_textureHover.setSize(m_button->getSize());
            m_textureHover.setColor({m_textureHover.getColor().r, m_textureHover.getColor().g, m_textureHover.getColor().b, static_cast<sf::Uint8>(m_button->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setDownTexture(const Texture& texture)
    {
        m_textureDown = texture;
        if (m_textureDown.isLoaded())
        {
            m_textureDown.setPosition(m_button->getPosition());
            m_textureDown.setSize(m_button->getSize());
            m_textureDown.setColor({m_textureDown.getColor().r, m_textureDown.getColor().g, m_textureDown.getColor().b, static_cast<sf::Uint8>(m_button->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setFocusTexture(const Texture& texture)
    {
        m_textureFocused = texture;
        if (m_textureFocused.isLoaded())
        {
            m_textureFocused.setPosition(m_button->getPosition());
            m_textureFocused.setSize(m_button->getSize());
            m_textureFocused.setColor({m_textureFocused.getColor().r, m_textureFocused.getColor().g, m_textureFocused.getColor().b, static_cast<sf::Uint8>(m_button->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Check if there is a background texture
        if (m_textureNormal.isLoaded())
        {
            if (m_button->m_mouseHover)
            {
                if (m_button->m_mouseDown && m_textureDown.isLoaded())
                    target.draw(m_textureDown, states);
                else if (m_textureHover.isLoaded())
                    target.draw(m_textureHover, states);
                else
                    target.draw(m_textureNormal, states);
            }
            else
                target.draw(m_textureNormal, states);

            // When the edit box is focused then draw an extra image
            if (m_button->m_focused && m_textureFocused.isLoaded())
                target.draw(m_textureFocused, states);
        }
        else // There is no background texture
        {
            sf::RectangleShape button(m_button->getSize());
            button.setPosition(m_button->getPosition());

            if (m_button->m_mouseHover)
            {
                if (m_button->m_mouseDown)
                    button.setFillColor(calcColorOpacity(m_backgroundColorDown, m_button->getOpacity()));
                else
                    button.setFillColor(calcColorOpacity(m_backgroundColorHover, m_button->getOpacity()));
            }
            else
                button.setFillColor(calcColorOpacity(m_backgroundColorNormal, m_button->getOpacity()));

            target.draw(button, states);
        }

        // Draw the borders around the button
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position = m_button->getPosition();
            sf::Vector2f size = m_button->getSize();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(calcColorOpacity(m_borderColor, m_button->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ButtonRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<ButtonRenderer>(*this);
        renderer->m_button = static_cast<Button*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
