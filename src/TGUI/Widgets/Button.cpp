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
        m_type = "Button";

        addSignal<sf::String>("Pressed");

        m_renderer = aurora::makeCopied<tgui::ButtonRenderer>();
        setRenderer(m_renderer->getData());

        setSize(120, 30);
        getRenderer()->setBorders({2, 2, 2, 2});
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
        getRenderer()->getTextureNormal().setSize(getInnerSize());
        getRenderer()->getTextureHover().setSize(getInnerSize());
        getRenderer()->getTextureDown().setSize(getInnerSize());
        getRenderer()->getTextureFocused().setSize(getInnerSize());

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
        if (getInnerSize().y <= getInnerSize().x * 2)
        {
            m_text.setText(text);

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                unsigned int textSize = findBestTextSize(getFont(), getInnerSize().y * 0.85f);
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
                unsigned int textSize = findBestTextSize(getFont(), getInnerSize().x * 0.85f);
                m_text.setTextSize(textSize);

                // Make the text smaller when it's too high
                if (m_text.getSize().y > (getInnerSize().y * 0.85f))
                    m_text.setTextSize(static_cast<unsigned int>(textSize * getInnerSize().y * 0.85f / m_text.getSize().y));
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
            updateSize();

            sf::Vector2f texturePosition{value.getOutline().left, value.getOutline().top};
            getRenderer()->getTextureNormal().setPosition(texturePosition);
            getRenderer()->getTextureHover().setPosition(texturePosition);
            getRenderer()->getTextureDown().setPosition(texturePosition);
            getRenderer()->getTextureFocused().setPosition(texturePosition);
        }
        else if (property == "textcolornormal")
        {
            if (!m_mouseHover)
                m_text.getRenderer()->setTextColor(value.getColor());
        }
        else if (property == "textcolorhover")
        {
            if (m_mouseHover && !m_mouseDown)
                m_text.getRenderer()->setTextColor(value.getColor());
        }
        else if (property == "textcolordown")
        {
            if (m_mouseHover && m_mouseDown)
                m_text.getRenderer()->setTextColor(value.getColor());
        }
        else if ((property == "texturenormal") || (property == "texturehover") || (property == "texturedown") || (property == "texturefocused"))
        {
            value.getTexture().setPosition({getRenderer()->getBorders().left, getRenderer()->getBorders().top});
            value.getTexture().setSize(getInnerSize());

            if (property == "texturefocused")
                m_allowFocus = value.getTexture().isLoaded();
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();

            getRenderer()->getTextureNormal().setOpacity(opacity);
            getRenderer()->getTextureHover().setOpacity(opacity);
            getRenderer()->getTextureDown().setOpacity(opacity);
            getRenderer()->getTextureFocused().setOpacity(opacity);

            m_text.getRenderer()->setOpacity(opacity);
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

    sf::Vector2f Button::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
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
                if (m_mouseDown && getRenderer()->getTextureDown().isLoaded())
                    getRenderer()->getTextureDown().draw(target, states);
                else if (getRenderer()->getTextureHover().isLoaded())
                    getRenderer()->getTextureHover().draw(target, states);
                else
                    getRenderer()->getTextureNormal().draw(target, states);
            }
            else
                getRenderer()->getTextureNormal().draw(target, states);

            // When the edit box is focused then draw an extra image
            if (m_focused && getRenderer()->getTextureFocused().isLoaded())
                getRenderer()->getTextureFocused().draw(target, states);
        }
        else // There is no background texture
        {
            sf::Color backroundColor;
            if (m_mouseHover)
            {
                if (m_mouseDown)
                    backroundColor = getRenderer()->getBackgroundColorDown();
                else
                    backroundColor = getRenderer()->getBackgroundColorHover();
            }
            else
                backroundColor = getRenderer()->getBackgroundColorNormal();

            drawRectangleShape(target, states, getPosition(), getInnerSize(), backroundColor);
        }

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getPosition(), getSize(), getRenderer()->getBorderColor());

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
        m_text.setPosition((getInnerSize().x - m_text.getSize().x) * 0.5f,
                           (getInnerSize().y - m_text.getSize().y) * 0.5f);
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

    TGUI_RENDERER_PROPERTY_OUTLINE(ButtonRenderer, Borders, Borders(0))

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
