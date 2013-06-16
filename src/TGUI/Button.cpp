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


#include <TGUI/Objects.hpp>
#include <TGUI/ClickableObject.hpp>
#include <TGUI/Button.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button() :
    m_SplitImage        (false),
    m_SeparateHoverImage(false),
    m_TextSize          (0)
    {
        m_Callback.objectType = Type_Button;
        m_Text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button(const Button& copy) :
    ClickableObject     (copy),
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
        if (m_TextureNormal_L.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureHover_L.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_R);

        if (m_TextureDown_L.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureDown_L);
        if (m_TextureDown_M.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureDown_M);
        if (m_TextureDown_R.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureDown_R);

        if (m_TextureFocused_L.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button& Button::operator= (const Button& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Button temp(right);
            this->ClickableObject::operator=(right);

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
        m_LoadedConfigFile = configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If the button was loaded before then remove the old textures first
        if (m_TextureNormal_L.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureHover_L.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_R);
        if (m_TextureDown_L.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureDown_L);
        if (m_TextureDown_M.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureDown_M);
        if (m_TextureDown_R.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureDown_R);
        if (m_TextureFocused_L.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_R);

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
        if (!configFile.read("Button", properties, values))
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
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section Button in " + configFileFilename + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "downimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "normalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_L in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "normalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_M in section Button in " + configFileFilename + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "normalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_R in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_L in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_M in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_R in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "downimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureDown_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_L in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "downimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureDown_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_M in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "downimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureDown_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for DownImage_R in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_L in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_M in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_R in section Button in " + configFileFilename + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Button in " + configFileFilename + ".");
        }

        // Check if the image is split
        if (m_SplitImage)
        {
            // Make sure the required textures were loaded
            if ((m_TextureNormal_L.data != NULL) && (m_TextureNormal_M.data != NULL) && (m_TextureNormal_R.data != NULL))
            {
                m_Loaded = true;
                setSize(static_cast<float>(m_TextureNormal_L.getSize().x + m_TextureNormal_M.getSize().x + m_TextureNormal_R.getSize().x),
                        static_cast<float>(m_TextureNormal_M.getSize().y));
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the button. Is the Button section in " + configFileFilename + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureFocused_L.data != NULL) && (m_TextureFocused_M.data != NULL) && (m_TextureFocused_R.data != NULL))
            {
                m_AllowFocus = true;
                m_ObjectPhase |= ObjectPhase_Focused;
            }
            if ((m_TextureHover_L.data != NULL) && (m_TextureHover_M.data != NULL) && (m_TextureHover_R.data != NULL))
            {
                m_ObjectPhase |= ObjectPhase_Hover;
            }
            if ((m_TextureDown_L.data != NULL) && (m_TextureDown_M.data != NULL) && (m_TextureDown_R.data != NULL))
            {
                m_ObjectPhase |= ObjectPhase_MouseDown;
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_TextureNormal_M.data != NULL)
            {
                m_Loaded = true;
                setSize(m_TextureNormal_M.getSize().x, m_TextureNormal_M.getSize().y);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the Button section in " + configFileFilename + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if (m_TextureFocused_M.data != NULL)
            {
                m_AllowFocus = true;
                m_ObjectPhase |= ObjectPhase_Focused;
            }
            if (m_TextureHover_M.data != NULL)
            {
                m_ObjectPhase |= ObjectPhase_Hover;
            }
            if (m_TextureDown_M.data != NULL)
            {
                m_ObjectPhase |= ObjectPhase_MouseDown;
            }
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Button::getLoadedConfigFile()
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
                m_TextureDown_M.sprite.setPosition(x + (m_TextureDown_L.getWidth() * m_TextureDown_L.sprite.getScale().x), y);
                m_TextureHover_M.sprite.setPosition(x + (m_TextureHover_L.getWidth() * m_TextureHover_L.sprite.getScale().x), y);
                m_TextureNormal_M.sprite.setPosition(x + (m_TextureNormal_L.getWidth() * m_TextureNormal_L.sprite.getScale().x), y);
                m_TextureFocused_M.sprite.setPosition(x + (m_TextureFocused_L.getWidth() * m_TextureFocused_L.sprite.getScale().x), y);

                m_TextureDown_R.sprite.setPosition(m_TextureDown_M.sprite.getPosition().x + (m_TextureDown_M.getWidth() * m_TextureDown_M.sprite.getScale().x), y);
                m_TextureHover_R.sprite.setPosition(m_TextureHover_M.sprite.getPosition().x + (m_TextureHover_M.getWidth() * m_TextureHover_M.sprite.getScale().x), y);
                m_TextureNormal_R.sprite.setPosition(m_TextureNormal_M.sprite.getPosition().x + (m_TextureNormal_M.getWidth() * m_TextureNormal_M.sprite.getScale().x), y);
                m_TextureFocused_R.sprite.setPosition(m_TextureFocused_M.sprite.getPosition().x + (m_TextureFocused_M.getWidth() * m_TextureFocused_M.sprite.getScale().x), y);
            }
            else // The middle image isn't drawn
            {
                m_TextureDown_R.sprite.setPosition(x + (m_TextureDown_L.getWidth() * m_TextureDown_L.sprite.getScale().x), y);
                m_TextureHover_R.sprite.setPosition(x + (m_TextureHover_L.getWidth() * m_TextureHover_L.sprite.getScale().x), y);
                m_TextureNormal_R.sprite.setPosition(x + (m_TextureNormal_L.getWidth() * m_TextureNormal_L.sprite.getScale().x), y);
                m_TextureFocused_R.sprite.setPosition(x + (m_TextureFocused_L.getWidth() * m_TextureFocused_L.sprite.getScale().x), y);
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

        // Recalculate the text size when auto sizing
        if (m_TextSize == 0)
            setText(m_Text.getString());

        // Drawing the button image will be different when the image is split
        if (m_SplitImage)
        {
            float scalingY = m_Size.y / m_TextureNormal_M.getSize().y;

            m_TextureDown_L.sprite.setScale(scalingY, scalingY);
            m_TextureHover_L.sprite.setScale(scalingY, scalingY);
            m_TextureNormal_L.sprite.setScale(scalingY, scalingY);
            m_TextureFocused_L.sprite.setScale(scalingY, scalingY);

            // Check if the middle image may be drawn
            if (((scalingY) * (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x)) < m_Size.x)
            {
                // Calculate the scale for our middle image
                float scaleX = (m_Size.x / m_TextureNormal_M.getSize().x) -
                               (((m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * (scalingY))
                                / m_TextureNormal_M.getSize().x);

                m_TextureDown_M.sprite.setScale(scaleX, scalingY);
                m_TextureHover_M.sprite.setScale(scaleX, scalingY);
                m_TextureNormal_M.sprite.setScale(scaleX, scalingY);
                m_TextureFocused_M.sprite.setScale(scaleX, scalingY);
            }
            else // The middle image is not drawn
            {
                m_TextureDown_M.sprite.setScale(0, 0);
                m_TextureHover_M.sprite.setScale(0, 0);
                m_TextureNormal_M.sprite.setScale(0, 0);
                m_TextureFocused_M.sprite.setScale(0, 0);
            }

            m_TextureDown_R.sprite.setScale(scalingY, scalingY);
            m_TextureHover_R.sprite.setScale(scalingY, scalingY);
            m_TextureNormal_R.sprite.setScale(scalingY, scalingY);
            m_TextureFocused_R.sprite.setScale(scalingY, scalingY);
        }
        else // The image is not split
        {
            m_TextureDown_M.sprite.setScale(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);
            m_TextureHover_M.sprite.setScale(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);
            m_TextureNormal_M.sprite.setScale(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);
            m_TextureFocused_M.sprite.setScale(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);
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
            float size = m_Size.y * 0.85f;
            m_Text.setCharacterSize(static_cast<unsigned int>(size));
            m_Text.setCharacterSize(static_cast<unsigned int>(m_Text.getCharacterSize() - m_Text.getLocalBounds().top));

            // Make sure that the text isn't too width
            if (m_Text.getGlobalBounds().width > (m_Size.x * 0.8f))
            {
                // The text is too width, so make it smaller
                m_Text.setCharacterSize(static_cast<unsigned int>(size * m_Size.x * 0.8f / m_Text.getGlobalBounds().width));
                m_Text.setCharacterSize(static_cast<unsigned int>(m_Text.getCharacterSize() - m_Text.getLocalBounds().top));
            }
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

    void Button::keyPressed(sf::Keyboard::Key key)
    {
        // Check if the space key or the return key was pressed
        if (key == sf::Keyboard::Space)
        {
            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[SpaceKeyPressed].empty() == false)
            {
                m_Callback.trigger = SpaceKeyPressed;
                addCallback();
            }
        }
        else if (key == sf::Keyboard::Return)
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

    void Button::objectFocused()
    {
        // We can't be focused when we don't have a focus image
        if ((m_ObjectPhase & ObjectPhase_Focused) == 0)
            m_Parent->unfocusObject(this);
        else
            Object::objectFocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::initialize(tgui::Group *const parent)
    {
        m_Parent = parent;
        m_Text.setFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_SplitImage)
        {
            if (m_SeparateHoverImage)
            {
                if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                {
                    target.draw(m_TextureDown_L, states);
                    target.draw(m_TextureDown_M, states);
                    target.draw(m_TextureDown_R, states);
                }
                else if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
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
                if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
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
                if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                {
                    target.draw(m_TextureHover_L, states);
                    target.draw(m_TextureHover_M, states);
                    target.draw(m_TextureHover_R, states);
                }
            }

            // When the button is focused then draw an extra image
            if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
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
                if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                {
                    target.draw(m_TextureDown_M, states);
                }
                else if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
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
                if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                {
                    target.draw(m_TextureDown_M, states);
                }
                else
                {
                    target.draw(m_TextureNormal_M, states);
                }

                // When the mouse is on top of the button then draw an extra image
                if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                {
                    target.draw(m_TextureHover_M, states);
                }
            }

            // When the button is focused then draw an extra image
            if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
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
