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


#include <cmath>

#include <TGUI/Container.hpp>
#include <TGUI/Checkbox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox::Checkbox()
    {
        m_Callback.widgetType = Type_Checkbox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox* Checkbox::clone()
    {
        return new Checkbox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Checkbox::load(const std::string& configFileFilename)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If the checkbox was loaded before then remove the old textures
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
        if (!configFile.read("Checkbox", properties, values))
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
                    TGUI_OUTPUT("TGUI error: Failed to parse value for CheckedImage in section Checkbox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "uncheckedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureUnchecked))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for UncheckedImage in section Checkbox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section Checkbox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section Checkbox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Checkbox in " + m_LoadedConfigFile + ".");
        }

        // Make sure the required texture was loaded
        if ((m_TextureChecked.data != nullptr) && (m_TextureUnchecked.data != nullptr))
        {
            m_Loaded = true;
            setSize(static_cast<float>(m_TextureUnchecked.getSize().x), static_cast<float>(m_TextureUnchecked.getSize().y));
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the checkbox. Is the Checkbox section in " + m_LoadedConfigFile + " complete?");
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

        // When there is no error we will return true
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::check()
    {
        if (m_Checked == false)
        {
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

    void Checkbox::uncheck()
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

    void Checkbox::leftMouseReleased(float x, float y)
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

        // Check if we clicked on the checkbox (not just mouse release)
        if (m_MouseDown == true)
        {
            // Check or uncheck the checkbox
            if (m_Checked)
                uncheck();
            else
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

    void Checkbox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if the space key or the return key was pressed
        if (event.code == sf::Keyboard::Space)
        {
            // Check or uncheck the checkbox
            if (m_Checked)
                uncheck();
            else
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
            // Check or uncheck the checkbox
            if (m_Checked)
                uncheck();
            else
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
