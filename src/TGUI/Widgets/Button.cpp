/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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

        m_renderer = aurora::makeCopied<tgui::ButtonRenderer>();
        setRenderer(m_renderer->getData());

        setSize(120, 30);
        getRenderer()->setBorders({2, 2, 2, 2});

        m_sizeDefined = false;
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
        getRenderer()->getTextureNormal().setSize(getSize());
        getRenderer()->getTextureHover().setSize(getSize());
        getRenderer()->getTextureDown().setSize(getSize());
        getRenderer()->getTextureFocused().setSize(getSize());

        // Recalculate the text size when auto sizing
        if (m_textSize == 0)
            setText(m_string);

        recalculateTextPosition();
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

        // Draw the text normally unless the height is more than double of the width
        if (getSize().y <= getSize().x * 2)
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
                unsigned int textSize = findBestTextSize(getFont(), getSize().x * 0.85f);
                m_text.setTextSize(textSize);

                // Make the text smaller when it's too high
                if (m_text.getSize().y > (getSize().y * 0.85f))
                    m_text.setTextSize(static_cast<unsigned int>(textSize * getSize().y * 0.85f / m_text.getSize().y));
            }
        }

        recalculateTextPosition();
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
            setText(m_string);
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

        m_text.getRenderer()->setTextColor(getRenderer()->getTextColorDown());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMouseReleased(float x, float y)
    {
        if (m_mouseDown)
            sendSignal("Pressed", m_text.getText());

        ClickableWidget::leftMouseReleased(x, y);

        if (m_mouseHover)
            m_text.getRenderer()->setTextColor(getRenderer()->getTextColorHover());
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

    void Button::rendererChanged(const std::string& property, ObjectConverter&& value)
    {
        if (property == "borders")
        {
            recalculateTextPosition();
        }
        else if (property == "textcolornormal")
        {
            if (!m_mouseHover)
                m_text.getRenderer()->setTextColor(calcColorOpacity(value.getColor(), getRenderer()->getOpacity()));
        }
        else if (property == "textcolorhover")
        {
            if (m_mouseHover && !m_mouseDown)
                m_text.getRenderer()->setTextColor(calcColorOpacity(value.getColor(), getRenderer()->getOpacity()));
        }
        else if (property == "textcolordown")
        {
            if (m_mouseHover && m_mouseDown)
                m_text.getRenderer()->setTextColor(calcColorOpacity(value.getColor(), getRenderer()->getOpacity()));
        }
        else if (property == "texturenormal")
        {
            if (value.getTexture().isLoaded())
            {
                value.getTexture().setColor({255, 255, 255, static_cast<sf::Uint8>(getRenderer()->getOpacity() * 255)});

                if (m_sizeDefined)
                    value.getTexture().setSize(getSize());
                else
                    setSize(value.getTexture().getSize());
            }
        }
        else if ((property == "texturehover") || (property == "texturedown"))
        {
            if (value.getTexture().isLoaded())
            {
                value.getTexture().setColor({255, 255, 255, static_cast<sf::Uint8>(getRenderer()->getOpacity() * 255)});
                value.getTexture().setSize(getSize());
            }
        }
        else if (property == "texturefocused")
        {
            if (value.getTexture().isLoaded())
            {
                value.getTexture().setColor({255, 255, 255, static_cast<sf::Uint8>(getRenderer()->getOpacity() * 255)});
                value.getTexture().setSize(getSize());
                m_allowFocus = true;
            }
            else
                m_allowFocus = false;
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();
            getRenderer()->getTextureNormal().setColor({255, 255, 255, static_cast<sf::Uint8>(opacity * 255)});
            getRenderer()->getTextureHover().setColor({255, 255, 255, static_cast<sf::Uint8>(opacity * 255)});
            getRenderer()->getTextureDown().setColor({255, 255, 255, static_cast<sf::Uint8>(opacity * 255)});
            getRenderer()->getTextureFocused().setColor({255, 255, 255, static_cast<sf::Uint8>(opacity * 255)});

            if (!m_mouseHover)
                m_text.getRenderer()->setTextColor(calcColorOpacity(getRenderer()->getTextColorNormal(), opacity));
            else if (m_mouseHover && !m_mouseDown)
                m_text.getRenderer()->setTextColor(calcColorOpacity(getRenderer()->getTextColorHover(), opacity));
            else if (m_mouseHover && m_mouseDown)
                m_text.getRenderer()->setTextColor(calcColorOpacity(getRenderer()->getTextColorDown(), opacity));
        }
        else if ((property != "bordercolor")
              && (property != "backgroundcolornormal")
              && (property != "backgroundcolorhover")
              && (property != "backgroundcolordown"))
        {
            Widget::rendererChanged(property, std::move(value));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();

        if (m_mouseDown)
            m_text.getRenderer()->setTextColor(calcColorOpacity(getRenderer()->getTextColorDown(), getRenderer()->getOpacity()));
        else
            m_text.getRenderer()->setTextColor(calcColorOpacity(getRenderer()->getTextColorHover(), getRenderer()->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();

        m_text.getRenderer()->setTextColor(calcColorOpacity(getRenderer()->getTextColorNormal(), getRenderer()->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Check if there is a background texture
        if (getRenderer()->getTextureNormal().isLoaded())
        {
            if (m_mouseHover)
            {
                if (m_mouseDown && m_mouseHover && getRenderer()->getTextureDown().isLoaded())
                    target.draw(getRenderer()->getTextureDown(), states);
                else if (getRenderer()->getTextureHover().isLoaded())
                    target.draw(getRenderer()->getTextureHover(), states);
                else
                    target.draw(getRenderer()->getTextureNormal(), states);
            }
            else
                target.draw(getRenderer()->getTextureNormal(), states);

            // When the edit box is focused then draw an extra image
            if (m_focused && getRenderer()->getTextureFocused().isLoaded())
                target.draw(getRenderer()->getTextureFocused(), states);
        }
        else // There is no background texture
        {
            sf::RectangleShape background(getSize());
            background.setPosition(getPosition());

            if (m_mouseHover)
            {
                if (m_mouseDown)
                    background.setFillColor(calcColorOpacity(getRenderer()->getBackgroundColorDown(), getRenderer()->getOpacity()));
                else
                    background.setFillColor(calcColorOpacity(getRenderer()->getBackgroundColorHover(), getRenderer()->getOpacity()));
            }
            else
                background.setFillColor(calcColorOpacity(getRenderer()->getBackgroundColorNormal(), getRenderer()->getOpacity()));

            target.draw(background, states);
        }

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0, 0, 0, 0})
        {
            drawBorders(target, states, borders, getPosition(), getSize(), calcColorOpacity(getRenderer()->getBorderColor(), getRenderer()->getOpacity()));

            // Don't try to draw the text when there is no space left for it
            if ((getSize().x <= borders.left + borders.right) || (getSize().y <= borders.top + borders.bottom))
                return;
        }

        // If the button has a text then also draw the text
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::recalculateTextPosition()
    {
        Borders borders = getRenderer()->getBorders();
        m_text.setPosition(((getSize().x - borders.left - borders.right) - m_text.getSize().x) * 0.5f,
                           ((getSize().y - borders.top - borders.bottom) - m_text.getSize().y) * 0.5f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColor(const Color& color)
    {
        setTextColorNormal(color);
        setTextColorHover(color);
        setTextColorDown(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColor(const Color& color)
    {
        setBackgroundColorNormal(color);
        setBackgroundColorHover(color);
        setBackgroundColorDown(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_RENDERER_PROPERTY_BORDERS(ButtonRenderer, Borders, Borders(0))

    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorNormal, Color(60, 60, 60))
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorHover, sf::Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorDown, sf::Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorNormal, Color(245, 245, 245))
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorHover, sf::Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorDown, sf::Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColor, sf::Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureNormal)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureFocused)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
