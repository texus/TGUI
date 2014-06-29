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
#include <TGUI/Checkbox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox::Checkbox()
    {
        m_callback.widgetType = Type_Checkbox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox::Ptr Checkbox::create(const std::string& configFileFilename)
    {
        auto checkbox = std::make_shared<Checkbox>();

        checkbox->m_loadedConfigFile = getResourcePath() + configFileFilename;

        // If the checkbox was loaded before then remove the old textures
        if (checkbox->m_textureUnchecked.getData() != nullptr) TGUI_TextureManager.removeTexture(checkbox->m_textureUnchecked);
        if (checkbox->m_textureChecked.getData() != nullptr)   TGUI_TextureManager.removeTexture(checkbox->m_textureChecked);
        if (checkbox->m_textureHover.getData() != nullptr)     TGUI_TextureManager.removeTexture(checkbox->m_textureHover);
        if (checkbox->m_textureFocused.getData() != nullptr)   TGUI_TextureManager.removeTexture(checkbox->m_textureFocused);

        // Open the config file
        ConfigFile configFile{checkbox->m_loadedConfigFile, "Checkbox"};

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = checkbox->m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = checkbox->m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "textcolor")
                checkbox->m_text.setColor(configFile.readColor(it));
            else if (it->first == "checkedimage")
                configFile.readTexture(it, configFileFolder, checkbox->m_textureChecked);
            else if (it->first == "uncheckedimage")
                configFile.readTexture(it, configFileFolder, checkbox->m_textureUnchecked);
            else if (it->first == "hoverimage")
                configFile.readTexture(it, configFileFolder, checkbox->m_textureHover);
            else if (it->first == "focusedimage")
                configFile.readTexture(it, configFileFolder, checkbox->m_textureFocused);
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section Checkbox in " + checkbox->m_loadedConfigFile + "."};
        }

        // Make sure the required texture was loaded
        if ((checkbox->m_textureChecked.getData() == nullptr) || (checkbox->m_textureUnchecked.getData() == nullptr))
            throw Exception{"Not all needed images were loaded for the checkbox. Is the Checkbox section in " + checkbox->m_loadedConfigFile + " complete?"};

        checkbox->setSize(checkbox->m_textureUnchecked.getImageSize());

        // The widget can only be focused when there is an image available for this phase
        if (checkbox->m_textureFocused.getData() != nullptr)
            checkbox->m_allowFocus = true;

        return checkbox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::check()
    {
        if (m_checked == false)
        {
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

    void Checkbox::uncheck()
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

    void Checkbox::leftMouseReleased(float x, float y)
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

        // Check if we clicked on the checkbox (not just mouse release)
        if (m_mouseDown == true)
        {
            // Check or uncheck the checkbox
            if (m_checked)
                uncheck();
            else
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

    void Checkbox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if the space key or the return key was pressed
        if (event.code == sf::Keyboard::Space)
        {
            // Check or uncheck the checkbox
            if (m_checked)
                uncheck();
            else
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
            // Check or uncheck the checkbox
            if (m_checked)
                uncheck();
            else
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
