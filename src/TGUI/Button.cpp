/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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
        m_callback.widgetType = Type_Button;
        m_text.setTextColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::create(const std::string& configFileFilename)
    {
        auto button = std::make_shared<Button>();

        button->m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Open the config file
        ConfigFile configFile{button->m_loadedConfigFile, "Button"};

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = button->m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = button->m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "separatehoverimage")
                button->m_separateHoverImage = configFile.readBool(it);
            else if (it->first == "textcolor")
                button->m_text.setTextColor(configFile.readColor(it));
            else if (it->first == "normalimage")
                configFile.readTexture(it, configFileFolder, button->m_textureNormal);
            else if (it->first == "hoverimage")
                configFile.readTexture(it, configFileFolder, button->m_textureHover);
            else if (it->first == "downimage")
                configFile.readTexture(it, configFileFolder, button->m_textureDown);
            else if (it->first == "focusedimage")
                configFile.readTexture(it, configFileFolder, button->m_textureFocused);
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section Button in " + button->m_loadedConfigFile + "."};
        }

        // Make sure the required texture was loaded
        if (button->m_textureNormal.getData() == nullptr)
            throw Exception{"NormalImage wasn't loaded. Is the Button section in " + button->m_loadedConfigFile + " complete?"};

        button->setSize(button->m_textureNormal.getImageSize());

        // The widget can only be focused when there is an image available for this phase
        if (button->m_textureFocused.getData() != nullptr)
            button->m_allowFocus = true;

        return button;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        m_textureDown.setPosition(getPosition());
        m_textureHover.setPosition(getPosition());
        m_textureNormal.setPosition(getPosition());
        m_textureFocused.setPosition(getPosition());

        // Set the position of the text
        m_text.setPosition(getPosition().x + (getSize().x - m_text.getSize().x) * 0.5f,
                           getPosition().y + (getSize().y - m_text.getSize().y) * 0.5f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(const Layout& size)
    {
        Widget::setSize(size);

        m_textureDown.setSize(getSize());
        m_textureHover.setSize(getSize());
        m_textureNormal.setSize(getSize());
        m_textureFocused.setSize(getSize());

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

    void Button::setTextFont(const sf::Font& font)
    {
        m_text.setTextFont(font);

        // Reposition the text
        setText(m_string);
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

        m_textureNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureHover.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureDown.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureFocused.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if the space key or the return key was pressed
        if (event.code == sf::Keyboard::Space)
        {
            // Add the callback (if the user requested it)
            if (m_callbackFunctions[SpaceKeyPressed].empty() == false)
            {
                m_callback.trigger = SpaceKeyPressed;
                addCallback();
            }
        }
        else if (event.code == sf::Keyboard::Return)
        {
            // Add the callback (if the user requested it)
            if (m_callbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_callback.trigger = ReturnKeyPressed;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::widgetFocused()
    {
        // We can't be focused when we don't have a focus image
        if (m_textureFocused.getData())
            Widget::widgetFocused();
        else
            unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    void Button::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "text")
        {
            setText(value);
        }
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "textsize")
        {
            setTextSize(tgui::stoi(value));
        }
        else if (property == "callback")
        {
            ClickableWidget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "SpaceKeyPressed") || (*it == "spacekeypressed"))
                    bindCallback(SpaceKeyPressed);
                else if ((*it == "ReturnKeyPressed") || (*it == "returnkeypressed"))
                    bindCallback(ReturnKeyPressed);
            }
        }
        else // The property didn't match
            ClickableWidget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "text")
            value = getText().toAnsiString();
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(SpaceKeyPressed) != m_callbackFunctions.end()) && (m_callbackFunctions.at(SpaceKeyPressed).size() == 1) && (m_callbackFunctions.at(SpaceKeyPressed).front() == nullptr))
                callbacks.push_back("SpaceKeyPressed");
            if ((m_callbackFunctions.find(ReturnKeyPressed) != m_callbackFunctions.end()) && (m_callbackFunctions.at(ReturnKeyPressed).size() == 1) && (m_callbackFunctions.at(ReturnKeyPressed).front() == nullptr))
                callbacks.push_back("ReturnKeyPressed");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            ClickableWidget::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Button::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Text", "string"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        return list;
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_separateHoverImage)
        {
            if (m_mouseDown && m_mouseHover && m_textureDown.getData())
                target.draw(m_textureDown, states);
            else if (m_mouseHover && m_textureHover.getData())
                target.draw(m_textureHover, states);
            else
                target.draw(m_textureNormal, states);
        }
        else // The hover image is drawn on top of the normal one
        {
            if (m_mouseDown && m_mouseHover && m_textureDown.getData())
                target.draw(m_textureDown, states);
            else
                target.draw(m_textureNormal, states);

            // When the mouse is on top of the button then draw an extra image
            if (m_mouseHover && m_textureHover.getData())
                target.draw(m_textureHover, states);
        }

        // When the button is focused then draw an extra image
        if (m_focused && m_textureFocused.getData())
            target.draw(m_textureFocused, states);

        // If the button has a text then also draw the text
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
