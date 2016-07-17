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


#include <TGUI/Widgets/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button()
    {
        m_callback.widgetType = "Button";
        m_type = "Button";

        addSignal<sf::String>("Pressed");

        m_renderer = aurora::makeCopied<ButtonRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders(2);
        getRenderer()->setTextColor({60, 60, 60});
        getRenderer()->setTextColorHover(sf::Color::Black);
        getRenderer()->setTextColorDown(sf::Color::Black);
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setBackgroundColorHover(sf::Color::White);
        getRenderer()->setBackgroundColorDown(sf::Color::White);
        getRenderer()->setBorderColor({60, 60, 60});
        getRenderer()->setBorderColorHover(sf::Color::Black);
        getRenderer()->setBorderColorDown(sf::Color::Black);
        ///TODO: Disabled colors

        setSize(120, 30);
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
        getRenderer()->getTexture().setSize(getInnerSize());
        getRenderer()->getTextureHover().setSize(getInnerSize());
        getRenderer()->getTextureDown().setSize(getInnerSize());
        getRenderer()->getTextureFocused().setSize(getInnerSize());

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

    void Button::disable()
    {
        Widget::disable();
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const sf::String& text)
    {
        m_string = text;
        m_callback.text = text;

        // Set the text size when the text has a fixed size
        if (m_textSize != 0)
            m_text.setTextSize(m_textSize);

        // Draw the text normally unless the height is more than double of the width
        if (getInnerSize().y <= getInnerSize().x * 2)
        {
            m_text.setText(text);

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                unsigned int textSize = findBestTextSize(getRenderer()->getFont(), getInnerSize().y * 0.85f);
                m_text.setTextSize(textSize);

                // Make the text smaller when it's too width
                if (m_text.getSize().x > (getInnerSize().x * 0.85f))
                    m_text.setTextSize(static_cast<unsigned int>(textSize * ((getInnerSize().x * 0.85f) / m_text.getSize().x)));
            }
        }
        else // Place the text vertically
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
                unsigned int textSize = findBestTextSize(getRenderer()->getFont(), getInnerSize().x * 0.85f);
                m_text.setTextSize(textSize);

                // Make the text smaller when it's too high
                if (m_text.getSize().y > (getInnerSize().y * 0.85f))
                    m_text.setTextSize(static_cast<unsigned int>(textSize * getInnerSize().y * 0.85f / m_text.getSize().y));
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
        return m_text.getTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMousePressed(float x, float y)
    {
        ClickableWidget::leftMousePressed(x, y);

        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMouseReleased(float x, float y)
    {
        if (m_mouseDown)
            sendSignal("Pressed", m_text.getText());

        ClickableWidget::leftMouseReleased(x, y);

        updateTextColorAndStyle();
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
        if (getRenderer()->getTextureFocused().isLoaded())
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

    void Button::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if (property == "borders")
        {
            updateSize();
        }
        else if ((property == "textcolor") || (property == "textcolorhover") || (property == "textcolordown") || (property == "textcolordisabled")
              || (property == "textstyle") || (property == "textstylehover") || (property == "textstyledown") || (property == "textstyledisabled"))
        {
            updateTextColorAndStyle();
        }
        else if ((property == "texture") || (property == "texturehover") || (property == "texturedown") || (property == "texturedisabled") || (property == "texturefocused"))
        {
            value.getTexture().setSize(getInnerSize());

            if (property == "texturefocused")
                m_allowFocus = value.getTexture().isLoaded();
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();

            getRenderer()->getTexture().setOpacity(opacity);
            getRenderer()->getTextureHover().setOpacity(opacity);
            getRenderer()->getTextureDown().setOpacity(opacity);
            getRenderer()->getTextureFocused().setOpacity(opacity);

            m_text.getRenderer()->setOpacity(opacity);
        }
        else if (property == "font")
        {
            m_text.getRenderer()->setFont(value.getFont());
            setText(getText());
        }
        else if ((property != "bordercolor")
              && (property != "bordercolorhover")
              && (property != "bordercolordown")
              && (property != "bordercolordisabled")
              && (property != "backgroundcolor")
              && (property != "backgroundcolorhover")
              && (property != "backgroundcolordown")
              && (property != "backgroundcolordisabled"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Button::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());

            // Don't try to draw the text when there is no space left for it
            if ((getSize().x <= borders.left + borders.right) || (getSize().y <= borders.top + borders.bottom))
                return;
        }

        // Check if there is a background texture
        states.transform.translate({borders.left, borders.top});
        if (getRenderer()->getTexture().isLoaded())
        {
            if (!m_enabled && getRenderer()->getTextureDisabled().isLoaded())
                getRenderer()->getTextureDisabled().draw(target, states);
            else if (m_mouseHover)
            {
                if (m_mouseDown && getRenderer()->getTextureDown().isLoaded())
                    getRenderer()->getTextureDown().draw(target, states);
                else if (getRenderer()->getTextureHover().isLoaded())
                    getRenderer()->getTextureHover().draw(target, states);
                else
                    getRenderer()->getTexture().draw(target, states);
            }
            else
                getRenderer()->getTexture().draw(target, states);

            // When the edit box is focused then draw an extra image
            if (m_focused && getRenderer()->getTextureFocused().isLoaded())
                getRenderer()->getTextureFocused().draw(target, states);
        }
        else // There is no background texture
        {
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());
        }

        // If the button has a text then also draw the text
        states.transform.translate({(getInnerSize().x - m_text.getSize().x) / 2.f, (getInnerSize().y - m_text.getSize().y) / 2.f});
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color Button::getCurrentBackgroundColor() const
    {
        if (!m_enabled && getRenderer()->getBackgroundColorDisabled().isSet())
            return getRenderer()->getBackgroundColorDisabled();
        else if (m_mouseHover && m_mouseDown && getRenderer()->getBackgroundColorDown().isSet())
            return getRenderer()->getBackgroundColorDown();
        else if (m_mouseHover && getRenderer()->getBackgroundColorHover().isSet())
            return getRenderer()->getBackgroundColorHover();
        else
            return getRenderer()->getBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::updateTextColorAndStyle()
    {
        if (!m_enabled && getRenderer()->getTextStyleDisabled().isSet())
            m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyleDisabled());
        else if (m_mouseHover && m_mouseDown && getRenderer()->getTextStyleDown().isSet())
            m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyleDown());
        else if (m_mouseHover && getRenderer()->getTextStyleHover().isSet())
            m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyleHover());
        else
            m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyle());

        sf::Color color;
        if (!m_enabled && getRenderer()->getTextColorDisabled().isSet())
            color = getRenderer()->getTextColorDisabled();
        else if (m_mouseHover && m_mouseDown && getRenderer()->getTextColorDown().isSet())
            color = getRenderer()->getTextColorDown();
        else if (m_mouseHover && getRenderer()->getTextColorHover().isSet())
            color = getRenderer()->getTextColorHover();
        else
            color = getRenderer()->getTextColor();

        m_text.getRenderer()->setTextColor(calcColorOpacity(color, getRenderer()->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
