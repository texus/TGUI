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
#include <TGUI/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button() :
    m_SplitImage        (false),
    m_SeparateHoverImage(false),
    m_TextSize          (0)
    {
        m_Callback.widgetType = Type_Button;
        m_Text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button(const Button& copy) :
    ClickableWidget     (copy),
    m_LoadedConfigFile  (copy.m_LoadedConfigFile),
    m_SplitImage        (copy.m_SplitImage),
    m_SeparateHoverImage(copy.m_SeparateHoverImage),
    m_Text              (copy.m_Text),
    m_TextSize          (copy.m_TextSize)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_L, m_TextureNormal_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_M, m_TextureNormal_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_R, m_TextureNormal_R);
        TGUI_TextureManager.copyTexture(copy.m_TextureHover_L, m_TextureHover_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureHover_M, m_TextureHover_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureHover_R, m_TextureHover_R);
        TGUI_TextureManager.copyTexture(copy.m_TextureDown_L, m_TextureDown_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureDown_M, m_TextureDown_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureDown_R, m_TextureDown_R);
        TGUI_TextureManager.copyTexture(copy.m_TextureFocused_L, m_TextureFocused_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureFocused_M, m_TextureFocused_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureFocused_R, m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::~Button()
    {
        if (m_TextureNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_R);

        if (m_TextureDown_L.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureDown_L);
        if (m_TextureDown_M.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureDown_M);
        if (m_TextureDown_R.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureDown_R);

        if (m_TextureFocused_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button& Button::operator= (const Button& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Button temp(right);
            this->ClickableWidget::operator=(right);

            std::swap(m_LoadedConfigFile,   temp.m_LoadedConfigFile);
            std::swap(m_TextureNormal_L,    temp.m_TextureNormal_L);
            std::swap(m_TextureNormal_M,    temp.m_TextureNormal_M);
            std::swap(m_TextureNormal_R,    temp.m_TextureNormal_R);
            std::swap(m_TextureHover_L,     temp.m_TextureHover_L);
            std::swap(m_TextureHover_M,     temp.m_TextureHover_M);
            std::swap(m_TextureHover_R,     temp.m_TextureHover_R);
            std::swap(m_TextureDown_L,      temp.m_TextureDown_L);
            std::swap(m_TextureDown_M,      temp.m_TextureDown_M);
            std::swap(m_TextureDown_R,      temp.m_TextureDown_R);
            std::swap(m_TextureFocused_L,   temp.m_TextureFocused_L);
            std::swap(m_TextureFocused_M,   temp.m_TextureFocused_M);
            std::swap(m_TextureFocused_R,   temp.m_TextureFocused_R);
            std::swap(m_SplitImage,         temp.m_SplitImage);
            std::swap(m_SeparateHoverImage, temp.m_SeparateHoverImage);
            std::swap(m_Text,               temp.m_Text);
            std::swap(m_TextSize,           temp.m_TextSize);
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
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If the button was loaded before then remove the old textures first
        if (m_TextureNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_R);
        if (m_TextureDown_L.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureDown_L);
        if (m_TextureDown_M.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureDown_M);
        if (m_TextureDown_R.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureDown_R);
        if (m_TextureFocused_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_R);

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
        if (!configFile.read("Button", properties, values))
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

            if (property == "separatehoverimage")
            {
                m_SeparateHoverImage = configFile.readBool(value, false);
            }
            else if (property == "textcolor")
            {
                m_Text.setColor(configFile.readColor(value));
            }
            else if (property == "normalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_L in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_M in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "normalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_R in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_L in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_M in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_R in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureDown_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_L in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_M in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureDown_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_R in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_L in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_M in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_R in section Button in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Button in " + m_LoadedConfigFile + ".");
        }

        // Check if the image is split
        if (m_SplitImage)
        {
            // Make sure the required textures were loaded
            if ((m_TextureNormal_L.data != nullptr) && (m_TextureNormal_M.data != nullptr) && (m_TextureNormal_R.data != nullptr))
            {
                m_Loaded = true;
                setSize(static_cast<float>(m_TextureNormal_L.getSize().x + m_TextureNormal_M.getSize().x + m_TextureNormal_R.getSize().x),
                        static_cast<float>(m_TextureNormal_M.getSize().y));

                m_TextureNormal_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the button. Is the Button section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureFocused_L.data != nullptr) && (m_TextureFocused_M.data != nullptr) && (m_TextureFocused_R.data != nullptr))
            {
                m_AllowFocus = true;
                m_WidgetPhase |= WidgetPhase_Focused;

                m_TextureFocused_M.data->texture.setRepeated(true);
            }
            if ((m_TextureHover_L.data != nullptr) && (m_TextureHover_M.data != nullptr) && (m_TextureHover_R.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Hover;

                m_TextureHover_M.data->texture.setRepeated(true);
            }
            if ((m_TextureDown_L.data != nullptr) && (m_TextureDown_M.data != nullptr) && (m_TextureDown_R.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_MouseDown;

                m_TextureDown_M.data->texture.setRepeated(true);
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_TextureNormal_M.data != nullptr)
            {
                m_Loaded = true;
                setSize(static_cast<float>(m_TextureNormal_M.getSize().x), static_cast<float>(m_TextureNormal_M.getSize().y));
            }
            else
            {
                TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the Button section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if (m_TextureFocused_M.data != nullptr)
            {
                m_AllowFocus = true;
                m_WidgetPhase |= WidgetPhase_Focused;
            }
            if (m_TextureHover_M.data != nullptr)
            {
                m_WidgetPhase |= WidgetPhase_Hover;
            }
            if (m_TextureDown_M.data != nullptr)
            {
                m_WidgetPhase |= WidgetPhase_MouseDown;
            }
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Button::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setPosition(float x, float y)
    {
        Transformable::setPosition(x, y);

        if (m_SplitImage)
        {
            m_TextureDown_L.sprite.setPosition(x, y);
            m_TextureHover_L.sprite.setPosition(x, y);
            m_TextureNormal_L.sprite.setPosition(x, y);
            m_TextureFocused_L.sprite.setPosition(x, y);

            // Check if the middle image may be drawn
            if ((m_TextureNormal_M.sprite.getScale().y * (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x)) < m_Size.x)
            {
                float scalingY = m_Size.y / m_TextureNormal_M.getSize().y;

                m_TextureDown_M.sprite.setPosition(x + (m_TextureDown_L.getSize().x * m_TextureDown_L.sprite.getScale().x), y);
                m_TextureHover_M.sprite.setPosition(x + (m_TextureHover_L.getSize().x * m_TextureHover_L.sprite.getScale().x), y);
                m_TextureNormal_M.sprite.setPosition(x + (m_TextureNormal_L.getSize().x * m_TextureNormal_L.sprite.getScale().x), y);
                m_TextureFocused_M.sprite.setPosition(x + (m_TextureFocused_L.getSize().x * m_TextureFocused_L.sprite.getScale().x), y);

                m_TextureDown_R.sprite.setPosition(m_TextureDown_M.sprite.getPosition().x + (m_TextureDown_M.sprite.getTextureRect().width * scalingY), y);
                m_TextureHover_R.sprite.setPosition(m_TextureHover_M.sprite.getPosition().x + (m_TextureHover_M.sprite.getTextureRect().width * scalingY), y);
                m_TextureNormal_R.sprite.setPosition(m_TextureNormal_M.sprite.getPosition().x + (m_TextureNormal_M.sprite.getTextureRect().width * scalingY), y);
                m_TextureFocused_R.sprite.setPosition(m_TextureFocused_M.sprite.getPosition().x + (m_TextureFocused_M.sprite.getTextureRect().width * scalingY), y);
            }
            else // The middle image isn't drawn
            {
                m_TextureDown_R.sprite.setPosition(x + (m_TextureDown_L.getSize().x * m_TextureDown_L.sprite.getScale().x), y);
                m_TextureHover_R.sprite.setPosition(x + (m_TextureHover_L.getSize().x * m_TextureHover_L.sprite.getScale().x), y);
                m_TextureNormal_R.sprite.setPosition(x + (m_TextureNormal_L.getSize().x * m_TextureNormal_L.sprite.getScale().x), y);
                m_TextureFocused_R.sprite.setPosition(x + (m_TextureFocused_L.getSize().x * m_TextureFocused_L.sprite.getScale().x), y);
            }
        }
        else // The images aren't split
        {
            m_TextureDown_M.sprite.setPosition(x, y);
            m_TextureHover_M.sprite.setPosition(x, y);
            m_TextureNormal_M.sprite.setPosition(x, y);
            m_TextureFocused_M.sprite.setPosition(x, y);
        }

        // Set the position of the text
        m_Text.setPosition(std::floor(x + (m_Size.x - m_Text.getLocalBounds().width) * 0.5f -  m_Text.getLocalBounds().left),
                           std::floor(y + (m_Size.y - m_Text.getLocalBounds().height) * 0.5f -  m_Text.getLocalBounds().top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(float width, float height)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new size of the button
        m_Size.x = width;
        m_Size.y = height;

        // A negative size is not allowed for this widget
        if (m_Size.x < 0) m_Size.x = -m_Size.x;
        if (m_Size.y < 0) m_Size.y = -m_Size.y;

        // Recalculate the text size when auto sizing
        if (m_TextSize == 0)
            setText(m_Text.getString());

        // Drawing the button image will be different when the image is split
        if (m_SplitImage)
        {
            float scalingY = m_Size.y / m_TextureNormal_M.getSize().y;
            float minimumWidth = (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * scalingY;

            if (m_Size.x < minimumWidth)
                m_Size.x = minimumWidth;

            m_TextureDown_L.sprite.setScale(scalingY, scalingY);
            m_TextureHover_L.sprite.setScale(scalingY, scalingY);
            m_TextureNormal_L.sprite.setScale(scalingY, scalingY);
            m_TextureFocused_L.sprite.setScale(scalingY, scalingY);

            m_TextureDown_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scalingY), m_TextureDown_M.getSize().y));
            m_TextureHover_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scalingY), m_TextureHover_M.getSize().y));
            m_TextureNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scalingY), m_TextureNormal_M.getSize().y));
            m_TextureFocused_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scalingY), m_TextureFocused_M.getSize().y));

            m_TextureDown_M.sprite.setScale(scalingY, scalingY);
            m_TextureHover_M.sprite.setScale(scalingY, scalingY);
            m_TextureNormal_M.sprite.setScale(scalingY, scalingY);
            m_TextureFocused_M.sprite.setScale(scalingY, scalingY);

            m_TextureDown_R.sprite.setScale(scalingY, scalingY);
            m_TextureHover_R.sprite.setScale(scalingY, scalingY);
            m_TextureNormal_R.sprite.setScale(scalingY, scalingY);
            m_TextureFocused_R.sprite.setScale(scalingY, scalingY);
        }
        else // The image is not split
        {
            m_TextureDown_M.sprite.setScale(m_Size.x / m_TextureDown_M.getSize().x, m_Size.y / m_TextureDown_M.getSize().y);
            m_TextureHover_M.sprite.setScale(m_Size.x / m_TextureHover_M.getSize().x, m_Size.y / m_TextureHover_M.getSize().y);
            m_TextureNormal_M.sprite.setScale(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);
            m_TextureFocused_M.sprite.setScale(m_Size.x / m_TextureFocused_M.getSize().x, m_Size.y / m_TextureFocused_M.getSize().y);
        }

        // Recalculate the position of the images
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const sf::String& text)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new text
        m_Text.setString(text);
        m_Callback.text = text;

        // Check if the text is auto sized
        if (m_TextSize == 0)
        {
            // Calculate a possible text size
            float size = m_Size.y * 0.75f;
            m_Text.setCharacterSize(static_cast<unsigned int>(size));

            // Make sure that the text isn't too width
            if (m_Text.getGlobalBounds().width > (m_Size.x * 0.8f))
                m_Text.setCharacterSize(static_cast<unsigned int>(size * m_Size.x * 0.8f / m_Text.getGlobalBounds().width));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_Text.setCharacterSize(m_TextSize);
        }

        // Set the position of the text
        m_Text.setPosition(std::floor(getPosition().x + (m_Size.x - m_Text.getLocalBounds().width) * 0.5f -  m_Text.getLocalBounds().left),
                           std::floor(getPosition().y + (m_Size.y - m_Text.getLocalBounds().height) * 0.5f -  m_Text.getLocalBounds().top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Button::getText() const
    {
        return m_Text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);

        // Call setText to reposition the text
        setText(m_Text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Button::getTextFont() const
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextColor(const sf::Color& color)
    {
        m_Text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Button::getTextColor() const
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextSize(unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Button::getTextSize() const
    {
        return m_Text.getCharacterSize();
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        if (m_SplitImage)
        {
            m_TextureNormal_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureHover_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureDown_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureFocused_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));

            m_TextureNormal_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureHover_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureDown_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureFocused_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        }

        m_TextureNormal_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureHover_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureDown_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureFocused_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if the space key or the return key was pressed
        if (event.code == sf::Keyboard::Space)
        {
            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[SpaceKeyPressed].empty() == false)
            {
                m_Callback.trigger = SpaceKeyPressed;
                addCallback();
            }
        }
        else if (event.code == sf::Keyboard::Return)
        {
            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_Callback.trigger = ReturnKeyPressed;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::widgetFocused()
    {
        // We can't be focused when we don't have a focus image
        if ((m_WidgetPhase & WidgetPhase_Focused) == 0)
            unfocus();
        else
            Widget::widgetFocused();
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
            setTextSize(atoi(value.c_str()));
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
            value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = to_string(getTextSize());
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

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
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_SplitImage)
        {
            if (m_SeparateHoverImage)
            {
                if ((m_MouseDown) && (m_MouseHover) && (m_WidgetPhase & WidgetPhase_MouseDown))
                {
                    target.draw(m_TextureDown_L, states);
                    target.draw(m_TextureDown_M, states);
                    target.draw(m_TextureDown_R, states);
                }
                else if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_TextureHover_L, states);
                    target.draw(m_TextureHover_M, states);
                    target.draw(m_TextureHover_R, states);
                }
                else
                {
                    target.draw(m_TextureNormal_L, states);
                    target.draw(m_TextureNormal_M, states);
                    target.draw(m_TextureNormal_R, states);
                }
            }
            else // The hover image is drawn on top of the normal one
            {
                if ((m_MouseDown) && (m_MouseHover) && (m_WidgetPhase & WidgetPhase_MouseDown))
                {
                    target.draw(m_TextureDown_L, states);
                    target.draw(m_TextureDown_M, states);
                    target.draw(m_TextureDown_R, states);
                }
                else
                {
                    target.draw(m_TextureNormal_L, states);
                    target.draw(m_TextureNormal_M, states);
                    target.draw(m_TextureNormal_R, states);
                }

                // When the mouse is on top of the button then draw an extra image
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_TextureHover_L, states);
                    target.draw(m_TextureHover_M, states);
                    target.draw(m_TextureHover_R, states);
                }
            }

            // When the button is focused then draw an extra image
            if ((m_Focused) && (m_WidgetPhase & WidgetPhase_Focused))
            {
                target.draw(m_TextureFocused_L, states);
                target.draw(m_TextureFocused_M, states);
                target.draw(m_TextureFocused_R, states);
            }
        }
        else // The images aren't split
        {
            if (m_SeparateHoverImage)
            {
                if ((m_MouseDown) && (m_MouseHover) && (m_WidgetPhase & WidgetPhase_MouseDown))
                {
                    target.draw(m_TextureDown_M, states);
                }
                else if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_TextureHover_M, states);
                }
                else
                {
                    target.draw(m_TextureNormal_M, states);
                }
            }
            else // The hover image is drawn on top of the normal one
            {
                if ((m_MouseDown) && (m_MouseHover) && (m_WidgetPhase & WidgetPhase_MouseDown))
                {
                    target.draw(m_TextureDown_M, states);
                }
                else
                {
                    target.draw(m_TextureNormal_M, states);
                }

                // When the mouse is on top of the button then draw an extra image
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_TextureHover_M, states);
                }
            }

            // When the button is focused then draw an extra image
            if ((m_Focused) && (m_WidgetPhase & WidgetPhase_Focused))
            {
                target.draw(m_TextureFocused_M, states);
            }
        }

        // If the button has a text then also draw the text
        target.draw(m_Text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
