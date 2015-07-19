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


#include <TGUI/Widgets/Container.hpp>
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

        m_text.getRenderer()->setTextColor(getRenderer()->m_textColorNormal);
        setSize(120, 30);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::copy(Button::ConstPtr button)
    {
        if (button)
            return std::make_shared<Button>(*button);
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
                // Calculate a possible text size
                float size = getSize().x * 0.75f;
                m_text.setTextSize(static_cast<unsigned int>(size));

                // Make the text smaller when it's too high
                if (m_text.getSize().y > (getSize().y * 0.8f))
                    m_text.setTextSize(static_cast<unsigned int>(size * getSize().y * 0.8f / m_text.getSize().y));
            }
        }
        else // The width of the button is big enough
        {
            m_text.setText(text);

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                // Calculate a possible text size
                float size = getSize().y * 0.75f;
                m_text.setTextSize(static_cast<unsigned int>(size));

                // Make the text smaller when it's too width
                if (m_text.getSize().x > (getSize().x * 0.8f))
                    m_text.setTextSize(static_cast<unsigned int>(size * getSize().x * 0.8f / m_text.getSize().x));
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

    void Button::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        getRenderer()->m_textureNormal.setColor({255, 255, 255, m_opacity});
        getRenderer()->m_textureHover.setColor({255, 255, 255, m_opacity});
        getRenderer()->m_textureDown.setColor({255, 255, 255, m_opacity});
        getRenderer()->m_textureFocused.setColor({255, 255, 255, m_opacity});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMousePressed(float x, float y)
    {
        ClickableWidget::leftMousePressed(x, y);

        m_text.getRenderer()->setTextColor(getRenderer()->m_textColorDown);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMouseReleased(float x, float y)
    {
        if (m_mouseDown)
            sendSignal("Pressed", m_text.getText());

        ClickableWidget::leftMouseReleased(x, y);

        m_text.getRenderer()->setTextColor(getRenderer()->m_textColorHover);
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
        if (getRenderer()->m_textureFocused.getData())
            Widget::widgetFocused();
        else
            unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            getRenderer()->setTextFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});

            m_theme->initWidget(this, primary, secondary);

            // The widget can only be focused when there is an image available for this phase
            if (getRenderer()->m_textureFocused.getData() != nullptr)
                m_allowFocus = true;

            if (force)
            {
                if (getRenderer()->m_textureNormal.getData())
                    setSize(getRenderer()->m_textureNormal.getImageSize());
            }
        }
        else // Load white theme
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
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();

        if (m_mouseDown)
            m_text.getRenderer()->setTextColor(getRenderer()->m_textColorDown);
        else
            m_text.getRenderer()->setTextColor(getRenderer()->m_textColorHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();

        m_text.getRenderer()->setTextColor(getRenderer()->m_textColorNormal);
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

    bool ButtonRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);
        if (property == "font")
            setTextFont(Deserializer::deserialize(ObjectConverter::Type::Font, value).getFont());
        else if (property == "borders")
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
            return WidgetRenderer::setProperty(property, value);

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ButtonRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Font)
        {
            if (property == "font")
                setTextFont(value.getFont());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Borders)
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
                return WidgetRenderer::setProperty(property, std::move(value));
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
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else
            return false;

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::pair<std::string, ObjectConverter>> ButtonRenderer::getPropertyValuePairs() const
    {
        std::vector<std::pair<std::string, ObjectConverter>> pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureNormal.isLoaded())
            pairs.emplace_back("NormalImage", m_textureNormal);
        if (m_textureHover.isLoaded())
            pairs.emplace_back("HoverImage", m_textureHover);
        if (m_textureDown.isLoaded())
            pairs.emplace_back("DownImage", m_textureDown);
        if (m_textureFocused.isLoaded())
            pairs.emplace_back("FocusedImage", m_textureFocused);

        pairs.emplace_back("TextcolorNormal", m_textColorNormal);
        pairs.emplace_back("TextcolorHover", m_textColorHover);
        pairs.emplace_back("TextcolorDown", m_textColorDown);
        pairs.emplace_back("BackgroundColorNormal", m_backgroundColorNormal);
        pairs.emplace_back("BackgroundColorHover", m_backgroundColorHover);
        pairs.emplace_back("BackgroundColorDown", m_backgroundColorDown);
        pairs.emplace_back("BorderColor", m_borderColor);
        pairs.emplace_back("Borders", m_borders);

