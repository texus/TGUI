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


#include <TGUI/Widgets/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{2}},
                {"bordercolor", Color{60, 60, 60}},
                {"bordercolorhover", sf::Color::Black},
                {"bordercolordown", sf::Color::Black},
                {"textcolor", Color{60, 60, 60}},
                {"textcolorhover", sf::Color::Black},
                {"textcolordown", sf::Color::Black},
                {"backgroundcolor", Color{245, 245, 245}},
                {"backgroundcolorhover", sf::Color::White},
                {"backgroundcolordown", sf::Color::White}
                ///TODO: Define default disabled colors
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button()
    {
        m_callback.widgetType = "Button";
        m_type = "Button";

        addSignal<sf::String>("Pressed");

        m_renderer = aurora::makeCopied<ButtonRenderer>();
        setRenderer(std::make_shared<RendererData>(defaultRendererValues));

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

    void Button::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // Reset the texture sizes
        m_sprite.setSize(getInnerSize());
        m_spriteHover.setSize(getInnerSize());
        m_spriteDown.setSize(getInnerSize());
        m_spriteDisabled.setSize(getInnerSize());
        m_spriteFocused.setSize(getInnerSize());

        // Recalculate the text size when auto sizing
        if (m_textSize == 0)
            setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::enable()
    {
        Widget::enable();
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::disable(bool blockMouseEvents)
    {
        Widget::disable(blockMouseEvents);
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const sf::String& text)
    {
        m_string = text;
        m_callback.text = text;

        // Set the text size when the text has a fixed size
        if (m_textSize != 0)
            m_text.setCharacterSize(m_textSize);

        // Draw the text normally unless the height is more than double of the width
        if (getInnerSize().y <= getInnerSize().x * 2)
        {
            m_text.setString(text);

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                unsigned int textSize = Text::findBestTextSize(m_fontCached, getInnerSize().y * 0.8f);
                m_text.setCharacterSize(textSize);

                // Make the text smaller when it's too width
                if (m_text.getSize().x > (getInnerSize().x * 0.85f))
                    m_text.setCharacterSize(static_cast<unsigned int>(textSize * ((getInnerSize().x * 0.85f) / m_text.getSize().x)));
            }
        }
        else // Place the text vertically
        {
            // The text is vertical
            if (!m_string.isEmpty())
            {
                m_text.setString(m_string[0]);

                for (unsigned int i = 1; i < m_string.getSize(); ++i)
                    m_text.setString(m_text.getString() + "\n" + m_string[i]);
            }

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                unsigned int textSize = Text::findBestTextSize(m_fontCached, getInnerSize().x * 0.8f);
                m_text.setCharacterSize(textSize);

                // Make the text smaller when it's too high
                if (m_text.getSize().y > (getInnerSize().y * 0.85f))
                    m_text.setCharacterSize(static_cast<unsigned int>(textSize * getInnerSize().y * 0.85f / m_text.getSize().y));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Button::getText() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextSize(unsigned int size)
    {
        if (size != m_textSize)
        {
            m_textSize = size;

            // Call setText to reposition the text
            setText(getText());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Button::getTextSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMousePressed(sf::Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMouseReleased(sf::Vector2f pos)
    {
        if (m_mouseDown)
            sendSignal("Pressed", m_text.getString());

        ClickableWidget::leftMouseReleased(pos);

        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::keyPressed(const sf::Event::KeyEvent& event)
    {
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            sendSignal("Pressed", m_text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::widgetFocused()
    {
        // We can't be focused when we don't have a focus image
        if (m_spriteFocused.getTexture().getData())
            Widget::widgetFocused();
        else
            unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
            updateSize();
        }
        else if ((property == "textcolor") || (property == "textcolorhover") || (property == "textcolordown") || (property == "textcolordisabled")
              || (property == "textstyle") || (property == "textstylehover") || (property == "textstyledown") || (property == "textstyledisabled"))
        {
            updateTextColorAndStyle();
        }
        else if (property == "texture")
        {
            m_sprite.setTexture(getRenderer()->getTexture());
        }
        else if (property == "texturehover")
        {
            m_spriteHover.setTexture(getRenderer()->getTextureHover());
        }
        else if (property == "texturedown")
        {
            m_spriteDown.setTexture(getRenderer()->getTextureDown());
        }
        else if (property == "texturedisabled")
        {
            m_spriteDisabled.setTexture(getRenderer()->getTextureDisabled());
        }
        else if (property == "texturefocused")
        {
            m_spriteFocused.setTexture(getRenderer()->getTextureFocused());
            m_allowFocus = m_spriteFocused.isSet();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getRenderer()->getBorderColorHover();
        }
        else if (property == "bordercolordown")
        {
            m_borderColorDownCached = getRenderer()->getBorderColorDown();
        }
        else if (property == "bordercolordisabled")
        {
            m_borderColorDisabledCached = getRenderer()->getBorderColorDisabled();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getRenderer()->getBackgroundColorHover();
        }
        else if (property == "backgroundcolordown")
        {
            m_backgroundColorDownCached = getRenderer()->getBackgroundColorDown();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_sprite.setOpacity(m_opacityCached);
            m_spriteHover.setOpacity(m_opacityCached);
            m_spriteDown.setOpacity(m_opacityCached);
            m_spriteDisabled.setOpacity(m_opacityCached);
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

    sf::Vector2f Button::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.left - m_bordersCached.right, getSize().y - m_bordersCached.top - m_bordersCached.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color Button::getCurrentBackgroundColor() const
    {
        if (!m_enabled && m_backgroundColorDisabledCached.isSet())
            return m_backgroundColorDisabledCached;
        else if (m_mouseHover && m_mouseDown && m_backgroundColorDownCached.isSet())
            return m_backgroundColorDownCached;
        else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
            return m_backgroundColorHoverCached;
        else
            return m_backgroundColorCached;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color Button::getCurrentBorderColor() const
    {
        if (!m_enabled && m_borderColorDisabledCached.isSet())
            return m_borderColorDisabledCached;
        else if (m_mouseHover && m_mouseDown && m_borderColorDownCached.isSet())
            return m_borderColorDownCached;
        else if (m_mouseHover && m_borderColorHoverCached.isSet())
            return m_borderColorHoverCached;
        else
            return m_borderColorCached;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::updateTextColorAndStyle()
    {
        if (!m_enabled && getRenderer()->getTextStyleDisabled().isSet())
            m_text.setStyle(getRenderer()->getTextStyleDisabled());
        else if (m_mouseHover && m_mouseDown && getRenderer()->getTextStyleDown().isSet())
            m_text.setStyle(getRenderer()->getTextStyleDown());
        else if (m_mouseHover && getRenderer()->getTextStyleHover().isSet())
            m_text.setStyle(getRenderer()->getTextStyleHover());
        else
            m_text.setStyle(getRenderer()->getTextStyle());

        sf::Color color;
        if (!m_enabled && getRenderer()->getTextColorDisabled().isSet())
            color = getRenderer()->getTextColorDisabled();
        else if (m_mouseHover && m_mouseDown && getRenderer()->getTextColorDown().isSet())
            color = getRenderer()->getTextColorDown();
        else if (m_mouseHover && getRenderer()->getTextColorHover().isSet())
            color = getRenderer()->getTextColorHover();
        else
            color = getRenderer()->getTextColor();

        m_text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), getCurrentBorderColor());
            states.transform.translate({m_bordersCached.left, m_bordersCached.top});
        }

        // Check if there is a background texture
        if (m_sprite.isSet())
        {
            if (!m_enabled && m_spriteDisabled.isSet())
                m_spriteDisabled.draw(target, states);
            else if (m_mouseHover)
            {
                if (m_mouseDown && m_spriteDown.isSet())
                    m_spriteDown.draw(target, states);
                else if (m_spriteHover.isSet())
                    m_spriteHover.draw(target, states);
                else
                    m_sprite.draw(target, states);
            }
            else
                m_sprite.draw(target, states);

            // When the edit box is focused then draw an extra image
            if (m_focused && m_spriteFocused.isSet())
                m_spriteFocused.draw(target, states);
        }
        else // There is no background texture
        {
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());
        }

        // If the button has a text then also draw the text
        states.transform.translate({(getInnerSize().x - m_text.getSize().x) / 2.f, (getInnerSize().y - m_text.getSize().y) / 2.f});
        m_text.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
