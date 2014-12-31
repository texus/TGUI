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
#include <TGUI/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button()
    {
        m_callback.widgetType = WidgetType::Button;

        addSignal<SignalString>("Pressed");

        m_renderer = std::make_shared<ButtonRenderer>(this);

        getRenderer()->setBorders(2, 2, 2, 2);

        m_text.setTextColor(getRenderer()->m_textColorNormal);

        setSize(120, 30);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto button = std::make_shared<Button>();

        if (themeFileFilename != "")
        {
            button->getRenderer()->setBorders({0, 0, 0, 0});

            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = loadedThemeFile.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = loadedThemeFile.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    button->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            if (button->getRenderer()->m_textureNormal.getData())
                button->setSize(button->getRenderer()->m_textureNormal.getImageSize());

            // The widget can only be focused when there is an image available for this phase
            if (button->getRenderer()->m_textureFocused.getData() != nullptr)
                button->m_allowFocus = true;
        }

        return button;
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

    void Button::setPosition(const Layout& position)
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

    void Button::setSize(const Layout& size)
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

        getRenderer()->m_textureNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureHover.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureDown.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureFocused.setColor(sf::Color(255, 255, 255, m_opacity));
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

    void Button::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();

        if (m_mouseDown)
            m_text.setTextColor(getRenderer()->m_textColorDown);
        else
            m_text.setTextColor(getRenderer()->m_textColorHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();

        m_text.setTextColor(getRenderer()->m_textColorNormal);
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

    void ButtonRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else if (property == "textcolornormal")
            setTextColorNormal(extractColorFromString(property, value));
        else if (property == "textcolorhover")
            setTextColorHover(extractColorFromString(property, value));
        else if (property == "textcolordown")
            setTextColorDown(extractColorFromString(property, value));
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "backgroundcolornormal")
            setBackgroundColorNormal(extractColorFromString(property, value));
        else if (property == "backgroundcolorhover")
            setBackgroundColorHover(extractColorFromString(property, value));
        else if (property == "backgroundcolordown")
            setBackgroundColorDown(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else if (property == "normalimage")
            extractTextureFromString(property, value, rootPath, m_textureNormal);
        else if (property == "hoverimage")
            extractTextureFromString(property, value, rootPath, m_textureHover);
        else if (property == "downimage")
            extractTextureFromString(property, value, rootPath, m_textureDown);
        else if (property == "focusedimage")
            extractTextureFromString(property, value, rootPath, m_textureFocused);
        else
            throw Exception{"Unrecognized property '" + property + "'."};
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

        m_button->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorNormal(const sf::Color& color)
    {
        m_textColorNormal = color;

        if (!m_button->m_mouseHover)
            m_button->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorHover(const sf::Color& color)
    {
        m_textColorHover = color;

        if (m_button->m_mouseHover && !m_button->m_mouseDown)
            m_button->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setTextColorDown(const sf::Color& color)
    {
        m_textColorDown = color;

        if (m_button->m_mouseHover && m_button->m_mouseDown)
            m_button->m_text.setTextColor(color);
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

    void ButtonRenderer::setNormalImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureNormal.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureNormal = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setHoverImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureHover.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureHover = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setDownImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureDown.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureDown = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::setFocusedImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureFocused.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureFocused = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Check if there is a background texture
        if (m_textureNormal.getData() != nullptr)
        {
            if (m_button->m_mouseHover)
            {
                if (m_button->m_mouseDown && m_button->m_mouseHover && m_textureDown.getData())
                    target.draw(m_textureDown, states);
                else if (m_textureHover.getData())
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
