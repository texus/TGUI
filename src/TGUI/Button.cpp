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


#include <cmath>

#include <TGUI/Container.hpp>
#include <TGUI/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button() :
    m_separateHoverImage(false),
    m_textSize          (0)
    {
        m_callback.widgetType = Type_Button;
        m_text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button(const Button& copy) :
    ClickableWidget     (copy),
    m_loadedConfigFile  (copy.m_loadedConfigFile),
    m_separateHoverImage(copy.m_separateHoverImage),
    m_text              (copy.m_text),
    m_textSize          (copy.m_textSize)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureNormal, m_textureNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureHover, m_textureHover);
        TGUI_TextureManager.copyTexture(copy.m_textureDown, m_textureDown);
        TGUI_TextureManager.copyTexture(copy.m_textureFocused, m_textureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::~Button()
    {
        if (m_textureNormal.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal);
        if (m_textureHover.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover);
        if (m_textureDown.getData() != nullptr)     TGUI_TextureManager.removeTexture(m_textureDown);
        if (m_textureFocused.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button& Button::operator= (const Button& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Button temp(right);
            this->ClickableWidget::operator=(right);

            std::swap(m_loadedConfigFile,   temp.m_loadedConfigFile);
            std::swap(m_textureNormal,      temp.m_textureNormal);
            std::swap(m_textureHover,       temp.m_textureHover);
            std::swap(m_textureDown,        temp.m_textureDown);
            std::swap(m_textureFocused,     temp.m_textureFocused);
            std::swap(m_separateHoverImage, temp.m_separateHoverImage);
            std::swap(m_text,               temp.m_text);
            std::swap(m_textSize,           temp.m_textSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button* Button::clone()
    {
        return new Button(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Button::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_loaded = false;

        // If the button was loaded before then remove the old textures first
        if (m_textureNormal.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal);
        if (m_textureHover.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover);
        if (m_textureDown.getData() != nullptr)     TGUI_TextureManager.removeTexture(m_textureDown);
        if (m_textureFocused.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(m_loadedConfigFile))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_loadedConfigFile + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("Button", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_loadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "separatehoverimage")
            {
                m_separateHoverImage = configFile.readBool(value, false);
            }
            else if (property == "textcolor")
            {
                m_text.setColor(configFile.readColor(value));
            }
            else if (property == "normalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureDown))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFocused))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Button in " + m_loadedConfigFile + ".");
        }

        // Make sure the required texture was loaded
        if (m_textureNormal.getData() != nullptr)
        {
            m_loaded = true;
            setSize(m_textureNormal.getSize().x, m_textureNormal.getSize().y);
        }
        else
        {
            TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the Button section in " + m_loadedConfigFile + " complete?");
            return false;
        }

        // Check if optional textures were loaded
        if (m_textureFocused.getData() != nullptr)
        {
            m_allowFocus = true;
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Button::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setPosition(float x, float y)
    {
        Transformable::setPosition(x, y);

        m_textureDown.setPosition(x, y);
        m_textureHover.setPosition(x, y);
        m_textureNormal.setPosition(x, y);
        m_textureFocused.setPosition(x, y);

        // Set the position of the text
        m_text.setPosition(std::floor(x + (m_textureNormal.getSize().x - m_text.getLocalBounds().width) * 0.5f -  m_text.getLocalBounds().left),
                           std::floor(y + (m_textureNormal.getSize().y - m_text.getLocalBounds().height) * 0.5f -  m_text.getLocalBounds().top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(float width, float height)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Recalculate the text size when auto sizing
        if (m_textSize == 0)
            setText(m_text.getString());

        m_textureDown.setSize(width, height);
        m_textureHover.setSize(width, height);
        m_textureNormal.setSize(width, height);
        m_textureFocused.setSize(width, height);

        // Recalculate the position of the images
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Button::getSize() const
    {
        return m_textureNormal.getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const sf::String& text)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Set the new text
        m_text.setString(text);
        m_callback.text = text;

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            // Calculate a possible text size
            float size = m_textureNormal.getSize().y * 0.85f;
            m_text.setCharacterSize(static_cast<unsigned int>(size));
            m_text.setCharacterSize(static_cast<unsigned int>(m_text.getCharacterSize() - m_text.getLocalBounds().top));

            // Make sure that the text isn't too width
            if (m_text.getGlobalBounds().width > (m_textureNormal.getSize().x * 0.8f))
            {
                // The text is too width, so make it smaller
                m_text.setCharacterSize(static_cast<unsigned int>(size * m_textureNormal.getSize().x * 0.8f / m_text.getGlobalBounds().width));
                m_text.setCharacterSize(static_cast<unsigned int>(m_text.getCharacterSize() - m_text.getLocalBounds().top));
            }
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_text.setCharacterSize(m_textSize);
        }

        // Set the position of the text
        m_text.setPosition(std::floor(getPosition().x + (m_textureNormal.getSize().x - m_text.getLocalBounds().width) * 0.5f -  m_text.getLocalBounds().left),
                           std::floor(getPosition().y + (m_textureNormal.getSize().y - m_text.getLocalBounds().height) * 0.5f -  m_text.getLocalBounds().top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Button::getText() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextFont(const sf::Font& font)
    {
        m_text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Button::getTextFont() const
    {
        return m_text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextColor(const sf::Color& color)
    {
        m_text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Button::getTextColor() const
    {
        return m_text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Button::getTextSize() const
    {
        return m_text.getCharacterSize();
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

    void Button::keyPressed(sf::Keyboard::Key key)
    {
        // Check if the space key or the return key was pressed
        if (key == sf::Keyboard::Space)
        {
            // Add the callback (if the user requested it)
            if (m_callbackFunctions[SpaceKeyPressed].empty() == false)
            {
                m_callback.trigger = SpaceKeyPressed;
                addCallback();
            }
        }
        else if (key == sf::Keyboard::Return)
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

    bool Button::setProperty(std::string property, const std::string& value)
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
            setTextSize(std::stoi(value));
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
            return ClickableWidget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Button::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "text")
            value = getText().toAnsiString();
        else if (property == "textcolor")
            value = "(" + std::to_string(int(getTextColor().r)) + "," + std::to_string(int(getTextColor().g)) + "," + std::to_string(int(getTextColor().b)) + "," + std::to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = std::to_string(getTextSize());
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
            return ClickableWidget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::initialize(Container *const parent)
    {
        m_parent = parent;
        m_text.setFont(m_parent->getGlobalFont());
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
