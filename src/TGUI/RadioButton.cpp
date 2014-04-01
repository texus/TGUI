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
#include <TGUI/RadioButton.hpp>

#include <cmath>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton() :
        m_checked       (false),
        m_allowTextClick(true),
        m_textSize      (0)
    {
        m_callback.widgetType = Type_RadioButton;

        m_text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton(const RadioButton& copy) :
    ClickableWidget   (copy),
    m_loadedConfigFile(copy.m_loadedConfigFile),
    m_checked         (copy.m_checked),
    m_allowTextClick  (copy.m_allowTextClick),
    m_text            (copy.m_text),
    m_textSize        (copy.m_textSize)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureUnchecked, m_textureUnchecked);
        TGUI_TextureManager.copyTexture(copy.m_textureChecked, m_textureChecked);
        TGUI_TextureManager.copyTexture(copy.m_textureHover, m_textureHover);
        TGUI_TextureManager.copyTexture(copy.m_textureFocused, m_textureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::~RadioButton()
    {
        if (m_textureUnchecked.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureUnchecked);
        if (m_textureChecked.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureChecked);
        if (m_textureHover.getData() != nullptr)      TGUI_TextureManager.removeTexture(m_textureHover);
        if (m_textureFocused.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton& RadioButton::operator= (const RadioButton& right)
    {
        if (this != &right)
        {
            RadioButton temp(right);
            this->ClickableWidget::operator=(right);

            std::swap(m_loadedConfigFile, temp.m_loadedConfigFile);
            std::swap(m_checked,          temp.m_checked);
            std::swap(m_allowTextClick,   temp.m_allowTextClick);
            std::swap(m_text,             temp.m_text);
            std::swap(m_textSize,         temp.m_textSize);
            std::swap(m_textureUnchecked, temp.m_textureUnchecked);
            std::swap(m_textureChecked,   temp.m_textureChecked);
            std::swap(m_textureHover,     temp.m_textureHover);
            std::swap(m_textureFocused,   temp.m_textureFocused);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton* RadioButton::clone()
    {
        return new RadioButton(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_loaded = false;

         // If the radio button was loaded before then remove the old textures
        if (m_textureUnchecked.getData() != nullptr) TGUI_TextureManager.removeTexture(m_textureUnchecked);
        if (m_textureChecked.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureChecked);
        if (m_textureHover.getData() != nullptr)     TGUI_TextureManager.removeTexture(m_textureHover);
        if (m_textureFocused.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureFocused);

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
        if (!configFile.read("RadioButton", properties, values))
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

            if (property == "textcolor")
            {
                m_text.setColor(configFile.readColor(value));
            }
            else if (property == "checkedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureChecked))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CheckedImage in section RadioButton in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "uncheckedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureUnchecked))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for UncheckedImage in section RadioButton in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section RadioButton in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFocused))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section RadioButton in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section RadioButton in " + m_loadedConfigFile + ".");
        }

        // Make sure the required texture was loaded
        if ((m_textureChecked.getData() != nullptr) && (m_textureUnchecked.getData() != nullptr))
        {
            m_loaded = true;
            setSize(m_textureUnchecked.getImageSize().x, m_textureUnchecked.getImageSize().y);
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the radio button. Is the RadioButton section in " + m_loadedConfigFile + " complete?");
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

    const std::string& RadioButton::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setPosition(float x, float y)
    {
        ClickableWidget::setPosition(x, y);

        m_textureUnchecked.setPosition(x, y);
        m_textureChecked.setPosition(x, y + m_textureUnchecked.getSize().y - m_textureChecked.getSize().y);
        m_textureFocused.setPosition(x, y);
        m_textureHover.setPosition(x, y);

        sf::FloatRect textBounds = m_text.getLocalBounds();
        m_text.setPosition(x + std::floor(m_textureUnchecked.getSize().x * 11.0f / 10.0f - textBounds.left),
                           y + std::floor(((m_textureUnchecked.getSize().y - textBounds.height) / 2.0f) - textBounds.top));
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(float width, float height)
    {
        // Don't do anything when the radio button wasn't loaded correctly
        if (m_loaded == false)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // If the text is auto sized then recalculate the size
        if (m_textSize == 0)
            setText(m_text.getString());

        m_textureChecked.setSize(width, height);
        m_textureUnchecked.setSize(width, height);
        m_textureFocused.setSize(width, height);
        m_textureHover.setSize(width, height);

        // Reposition the text
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getSize() const
    {
        return m_textureUnchecked.getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getFullSize() const
    {
        if (m_text.getString().isEmpty())
            return m_textureUnchecked.getSize();
        else
            return sf::Vector2f((m_textureUnchecked.getSize().x * 11.0f / 10.0f) + m_text.getLocalBounds().left + m_text.getLocalBounds().width, m_textureUnchecked.getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::check()
    {
        if (m_checked == false)
        {
            // Tell our parent that all the radio buttons should be unchecked
            m_parent->uncheckRadioButtons();

            // Check this radio button
            m_checked = true;

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[Checked].empty() == false)
            {
                m_callback.trigger = Checked;
                m_callback.checked = true;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::uncheck()
    {
        if (m_checked)
        {
            m_checked = false;

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[Unchecked].empty() == false)
            {
                m_callback.trigger = Unchecked;
                m_callback.checked = false;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::isChecked() const
    {
        return m_checked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setText(const sf::String& text)
    {
        // Don't do anything when the radio button wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Set the new text
        m_text.setString(text);

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            // Set the text size
            m_text.setCharacterSize(static_cast<unsigned int>(m_textureUnchecked.getSize().y));
            m_text.setCharacterSize(static_cast<unsigned int>(m_text.getCharacterSize() - m_text.getLocalBounds().top));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_text.setCharacterSize(m_textSize);
        }

        // Reposition the text
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String RadioButton::getText() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextFont(const sf::Font& font)
    {
        m_text.setFont(font);

        // Recalculate the text position and size
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* RadioButton::getTextFont() const
    {
        return m_text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextColor(const sf::Color& Color)
    {
        m_text.setColor(Color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& RadioButton::getTextColor() const
    {
        return m_text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int RadioButton::getTextSize() const
    {
        return m_text.getCharacterSize();
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

        m_textureChecked.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureUnchecked.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureHover.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureFocused.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the radio button wasn't loaded correctly
        if (m_loaded == false)
            return false;

        // Check if the mouse is on top of the image
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_textureUnchecked.getSize().x, m_textureUnchecked.getSize().y)).contains(x, y))
            return true;
        else
        {
            // Check if the mouse is on top of the text
            if (m_allowTextClick)
            {
                sf::FloatRect bounds = m_text.getLocalBounds();
                if (sf::FloatRect(bounds.left, bounds.top, bounds.width, bounds.height).contains(x - (getPosition().x + ((m_textureUnchecked.getSize().x * 11.0f / 10.0f))), y - getPosition().y - ((m_textureUnchecked.getSize().y - bounds.height) / 2.0f) + bounds.top))
                    return true;
            }
        }

        if (m_mouseHover == true)
            mouseLeftWidget();

        // The mouse is not on top of the radio button
        m_mouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::leftMouseReleased(float x, float y)
    {
        // Add the callback (if the user requested it)
        if (m_callbackFunctions[LeftMouseReleased].empty() == false)
        {
            m_callback.trigger = LeftMouseReleased;
            m_callback.checked = m_checked;
            m_callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }

        // Check if we clicked on the radio button (not just mouse release)
        if (m_mouseDown == true)
        {
            // Check the radio button
            check();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[LeftMouseClicked].empty() == false)
            {
                m_callback.trigger = LeftMouseClicked;
                m_callback.checked = m_checked;
                m_callback.mouse.x = static_cast<int>(x - getPosition().x);
                m_callback.mouse.y = static_cast<int>(y - getPosition().y);
                addCallback();
            }

            m_mouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if the space key or the return key was pressed
        if (event.code == sf::Keyboard::Space)
        {
            // Check the radio button
            check();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[SpaceKeyPressed].empty() == false)
            {
                m_callback.trigger = SpaceKeyPressed;
                m_callback.checked = m_checked;
                addCallback();
            }
        }
        else if (event.code == sf::Keyboard::Return)
        {
            // Check the radio button
            check();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_callback.trigger = ReturnKeyPressed;
                m_callback.checked = m_checked;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::widgetFocused()
    {
        // We can't be focused when we don't have a focus image
        if (m_textureFocused.getData())
            Widget::widgetFocused();
        else
            unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "checked")
        {
            if ((value == "true") || (value == "True"))
                check();
            else if ((value == "false") || (value == "False"))
                uncheck();
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Checked' property.");
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
        else if (property == "allowtextclick")
        {
            if ((value == "true") || (value == "True"))
                allowTextClick(true);
            else if ((value == "false") || (value == "False"))
                allowTextClick(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'AllowTextClick' property.");
        }
        else if (property == "callback")
        {
            ClickableWidget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "Checked") || (*it == "checked"))
                    bindCallback(Checked);
                else if ((*it == "Unchecked") || (*it == "unchecked"))
                    bindCallback(Unchecked);
                else if ((*it == "SpaceKeyPressed") || (*it == "spacekeypressed"))
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

    bool RadioButton::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "width")
            value = tgui::to_string(m_textureUnchecked.getSize().x);
        else if (property == "height")
            value = tgui::to_string(m_textureUnchecked.getSize().y);
        else if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "checked")
            value = m_checked ? "true" : "false";
        else if (property == "text")
            value = getText().toAnsiString();
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "allowtextclick")
            value = m_allowTextClick ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(Checked) != m_callbackFunctions.end()) && (m_callbackFunctions.at(Checked).size() == 1) && (m_callbackFunctions.at(Checked).front() == nullptr))
                callbacks.push_back("Checked");
            if ((m_callbackFunctions.find(Unchecked) != m_callbackFunctions.end()) && (m_callbackFunctions.at(Unchecked).size() == 1) && (m_callbackFunctions.at(Unchecked).front() == nullptr))
                callbacks.push_back("Unchecked");
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
        else
            return ClickableWidget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > RadioButton::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Checked", "bool"));
        list.push_back(std::pair<std::string, std::string>("Text", "string"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("AllowTextClick", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_checked)
            target.draw(m_textureChecked, states);
        else
            target.draw(m_textureUnchecked, states);

        // When the radio button is focused then draw an extra image
        if (m_focused && m_textureFocused.getData())
            target.draw(m_textureFocused, states);

        // When the mouse is on top of the radio button then draw an extra image
        if (m_mouseHover && m_textureHover.getData())
            target.draw(m_textureHover, states);

        // Draw the text
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
