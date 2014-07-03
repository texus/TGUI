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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton()
    {
        m_callback.widgetType = Type_RadioButton;

        m_text.setTextColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::create(const std::string& configFileFilename)
    {
        auto radioButton = std::make_shared<RadioButton>();

        radioButton->m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Open the config file
        ConfigFile configFile{radioButton->m_loadedConfigFile, "RadioButton"};

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = radioButton->m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = radioButton->m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "textcolor")
                radioButton->m_text.setTextColor(configFile.readColor(it));
            else if (it->first == "checkedimage")
                configFile.readTexture(it, configFileFolder, radioButton->m_textureChecked);
            else if (it->first == "uncheckedimage")
                configFile.readTexture(it, configFileFolder, radioButton->m_textureUnchecked);
            else if (it->first == "hoverimage")
                configFile.readTexture(it, configFileFolder, radioButton->m_textureHover);
            else if (it->first == "focusedimage")
                configFile.readTexture(it, configFileFolder, radioButton->m_textureFocused);
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section RadioButton in " + radioButton->m_loadedConfigFile + "."};
        }

        // Make sure the required texture was loaded
        if ((radioButton->m_textureChecked.getData() == nullptr) || (radioButton->m_textureUnchecked.getData() == nullptr))
            throw Exception{"Not all needed images were loaded for the radio button. Is the RadioButton section in " + radioButton->m_loadedConfigFile + " complete?"};

        radioButton->setSize(radioButton->m_textureUnchecked.getImageSize());

        // The widget can only be focused when there is an image available for this phase
        if (radioButton->m_textureFocused.getData() != nullptr)
            radioButton->m_allowFocus = true;

        return radioButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setPosition(const Layout& position)
    {
        ClickableWidget::setPosition(position);

        m_textureUnchecked.setPosition(position.getValue());
        m_textureChecked.setPosition(position.getValue().x, position.getValue().y + getSize().y - m_textureChecked.getSize().y);
        m_textureFocused.setPosition(position.getValue());
        m_textureHover.setPosition(position.getValue());

        m_text.setPosition(position.getValue().x + getSize().x * 11.0f / 10.0f,
                           position.getValue().y + ((getSize().y - m_text.getSize().y) / 2.0f));
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(const Layout& size)
    {
        Widget::setSize(size);

        // If the text is auto sized then recalculate the size
        if (m_textSize == 0)
            setText(m_text.getText());

        m_textureUnchecked.setSize(getSize());
        m_textureFocused.setSize(getSize());
        m_textureHover.setSize(getSize());
        m_textureChecked.setSize({getSize().x + ((m_textureChecked.getImageSize().x - m_textureUnchecked.getImageSize().x) * (getSize().x / m_textureUnchecked.getImageSize().x)),
                                  getSize().y + ((m_textureChecked.getImageSize().y - m_textureUnchecked.getImageSize().y) * (getSize().y / m_textureUnchecked.getImageSize().y))});

        // Reposition the text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getFullSize() const
    {
        if (m_text.getText().isEmpty())
            return getSize();
        else
            return {(getSize().x * 11.0f / 10.0f) + m_text.getSize().x, getSize().y};
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

    void RadioButton::setText(const sf::String& text)
    {
        // Set the new text
        m_text.setText(text);

        // Set the text size
        if (m_textSize == 0)
            m_text.setTextSize(static_cast<unsigned int>(getSize().y * 0.75f));
        else
            m_text.setTextSize(m_textSize);

        // Reposition the text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextFont(const sf::Font& font)
    {
        m_text.setTextFont(font);

        // Recalculate the text position and size
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_text.getText());
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
        // Check if the mouse is on top of the image
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
            return true;
        else
        {
            // Check if the mouse is on top of the text
            if (m_allowTextClick)
            {
                if (sf::FloatRect(0, 0, m_text.getSize().x, m_text.getSize().y).contains(x - (getPosition().x + ((getSize().x * 11.0f / 10.0f))), y - getPosition().y - ((getSize().y - m_text.getSize().y) / 2.0f)))
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
/**
    void RadioButton::setProperty(std::string property, const std::string& value)
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
                throw Exception{"Failed to parse 'Checked' property."};
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
                throw Exception{"Failed to parse 'AllowTextClick' property."};
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
            ClickableWidget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "width")
            value = tgui::to_string(getSize().x);
        else if (property == "height")
            value = tgui::to_string(getSize().y);
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
            ClickableWidget::getProperty(property, value);
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
*/
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