/// TODO: Font?
///        pairs.emplace_back("font", m_button->m_font);

        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_button->m_font = font;
        m_button->m_text.setTextFont(font);

        // Reposition the text
        m_button->setText(m_button->m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColor(const sf::Color& color)
    {
        m_textColorNormal = color;
        m_textColorHover = color;
        m_textColorDown = color;

        m_button->m_text.getRenderer()->setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorNormal(const sf::Color& color)
    {
        m_textColorNormal = color;

        if (!m_button->m_mouseHover)
            m_button->m_text.getRenderer()->setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorHover(const sf::Color& color)
    {
        m_textColorHover = color;

        if (m_button->m_mouseHover && !m_button->m_mouseDown)
            m_button->m_text.getRenderer()->setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorDown(const sf::Color& color)
    {
        m_textColorDown = color;

        if (m_button->m_mouseHover && m_button->m_mouseDown)
            m_button->m_text.getRenderer()->setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColor(const sf::Color& color)
    {
        m_backgroundColorNormal = color;
        m_backgroundColorHover = color;
        m_backgroundColorDown = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColorNormal(const sf::Color& color)
    {
        m_backgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColorHover(const sf::Color& color)
    {
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBackgroundColorDown(const sf::Color& color)
    {
        m_backgroundColorDown = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setBorderColor(const sf::Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setNormalTexture(const Texture& texture)
    {
        m_textureNormal = texture;
        m_textureNormal.setPosition(m_button->getPosition());
        m_textureNormal.setSize(m_button->getSize());
        m_textureNormal.setColor({255, 255, 255, m_button->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setHoverTexture(const Texture& texture)
    {
        m_textureHover = texture;
        m_textureHover.setPosition(m_button->getPosition());
        m_textureHover.setSize(m_button->getSize());
        m_textureHover.setColor({255, 255, 255, m_button->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setDownTexture(const Texture& texture)
    {
        m_textureDown = texture;
        m_textureDown.setPosition(m_button->getPosition());
        m_textureDown.setSize(m_button->getSize());
        m_textureDown.setColor({255, 255, 255, m_button->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setFocusTexture(const Texture& texture)
    {
        m_textureFocused = texture;
        m_textureFocused.setPosition(m_button->getPosition());
        m_textureFocused.setSize(m_button->getSize());
        m_textureFocused.setColor({255, 255, 255, m_button->getTransparency()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Check if there is a background texture
        if (m_textureNormal.isLoaded())
        {
            if (m_button->m_mouseHover)
            {
                if (m_button->m_mouseDown && m_button->m_mouseHover && m_textureDown.isLoaded())
                    target.draw(m_textureDown, states);
                else if (m_textureHover.isLoaded())
                    target.draw(m_textureHover, states);
                else
                    target.draw(m_textureNormal, states);
            }
            else
                target.draw(m_textureNormal, states);
        }
        else // There is no background texture
        {
            sf::RectangleShape button(m_button->getSize());
            button.setPosition(m_button->getPosition());

            if (m_button->m_mouseHover)
            {
                if (m_button->m_mouseDown)
                    button.setFillColor(m_backgroundColorDown);
                else
                    button.setFillColor(m_backgroundColorHover);
            }
            else
                button.setFillColor(m_backgroundColorNormal);

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
            border.setFillColor(m_borderColor);
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
        auto renderer = std::shared_ptr<ButtonRenderer>(new ButtonRenderer{*this});
        renderer->m_button = static_cast<Button*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
