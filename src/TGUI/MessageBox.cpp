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


#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/MessageBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox() :
    m_TextSize(16)
    {
        m_Callback.widgetType = Type_MessageBox;

        add(m_Label, "MessageBoxText");
        m_Label->setTextSize(m_TextSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const MessageBox& messageBoxToCopy) :
    ChildWindow               (messageBoxToCopy),
    m_LoadedConfigFile        (messageBoxToCopy.m_LoadedConfigFile),
    m_ButtonConfigFileFilename(messageBoxToCopy.m_ButtonConfigFileFilename),
    m_TextSize                (messageBoxToCopy.m_TextSize)
    {
        m_Label = copy(messageBoxToCopy.m_Label, "MessageBoxText");

        for (auto it = messageBoxToCopy.m_Buttons.begin(); it != messageBoxToCopy.m_Buttons.end(); ++it)
        {
            tgui::Button::Ptr button = copy(*it);
            button->unbindAllCallback();
            button->bindCallbackEx(&MessageBox::ButtonClickedCallbackFunction, this, Button::LeftMouseClicked | Button::SpaceKeyPressed | Button::ReturnKeyPressed);

            m_Buttons.push_back(button);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::~MessageBox()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (const MessageBox& right)
    {
        if (this != &right)
        {
            MessageBox temp(right);
            this->ChildWindow::operator=(right);

            std::swap(m_LoadedConfigFile,         temp.m_LoadedConfigFile);
            std::swap(m_ButtonConfigFileFilename, temp.m_ButtonConfigFileFilename);
            std::swap(m_Buttons,                  temp.m_Buttons);
            std::swap(m_Label,                    temp.m_Label);
            std::swap(m_TextSize,                 temp.m_TextSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox* MessageBox::clone()
    {
        return new MessageBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MessageBox::load(const std::string& configFileFilename, const std::string& sectionName)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

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
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        bool childWindowPropertyFound = false;
        bool buttonPropertyFound = false;

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "textcolor")
            {
                m_Label->setTextColor(configFile.readColor(value));
            }
            else if (property == "childwindow")
            {
                if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ChildWindow in section MessageBox in " + m_LoadedConfigFile + ".");
                    return false;
                }

                if (!ChildWindow::load(configFileFolder + value.substr(1, value.length()-2)))
                {
                    TGUI_OUTPUT("TGUI error: Failed to load the internal ChildWindow for MessageBox.");
                }
                else
                    childWindowPropertyFound = true;
            }
            else if (property == "button")
            {
                if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for Button in section MessageBox in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_ButtonConfigFileFilename = configFileFolder + value.substr(1, value.length()-2);
                buttonPropertyFound = true;
            }
            else
            {
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section MessageBox in " + m_LoadedConfigFile + ".");
            }
        }

        if (!childWindowPropertyFound)
        {
            TGUI_OUTPUT("TGUI error: Missing a ChildWindow property in section MessageBox in " + m_LoadedConfigFile + ".");
            return false;
        }
        if (!buttonPropertyFound)
        {
            TGUI_OUTPUT("TGUI error: Missing a Button property in section MessageBox in " + m_LoadedConfigFile + ".");
            return false;
        }

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setText(const sf::String& text)
    {
        if (m_Loaded)
        {
            m_Label->setText(text);

            rearrange();
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Failed to set the text. MessageBox was not loaded completely.");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String MessageBox::getText() const
    {
        if (m_Loaded)
            return m_Label->getText();
        else
        {
            TGUI_OUTPUT("TGUI error: Failed to set the text. MessageBox was not loaded completely.");
            return "";
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setTextFont(const sf::Font& font)
    {
        m_Label->setTextFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* MessageBox::getTextFont() const
    {
        return m_Label->getTextFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setTextColor(const sf::Color& color)
    {
        m_Label->setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MessageBox::getTextColor() const
    {
        return m_Label->getTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setTextSize(unsigned int size)
    {
        m_TextSize = size;

        if (m_Loaded)
        {
            m_Label->setTextSize(size);

            for (unsigned int i = 0; i < m_Buttons.size(); ++i)
                m_Buttons[i]->setTextSize(m_TextSize);

            rearrange();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MessageBox::getTextSize() const
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::addButton(const sf::String& caption)
    {
        if (m_Loaded)
        {
            Button::Ptr button(*this);
            button->load(m_ButtonConfigFileFilename);
            button->setTextSize(m_TextSize);
            button->setText(caption);
            button->bindCallbackEx(&MessageBox::ButtonClickedCallbackFunction, this, Button::LeftMouseClicked | Button::SpaceKeyPressed | Button::ReturnKeyPressed);

            m_Buttons.push_back(button);

            rearrange();
        }
        else // MessageBox wasn't loaded
        {
            TGUI_OUTPUT("TGUI error: Could not add a button. MessageBox was not loaded completely.");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MessageBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "text")
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
        else if (property == "buttons")
        {
            removeAllWidgets();
            m_Buttons.clear();
            add(m_Label);

            std::vector<sf::String> buttons;
            decodeList(value, buttons);

            for (auto it = buttons.cbegin(); it != buttons.cend(); ++it)
                addButton(*it);
        }
        else if (property == "callback")
        {
            ChildWindow::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "ButtonClicked") || (*it == "buttonclicked"))
                    bindCallback(ButtonClicked);
            }
        }
        else // The property didn't match
            return ChildWindow::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MessageBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "text")
            value = getText().toAnsiString();
        else if (property == "textcolor")
            value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = to_string(getTextSize());
        else if (property == "buttons")
        {
            std::vector<sf::String> buttons;
            for (auto it = m_Buttons.cbegin(); it != m_Buttons.cend(); ++it)
                buttons.push_back((*it)->getText());

            encodeList(buttons, value);
        }
        else if (property == "callback")
        {
            std::string tempValue;
            ChildWindow::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(ButtonClicked) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(ButtonClicked).size() == 1) && (m_CallbackFunctions.at(ButtonClicked).front() == nullptr))
                callbacks.push_back("ButtonClicked");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            return ChildWindow::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > MessageBox::getPropertyList() const
    {
        auto list = ChildWindow::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("Text", "string"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("Buttons", "string"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rearrange()
    {
        if (!m_Loaded)
            return;

        // Calculate the button size
        float buttonWidth = 5.0f * m_TextSize;
        float buttonHeight = m_TextSize * 10.0f / 8.0f;
        for (unsigned int i = 0; i < m_Buttons.size(); ++i)
        {
            float width = sf::Text(m_Buttons[i]->getText(), *m_Buttons[i]->getTextFont(), m_TextSize).getLocalBounds().width;
            if (buttonWidth < width * 10.0f / 9.0f)
                buttonWidth = width * 10.0f / 9.0f;
        }

        // Calculate the space needed for the buttons
        float distance = buttonHeight * 2.0f / 3.0f;
        float buttonsAreaWidth = distance;
        for (unsigned int i = 0; i < m_Buttons.size(); ++i)
        {
            m_Buttons[i]->setSize(buttonWidth, buttonHeight);
            buttonsAreaWidth += m_Buttons[i]->getSize().x + distance;
        }

        // Calculate the suggested size of the window
        float width = 2*distance + m_Label->getSize().x;
        float height = 3*distance + m_Label->getSize().y + buttonHeight;

        // Make sure the buttons fit inside the message box
        if (buttonsAreaWidth > width)
            width = buttonsAreaWidth;

        // Set the size of the window
        setSize(width, height);

        // Set the text on the correct position
        m_Label->setPosition(distance, distance);

        // Set the buttons on the correct position
        float leftPosition = 0;
        float topPosition = 2*distance + m_Label->getSize().y;
        for (unsigned int i = 0; i < m_Buttons.size(); ++i)
        {
            leftPosition += distance + ((width - buttonsAreaWidth) / (m_Buttons.size()+1));
            m_Buttons[i]->setPosition(leftPosition, topPosition);
            leftPosition += m_Buttons[i]->getSize().x;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::ButtonClickedCallbackFunction(const Callback& callback)
    {
        if (m_CallbackFunctions[ButtonClicked].empty() == false)
        {
            m_Callback.trigger = ButtonClicked;
            m_Callback.text    = static_cast<Button*>(callback.widget)->getText();
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
