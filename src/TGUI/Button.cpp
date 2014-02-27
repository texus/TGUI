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
    m_splitImage        (false),
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
    m_splitImage        (copy.m_splitImage),
    m_separateHoverImage(copy.m_separateHoverImage),
    m_text              (copy.m_text),
    m_textSize          (copy.m_textSize)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureNormal_L, m_textureNormal_L);
        TGUI_TextureManager.copyTexture(copy.m_textureNormal_M, m_textureNormal_M);
        TGUI_TextureManager.copyTexture(copy.m_textureNormal_R, m_textureNormal_R);
        TGUI_TextureManager.copyTexture(copy.m_textureHover_L, m_textureHover_L);
        TGUI_TextureManager.copyTexture(copy.m_textureHover_M, m_textureHover_M);
        TGUI_TextureManager.copyTexture(copy.m_textureHover_R, m_textureHover_R);
        TGUI_TextureManager.copyTexture(copy.m_textureDown_L, m_textureDown_L);
        TGUI_TextureManager.copyTexture(copy.m_textureDown_M, m_textureDown_M);
        TGUI_TextureManager.copyTexture(copy.m_textureDown_R, m_textureDown_R);
        TGUI_TextureManager.copyTexture(copy.m_textureFocused_L, m_textureFocused_L);
        TGUI_TextureManager.copyTexture(copy.m_textureFocused_M, m_textureFocused_M);
        TGUI_TextureManager.copyTexture(copy.m_textureFocused_R, m_textureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::~Button()
    {
        if (m_textureNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal_L);
        if (m_textureNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal_M);
        if (m_textureNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal_R);

        if (m_textureHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover_L);
        if (m_textureHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover_M);
        if (m_textureHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover_R);

        if (m_textureDown_L.data != nullptr)     TGUI_TextureManager.removeTexture(m_textureDown_L);
        if (m_textureDown_M.data != nullptr)     TGUI_TextureManager.removeTexture(m_textureDown_M);
        if (m_textureDown_R.data != nullptr)     TGUI_TextureManager.removeTexture(m_textureDown_R);

        if (m_textureFocused_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused_L);
        if (m_textureFocused_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused_M);
        if (m_textureFocused_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused_R);
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
            std::swap(m_textureNormal_L,    temp.m_textureNormal_L);
            std::swap(m_textureNormal_M,    temp.m_textureNormal_M);
            std::swap(m_textureNormal_R,    temp.m_textureNormal_R);
            std::swap(m_textureHover_L,     temp.m_textureHover_L);
            std::swap(m_textureHover_M,     temp.m_textureHover_M);
            std::swap(m_textureHover_R,     temp.m_textureHover_R);
            std::swap(m_textureDown_L,      temp.m_textureDown_L);
            std::swap(m_textureDown_M,      temp.m_textureDown_M);
            std::swap(m_textureDown_R,      temp.m_textureDown_R);
            std::swap(m_textureFocused_L,   temp.m_textureFocused_L);
            std::swap(m_textureFocused_M,   temp.m_textureFocused_M);
            std::swap(m_textureFocused_R,   temp.m_textureFocused_R);
            std::swap(m_splitImage,         temp.m_splitImage);
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
        if (m_textureNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal_L);
        if (m_textureNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal_M);
        if (m_textureNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal_R);
        if (m_textureHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover_L);
        if (m_textureHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover_M);
        if (m_textureHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover_R);
        if (m_textureDown_L.data != nullptr)     TGUI_TextureManager.removeTexture(m_textureDown_L);
        if (m_textureDown_M.data != nullptr)     TGUI_TextureManager.removeTexture(m_textureDown_M);
        if (m_textureDown_R.data != nullptr)     TGUI_TextureManager.removeTexture(m_textureDown_R);
        if (m_textureFocused_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused_L);
        if (m_textureFocused_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused_M);
        if (m_textureFocused_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused_R);

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
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = false;
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_L in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_M in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = true;
            }
            else if (property == "normalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_R in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_L in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_M in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_R in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureDown_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_L in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_M in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "downimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureDown_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_R in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFocused_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_L in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_M in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFocused_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_R in section Button in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Button in " + m_loadedConfigFile + ".");
        }

        // Check if the image is split
        if (m_splitImage)
        {
            // Make sure the required textures were loaded
            if ((m_textureNormal_L.data != nullptr) && (m_textureNormal_M.data != nullptr) && (m_textureNormal_R.data != nullptr))
            {
                m_loaded = true;
                setSize(static_cast<float>(m_textureNormal_L.getSize().x + m_textureNormal_M.getSize().x + m_textureNormal_R.getSize().x),
                        static_cast<float>(m_textureNormal_M.getSize().y));

                m_textureNormal_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the button. Is the Button section in " + m_loadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_textureFocused_L.data != nullptr) && (m_textureFocused_M.data != nullptr) && (m_textureFocused_R.data != nullptr))
            {
                m_allowFocus = true;
                m_widgetPhase |= WidgetPhase_Focused;

                m_textureFocused_M.data->texture.setRepeated(true);
            }
            if ((m_textureHover_L.data != nullptr) && (m_textureHover_M.data != nullptr) && (m_textureHover_R.data != nullptr))
            {
                m_widgetPhase |= WidgetPhase_Hover;

                m_textureHover_M.data->texture.setRepeated(true);
            }
            if ((m_textureDown_L.data != nullptr) && (m_textureDown_M.data != nullptr) && (m_textureDown_R.data != nullptr))
            {
                m_widgetPhase |= WidgetPhase_MouseDown;

                m_textureDown_M.data->texture.setRepeated(true);
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_textureNormal_M.data != nullptr)
            {
                m_loaded = true;
                setSize(static_cast<float>(m_textureNormal_M.getSize().x), static_cast<float>(m_textureNormal_M.getSize().y));
            }
            else
            {
                TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the Button section in " + m_loadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if (m_textureFocused_M.data != nullptr)
            {
                m_allowFocus = true;
                m_widgetPhase |= WidgetPhase_Focused;
            }
            if (m_textureHover_M.data != nullptr)
            {
                m_widgetPhase |= WidgetPhase_Hover;
            }
            if (m_textureDown_M.data != nullptr)
            {
                m_widgetPhase |= WidgetPhase_MouseDown;
            }
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

        if (m_splitImage)
        {
            m_textureDown_L.sprite.setPosition(x, y);
            m_textureHover_L.sprite.setPosition(x, y);
            m_textureNormal_L.sprite.setPosition(x, y);
            m_textureFocused_L.sprite.setPosition(x, y);

            // Check if the middle image may be drawn
            if ((m_textureNormal_M.sprite.getScale().y * (m_textureNormal_L.getSize().x + m_textureNormal_R.getSize().x)) < m_size.x)
            {
                float scalingY = m_size.y / m_textureNormal_M.getSize().y;

                m_textureDown_M.sprite.setPosition(x + (m_textureDown_L.getSize().x * m_textureDown_L.sprite.getScale().x), y);
                m_textureHover_M.sprite.setPosition(x + (m_textureHover_L.getSize().x * m_textureHover_L.sprite.getScale().x), y);
                m_textureNormal_M.sprite.setPosition(x + (m_textureNormal_L.getSize().x * m_textureNormal_L.sprite.getScale().x), y);
                m_textureFocused_M.sprite.setPosition(x + (m_textureFocused_L.getSize().x * m_textureFocused_L.sprite.getScale().x), y);

                m_textureDown_R.sprite.setPosition(m_textureDown_M.sprite.getPosition().x + (m_textureDown_M.sprite.getTextureRect().width * scalingY), y);
                m_textureHover_R.sprite.setPosition(m_textureHover_M.sprite.getPosition().x + (m_textureHover_M.sprite.getTextureRect().width * scalingY), y);
                m_textureNormal_R.sprite.setPosition(m_textureNormal_M.sprite.getPosition().x + (m_textureNormal_M.sprite.getTextureRect().width * scalingY), y);
                m_textureFocused_R.sprite.setPosition(m_textureFocused_M.sprite.getPosition().x + (m_textureFocused_M.sprite.getTextureRect().width * scalingY), y);
            }
            else // The middle image isn't drawn
            {
                m_textureDown_R.sprite.setPosition(x + (m_textureDown_L.getSize().x * m_textureDown_L.sprite.getScale().x), y);
                m_textureHover_R.sprite.setPosition(x + (m_textureHover_L.getSize().x * m_textureHover_L.sprite.getScale().x), y);
                m_textureNormal_R.sprite.setPosition(x + (m_textureNormal_L.getSize().x * m_textureNormal_L.sprite.getScale().x), y);
                m_textureFocused_R.sprite.setPosition(x + (m_textureFocused_L.getSize().x * m_textureFocused_L.sprite.getScale().x), y);
            }
        }
        else // The images aren't split
        {
            m_textureDown_M.sprite.setPosition(x, y);
            m_textureHover_M.sprite.setPosition(x, y);
            m_textureNormal_M.sprite.setPosition(x, y);
            m_textureFocused_M.sprite.setPosition(x, y);
        }

        // Set the position of the text
        m_text.setPosition(std::floor(x + (m_size.x - m_text.getLocalBounds().width) * 0.5f -  m_text.getLocalBounds().left),
                           std::floor(y + (m_size.y - m_text.getLocalBounds().height) * 0.5f -  m_text.getLocalBounds().top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(float width, float height)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Set the new size of the button
        m_size.x = width;
        m_size.y = height;

        // A negative size is not allowed for this widget
        if (m_size.x < 0) m_size.x = -m_size.x;
        if (m_size.y < 0) m_size.y = -m_size.y;

        // Recalculate the text size when auto sizing
        if (m_textSize == 0)
            setText(m_text.getString());

        // Drawing the button image will be different when the image is split
        if (m_splitImage)
        {
            float scalingY = m_size.y / m_textureNormal_M.getSize().y;
            float minimumWidth = (m_textureNormal_L.getSize().x + m_textureNormal_R.getSize().x) * scalingY;

            if (m_size.x < minimumWidth)
                m_size.x = minimumWidth;

            m_textureDown_L.sprite.setScale(scalingY, scalingY);
            m_textureHover_L.sprite.setScale(scalingY, scalingY);
            m_textureNormal_L.sprite.setScale(scalingY, scalingY);
            m_textureFocused_L.sprite.setScale(scalingY, scalingY);

            m_textureDown_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_size.x - minimumWidth) / scalingY), m_textureDown_M.getSize().y));
            m_textureHover_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_size.x - minimumWidth) / scalingY), m_textureHover_M.getSize().y));
            m_textureNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_size.x - minimumWidth) / scalingY), m_textureNormal_M.getSize().y));
            m_textureFocused_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_size.x - minimumWidth) / scalingY), m_textureFocused_M.getSize().y));

            m_textureDown_M.sprite.setScale(scalingY, scalingY);
            m_textureHover_M.sprite.setScale(scalingY, scalingY);
            m_textureNormal_M.sprite.setScale(scalingY, scalingY);
            m_textureFocused_M.sprite.setScale(scalingY, scalingY);

            m_textureDown_R.sprite.setScale(scalingY, scalingY);
            m_textureHover_R.sprite.setScale(scalingY, scalingY);
            m_textureNormal_R.sprite.setScale(scalingY, scalingY);
            m_textureFocused_R.sprite.setScale(scalingY, scalingY);
        }
        else // The image is not split
        {
            m_textureDown_M.sprite.setScale(m_size.x / m_textureDown_M.getSize().x, m_size.y / m_textureDown_M.getSize().y);
            m_textureHover_M.sprite.setScale(m_size.x / m_textureHover_M.getSize().x, m_size.y / m_textureHover_M.getSize().y);
            m_textureNormal_M.sprite.setScale(m_size.x / m_textureNormal_M.getSize().x, m_size.y / m_textureNormal_M.getSize().y);
            m_textureFocused_M.sprite.setScale(m_size.x / m_textureFocused_M.getSize().x, m_size.y / m_textureFocused_M.getSize().y);
        }

        // Recalculate the position of the images
        setPosition(getPosition());
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
            float size = m_size.y * 0.85f;
            m_text.setCharacterSize(static_cast<unsigned int>(size));
            m_text.setCharacterSize(static_cast<unsigned int>(m_text.getCharacterSize() - m_text.getLocalBounds().top));

            // Make sure that the text isn't too width
            if (m_text.getGlobalBounds().width > (m_size.x * 0.8f))
            {
                // The text is too width, so make it smaller
                m_text.setCharacterSize(static_cast<unsigned int>(size * m_size.x * 0.8f / m_text.getGlobalBounds().width));
                m_text.setCharacterSize(static_cast<unsigned int>(m_text.getCharacterSize() - m_text.getLocalBounds().top));
            }
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_text.setCharacterSize(m_textSize);
        }

        // Set the position of the text
        m_text.setPosition(std::floor(getPosition().x + (m_size.x - m_text.getLocalBounds().width) * 0.5f -  m_text.getLocalBounds().left),
                           std::floor(getPosition().y + (m_size.y - m_text.getLocalBounds().height) * 0.5f -  m_text.getLocalBounds().top));
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

        if (m_splitImage)
        {
            m_textureNormal_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
            m_textureHover_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
            m_textureDown_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
            m_textureFocused_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));

            m_textureNormal_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
            m_textureHover_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
            m_textureDown_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
            m_textureFocused_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        }

        m_textureNormal_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureHover_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureDown_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureFocused_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
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
        if ((m_widgetPhase & WidgetPhase_Focused) == 0)
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
        if (m_splitImage)
        {
            if (m_separateHoverImage)
            {
                if ((m_mouseDown) && (m_mouseHover) && (m_widgetPhase & WidgetPhase_MouseDown))
                {
                    target.draw(m_textureDown_L, states);
                    target.draw(m_textureDown_M, states);
                    target.draw(m_textureDown_R, states);
                }
                else if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_textureHover_L, states);
                    target.draw(m_textureHover_M, states);
                    target.draw(m_textureHover_R, states);
                }
                else
                {
                    target.draw(m_textureNormal_L, states);
                    target.draw(m_textureNormal_M, states);
                    target.draw(m_textureNormal_R, states);
                }
            }
            else // The hover image is drawn on top of the normal one
            {
                if ((m_mouseDown) && (m_mouseHover) && (m_widgetPhase & WidgetPhase_MouseDown))
                {
                    target.draw(m_textureDown_L, states);
                    target.draw(m_textureDown_M, states);
                    target.draw(m_textureDown_R, states);
                }
                else
                {
                    target.draw(m_textureNormal_L, states);
                    target.draw(m_textureNormal_M, states);
                    target.draw(m_textureNormal_R, states);
                }

                // When the mouse is on top of the button then draw an extra image
                if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_textureHover_L, states);
                    target.draw(m_textureHover_M, states);
                    target.draw(m_textureHover_R, states);
                }
            }

            // When the button is focused then draw an extra image
            if ((m_focused) && (m_widgetPhase & WidgetPhase_Focused))
            {
                target.draw(m_textureFocused_L, states);
                target.draw(m_textureFocused_M, states);
                target.draw(m_textureFocused_R, states);
            }
        }
        else // The images aren't split
        {
            if (m_separateHoverImage)
            {
                if ((m_mouseDown) && (m_mouseHover) && (m_widgetPhase & WidgetPhase_MouseDown))
                {
                    target.draw(m_textureDown_M, states);
                }
                else if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_textureHover_M, states);
                }
                else
                {
                    target.draw(m_textureNormal_M, states);
                }
            }
            else // The hover image is drawn on top of the normal one
            {
                if ((m_mouseDown) && (m_mouseHover) && (m_widgetPhase & WidgetPhase_MouseDown))
                {
                    target.draw(m_textureDown_M, states);
                }
                else
                {
                    target.draw(m_textureNormal_M, states);
                }

                // When the mouse is on top of the button then draw an extra image
                if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_textureHover_M, states);
                }
            }

            // When the button is focused then draw an extra image
            if ((m_focused) && (m_widgetPhase & WidgetPhase_Focused))
            {
                target.draw(m_textureFocused_M, states);
            }
        }

        // If the button has a text then also draw the text
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
