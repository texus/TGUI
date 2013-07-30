/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets.hpp>
#include <TGUI/ClickableWidget.hpp>
#include <TGUI/Checkbox.hpp>
#include <TGUI/RadioButton.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton()
    {
        m_Callback.widgetType = Type_RadioButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton(const RadioButton& copy) :
    Checkbox(copy)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton& RadioButton::operator= (const RadioButton& right)
    {
        if (this != &right)
        {
            this->Checkbox::operator=(right);
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
        m_LoadedConfigFile = configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

         // If the radio button was loaded before then remove the old textures
        if (m_TextureUnchecked.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureUnchecked);
        if (m_TextureChecked.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureChecked);
        if (m_TextureHover.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureHover);
        if (m_TextureFocused.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureFocused);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(configFileFilename))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + configFileFilename + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("RadioButton", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + configFileFilename + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

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
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CheckedImage in section RadioButton in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "uncheckedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureUnchecked))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for UncheckedImage in section RadioButton in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section RadioButton in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section RadioButton in " + configFileFilename + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section RadioButton in " + configFileFilename + ".");
        }

        // Make sure the required texture was loaded
        if ((m_TextureChecked.data != nullptr) && (m_TextureUnchecked.data != nullptr))
        {
            m_Loaded = true;
            m_Size = Vector2f(m_TextureChecked.getSize());
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the radio button. Is the RadioButton section in " + configFileFilename + " complete?");
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

    void RadioButton::check()
    {
        // Tell our parent that all the radio buttons should be unchecked
        m_Parent->uncheckRadioButtons();

        // Check this radio button
        Checkbox::check();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::uncheck()
    {
        // The radio button can't be unchecked, so we override the original function with an empty one.
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::forceUncheck()
    {
        Checkbox::uncheck();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
