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
#include <TGUI/RadioButton.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton() :
        m_Checked       (false),
        m_AllowTextClick(true),
        m_TextSize      (0)
    {
        m_Callback.widgetType = Type_RadioButton;

        m_Text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton(const RadioButton& copy) :
    ClickableWidget   (copy),
    m_LoadedConfigFile(copy.m_LoadedConfigFile),
    m_Checked         (copy.m_Checked),
    m_AllowTextClick  (copy.m_AllowTextClick),
    m_Text            (copy.m_Text),
    m_TextSize        (copy.m_TextSize)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureUnchecked, m_TextureUnchecked);
        TGUI_TextureManager.copyTexture(copy.m_TextureChecked, m_TextureChecked);
        TGUI_TextureManager.copyTexture(copy.m_TextureHover, m_TextureHover);
        TGUI_TextureManager.copyTexture(copy.m_TextureFocused, m_TextureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::~RadioButton()
    {
        if (m_TextureUnchecked.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureUnchecked);
        if (m_TextureChecked.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureChecked);
        if (m_TextureHover.data != nullptr)      TGUI_TextureManager.removeTexture(m_TextureHover);
        if (m_TextureFocused.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton& RadioButton::operator= (const RadioButton& right)
    {
        if (this != &right)
        {
            RadioButton temp(right);
            this->ClickableWidget::operator=(right);

            std::swap(m_LoadedConfigFile, temp.m_LoadedConfigFile);
            std::swap(m_Checked,          temp.m_Checked);
            std::swap(m_AllowTextClick,   temp.m_AllowTextClick);
            std::swap(m_Text,             temp.m_Text);
            std::swap(m_TextSize,         temp.m_TextSize);
            std::swap(m_TextureUnchecked, temp.m_TextureUnchecked);
            std::swap(m_TextureChecked,   temp.m_TextureChecked);
            std::swap(m_TextureHover,     temp.m_TextureHover);
            std::swap(m_TextureFocused,   temp.m_TextureFocused);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton* RadioButton::clone()
    {
        return new RadioButton(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::load(const std::string& configFileFilename, const std::string& sectionName)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

         // If the radio button was loaded before then remove the old textures
        if (m_TextureUnchecked.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureUnchecked);
        if (m_TextureChecked.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureChecked);
        if (m_TextureHover.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureHover);
        if (m_TextureFocused.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureFocused);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(m_LoadedConfigFile))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_LoadedConfigFile + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read(sectionName, properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_LoadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_LoadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_LoadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "textcolor")
            {
                m_Text.setColor(configFile.readColor(value));
            }
            else if (property == "checkedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureChecked))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CheckedImage in section RadioButton in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "uncheckedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureUnchecked))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for UncheckedImage in section RadioButton in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section RadioButton in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section RadioButton in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section RadioButton in " + m_LoadedConfigFile + ".");
        }

        // Make sure the required texture was loaded
        if ((m_TextureChecked.data != nullptr) && (m_TextureUnchecked.data != nullptr))
        {
            m_Loaded = true;
            m_Size = sf::Vector2f(m_TextureChecked.getSize());
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the radio button. Is the RadioButton section in " + m_LoadedConfigFile + " complete?");
            return false;
        }

        // Check if optional textures were loaded
        if (m_TextureFocused.data != nullptr)
        {
            m_AllowFocus = true;
            m_WidgetPhase |= WidgetPhase_Focused;
        }
        if (m_TextureHover.data != nullptr)
        {
            m_WidgetPhase |= WidgetPhase_Hover;
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& RadioButton::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setPosition(float x, float y)
    {
        ClickableWidget::setPosition(x, y);

        m_TextureUnchecked.sprite.setPosition(x, y);
        m_TextureChecked.sprite.setPosition(x, y + m_TextureUnchecked.getSize().y - m_TextureChecked.getSize().y);
        m_TextureFocused.sprite.setPosition(x, y);
        m_TextureHover.sprite.setPosition(x, y);

        sf::FloatRect textBounds = m_Text.getLocalBounds();
        m_Text.setPosition(x + std::floor(m_Size.x * 11.0f / 10.0f - textBounds.left),
                           y + std::floor(((m_Size.y - textBounds.height) / 2.0f) - textBounds.top));
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(float width, float height)
    {
        // Don't do anything when the radio button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the radio button
        m_Size.x = width;
        m_Size.y = height;

        // If the text is auto sized then recalculate the size
        if (m_TextSize == 0)
            setText(m_Text.getString());

        sf::Vector2f scaling(m_Size.x / m_TextureUnchecked.getSize().x, m_Size.y / m_TextureUnchecked.getSize().y);
        m_TextureChecked.sprite.setScale(scaling);
        m_TextureUnchecked.sprite.setScale(scaling);
        m_TextureFocused.sprite.setScale(scaling);
        m_TextureHover.sprite.setScale(scaling);

        // Reposition the text
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getSize() const
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getFullSize() const
    {
        if (m_Text.getString().isEmpty())
            return m_Size;
        else
            return sf::Vector2f(static_cast<float>((m_Size.x * 11.0 / 10.0) + m_Text.getLocalBounds().left + m_Text.getLocalBounds().width), m_Size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::check()
    {
        if (m_Checked == false)
        {
            // Tell our parent that all the radio buttons should be unchecked
            m_Parent->uncheckRadioButtons();

            // Check this radio button
            m_Checked = true;

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[Checked].empty() == false)
            {
                m_Callback.trigger = Checked;
                m_Callback.checked = true;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::uncheck()
    {
        if (m_Checked)
        {
            m_Checked = false;

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[Unchecked].empty() == false)
            {
                m_Callback.trigger = Unchecked;
                m_Callback.checked = false;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::isChecked() const
    {
        return m_Checked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setText(const sf::String& text)
    {
        // Don't do anything when the radio button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new text
        m_Text.setString(text);

        // Check if the text is auto sized
        if (m_TextSize == 0)
            m_Text.setCharacterSize(static_cast<unsigned int>(m_Size.y * 0.75f));
        else
            m_Text.setCharacterSize(m_TextSize);

        // Reposition the text
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String RadioButton::getText() const
    {
        return m_Text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* RadioButton::getTextFont() const
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextColor(const sf::Color& Color)
    {
        m_Text.setColor(Color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& RadioButton::getTextColor() const
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextSize(unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int RadioButton::getTextSize() const
    {
        return m_Text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::allowTextClick(bool acceptTextClick)
    {
        m_AllowTextClick = acceptTextClick;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        m_TextureChecked.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureUnchecked.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureHover.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureFocused.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the radio button wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Check if the mouse is on top of the image
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
            return true;
        else
        {
            // Check if the mouse is on top of the text
            if (m_AllowTextClick)
            {
                sf::FloatRect bounds = m_Text.getLocalBounds();
                if (sf::FloatRect(bounds.left, bounds.top, bounds.width, bounds.height).contains(x - (getPosition().x + ((m_Size.x * 11.0f / 10.0f))), y - getPosition().y - ((m_Size.y - bounds.height) / 2.0f) + bounds.top))
                    return true;
            }
        }

        if (m_MouseHover == true)
            mouseLeftWidget();

        // The mouse is not on top of the radio button
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::leftMouseReleased(float x, float y)
    {
        // Add the callback (if the user requested it)
        if (m_CallbackFunctions[LeftMouseReleased].empty() == false)
        {
            m_Callback.trigger = LeftMouseReleased;
            m_Callback.checked = m_Checked;
            m_Callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_Callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }

        // Check if we clicked on the radio button (not just mouse release)
        if (m_MouseDown == true)
        {
            // Check the radio button
            check();

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[LeftMouseClicked].empty() == false)
            {
                m_Callback.trigger = LeftMouseClicked;
                m_Callback.checked = m_Checked;
                m_Callback.mouse.x = static_cast<int>(x - getPosition().x);
                m_Callback.mouse.y = static_cast<int>(y - getPosition().y);
                addCallback();
            }

            m_MouseDown = false;
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
            if (m_CallbackFunctions[SpaceKeyPressed].empty() == false)
            {
                m_Callback.trigger = SpaceKeyPressed;
                m_Callback.checked = m_Checked;
                addCallback();
            }
        }
        else if (event.code == sf::Keyboard::Return)
        {
            // Check the radio button
            check();

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_Callback.trigger = ReturnKeyPressed;
                m_Callback.checked = m_Checked;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::widgetFocused()
    {
        // We can't be focused when we don't have a focus image
        if ((m_WidgetPhase & WidgetPhase_Focused) == 0)
            unfocus();
        else
            Widget::widgetFocused();
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
            setTextSize(atoi(value.c_str()));
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
            value = to_string(m_TextureUnchecked.sprite.getGlobalBounds().width);
        else if (property == "height")
            value = to_string(m_TextureUnchecked.sprite.getGlobalBounds().height);
        else if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "checked")
            value = m_Checked ? "true" : "false";
        else if (property == "text")
            value = getText().toAnsiString();
        else if (property == "textcolor")
            value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = to_string(getTextSize());
        else if (property == "allowtextclick")
            value = m_AllowTextClick ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(Checked) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(Checked).size() == 1) && (m_CallbackFunctions.at(Checked).front() == nullptr))
                callbacks.push_back("Checked");
            if ((m_CallbackFunctions.find(Unchecked) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(Unchecked).size() == 1) && (m_CallbackFunctions.at(Unchecked).front() == nullptr))
                callbacks.push_back("Unchecked");
            if ((m_CallbackFunctions.find(SpaceKeyPressed) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(SpaceKeyPressed).size() == 1) && (m_CallbackFunctions.at(SpaceKeyPressed).front() == nullptr))
                callbacks.push_back("SpaceKeyPressed");
            if ((m_CallbackFunctions.find(ReturnKeyPressed) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(ReturnKeyPressed).size() == 1) && (m_CallbackFunctions.at(ReturnKeyPressed).front() == nullptr))
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
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_Checked)
            target.draw(m_TextureChecked, states);
        else
            target.draw(m_TextureUnchecked, states);

        // When the radio button is focused then draw an extra image
        if ((m_Focused) && (m_WidgetPhase & WidgetPhase_Focused))
            target.draw(m_TextureFocused, states);

        // When the mouse is on top of the radio button then draw an extra image
        if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
            target.draw(m_TextureHover, states);

        // Draw the text
        target.draw(m_Text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
