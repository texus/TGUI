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
#include <TGUI/Slider.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider() :
    m_mouseDownOnThumb    (false),
    m_minimum             ( 0),
    m_maximum             (10),
    m_value               ( 0),
    m_verticalScroll      (true),
    m_verticalImage       (true),
    m_splitImage          (false),
    m_separateHoverImage  (false)
    {
        m_callback.widgetType = Type_Slider;
        m_draggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider(const Slider& copy) :
    Widget               (copy),
    m_loadedConfigFile   (copy.m_loadedConfigFile),
    m_mouseDownOnThumb   (copy.m_mouseDownOnThumb),
    m_mouseDownOnThumbPos(copy.m_mouseDownOnThumbPos),
    m_minimum            (copy.m_minimum),
    m_maximum            (copy.m_maximum),
    m_value              (copy.m_value),
    m_verticalScroll     (copy.m_verticalScroll),
    m_verticalImage      (copy.m_verticalImage),
    m_splitImage         (copy.m_splitImage),
    m_separateHoverImage (copy.m_separateHoverImage),
    m_size               (copy.m_size),
    m_thumbSize          (copy.m_thumbSize)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureTrackNormal_L, m_textureTrackNormal_L);
        TGUI_TextureManager.copyTexture(copy.m_textureTrackHover_L, m_textureTrackHover_L);
        TGUI_TextureManager.copyTexture(copy.m_textureTrackNormal_M, m_textureTrackNormal_M);
        TGUI_TextureManager.copyTexture(copy.m_textureTrackHover_M, m_textureTrackHover_M);
        TGUI_TextureManager.copyTexture(copy.m_textureTrackNormal_R, m_textureTrackNormal_R);
        TGUI_TextureManager.copyTexture(copy.m_textureTrackHover_R, m_textureTrackHover_R);
        TGUI_TextureManager.copyTexture(copy.m_textureThumbNormal, m_textureThumbNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureThumbHover, m_textureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::~Slider()
    {
        if (m_textureTrackNormal_L.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTrackNormal_L);
        if (m_textureTrackHover_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackHover_L);
        if (m_textureTrackNormal_M.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTrackNormal_M);
        if (m_textureTrackHover_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackHover_M);
        if (m_textureTrackNormal_R.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTrackNormal_R);
        if (m_textureTrackHover_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackHover_R);
        if (m_textureThumbNormal.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider& Slider::operator= (const Slider& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Slider temp(right);
            this->Widget::operator=(right);

            std::swap(m_loadedConfigFile,     temp.m_loadedConfigFile);
            std::swap(m_mouseDownOnThumb,     temp.m_mouseDownOnThumb);
            std::swap(m_mouseDownOnThumbPos,  temp.m_mouseDownOnThumbPos);
            std::swap(m_minimum,              temp.m_minimum);
            std::swap(m_maximum,              temp.m_maximum);
            std::swap(m_value,                temp.m_value);
            std::swap(m_verticalScroll,       temp.m_verticalScroll);
            std::swap(m_verticalImage,        temp.m_verticalImage);
            std::swap(m_splitImage,           temp.m_splitImage);
            std::swap(m_separateHoverImage,   temp.m_separateHoverImage);
            std::swap(m_size,                 temp.m_size);
            std::swap(m_thumbSize,            temp.m_thumbSize);
            std::swap(m_textureTrackNormal_L, temp.m_textureTrackNormal_L);
            std::swap(m_textureTrackHover_L,  temp.m_textureTrackHover_L);
            std::swap(m_textureTrackNormal_M, temp.m_textureTrackNormal_M);
            std::swap(m_textureTrackHover_M,  temp.m_textureTrackHover_M);
            std::swap(m_textureTrackNormal_R, temp.m_textureTrackNormal_R);
            std::swap(m_textureTrackHover_R,  temp.m_textureTrackHover_R);
            std::swap(m_textureThumbNormal,   temp.m_textureThumbNormal);
            std::swap(m_textureThumbHover,    temp.m_textureThumbHover);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider* Slider::clone()
    {
        return new Slider(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_loaded = false;

        // Remove all textures if they were loaded before
        if (m_textureTrackNormal_L.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTrackNormal_L);
        if (m_textureTrackHover_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackHover_L);
        if (m_textureTrackNormal_M.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTrackNormal_M);
        if (m_textureTrackHover_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackHover_M);
        if (m_textureTrackNormal_R.data != nullptr) TGUI_TextureManager.removeTexture(m_textureTrackNormal_R);
        if (m_textureTrackHover_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackHover_R);
        if (m_textureThumbNormal.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureThumbHover);

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
        if (!configFile.read("Slider", properties, values))
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
            else if (property == "verticalscroll")
            {
                m_verticalScroll = configFile.readBool(value, false);
                m_verticalImage = m_verticalScroll;
            }
            else if (property == "tracknormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = false;
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_L in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_M in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = true;
            }
            else if (property == "tracknormalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_R in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_L in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_M in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_R in section Slider in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Slider in " + m_loadedConfigFile + ".");
        }

        // Check if the image is split
        if (m_splitImage)
        {
            // Make sure the required textures were loaded
            if ((m_textureTrackNormal_L.data != nullptr) && (m_textureTrackNormal_M.data != nullptr)
             && (m_textureTrackNormal_R.data != nullptr) && (m_textureThumbNormal.data != nullptr))
            {
                // Set the size of the slider
                if (m_verticalImage)
                    m_size = sf::Vector2f(static_cast<float>(m_textureTrackNormal_M.getSize().x), static_cast<float>(m_textureTrackNormal_L.getSize().y + m_textureTrackNormal_M.getSize().y + m_textureTrackNormal_R.getSize().y));
                else
                    m_size = sf::Vector2f(static_cast<float>(m_textureTrackNormal_L.getSize().x + m_textureTrackNormal_M.getSize().x + m_textureTrackNormal_R.getSize().x), static_cast<float>(m_textureTrackNormal_M.getSize().y));

                // Set the thumb size
                m_thumbSize = sf::Vector2f(m_textureThumbNormal.getSize());

                m_textureTrackNormal_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider section in " + m_loadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_textureTrackHover_L.data != nullptr) && (m_textureTrackHover_M.data != nullptr)
             && (m_textureTrackHover_R.data != nullptr) && (m_textureThumbHover.data != nullptr))
            {
                m_widgetPhase |= WidgetPhase_Hover;

                m_textureTrackHover_M.data->texture.setRepeated(true);
            }
        }
        else // The image isn't split
        {
            // Make sure the required textures were loaded
            if ((m_textureTrackNormal_M.data != nullptr) && (m_textureThumbNormal.data != nullptr))
            {
                // Set the size of the slider
                m_size = sf::Vector2f(m_textureTrackNormal_M.getSize());

                // Set the thumb size
                m_thumbSize = sf::Vector2f(m_textureThumbNormal.getSize());
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider section in " + m_loadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_textureTrackHover_M.data != nullptr) && (m_textureThumbHover.data != nullptr))
            {
                m_widgetPhase |= WidgetPhase_Hover;
            }
        }

        return m_loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Slider::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setPosition(float x, float y)
    {
        Widget::setPosition(x, y);

        if (m_splitImage)
        {
            m_textureTrackNormal_L.sprite.setPosition(x, y);
            m_textureTrackHover_L.sprite.setPosition(x, y);

            // Swap the width and height meanings depending on how the slider lies
            float width;
            float height;
            if (m_verticalScroll)
            {
                width = m_size.x;
                height = m_size.y;
            }
            else
            {
                width = m_size.y;
                height = m_size.x;
            }

            if (m_verticalImage)
            {
                // Check if the middle image may be drawn
                if ((m_textureTrackNormal_M.sprite.getScale().x * (m_textureTrackNormal_L.getSize().y + m_textureTrackNormal_R.getSize().y)) < height)
                {
                    float scalingX = width / m_textureTrackNormal_M.getSize().x;

                    m_textureTrackNormal_M.sprite.setPosition(x, y + (m_textureTrackNormal_L.getSize().y * m_textureTrackNormal_L.sprite.getScale().y));
                    m_textureTrackHover_M.sprite.setPosition(x, y + (m_textureTrackHover_L.getSize().y * m_textureTrackHover_L.sprite.getScale().y));

                    m_textureTrackNormal_R.sprite.setPosition(x, m_textureTrackNormal_M.sprite.getPosition().y + (m_textureTrackNormal_M.sprite.getTextureRect().height * scalingX));
                    m_textureTrackHover_R.sprite.setPosition(x, m_textureTrackHover_M.sprite.getPosition().y + (m_textureTrackHover_M.sprite.getTextureRect().height * scalingX));
                }
                else // The middle image isn't drawn
                {
                    m_textureTrackNormal_R.sprite.setPosition(x, y + (m_textureTrackNormal_L.getSize().y * m_textureTrackNormal_L.sprite.getScale().y));
                    m_textureTrackHover_R.sprite.setPosition(x, y + (m_textureTrackHover_L.getSize().y * m_textureTrackHover_L.sprite.getScale().y));
                }
            }
            else // The slider image lies vertical
            {
                // Check if the middle image may be drawn
                if ((m_textureTrackNormal_M.sprite.getScale().y * (m_textureTrackNormal_L.getSize().x + m_textureTrackNormal_R.getSize().x)) < height)
                {
                    float scalingY = width / m_textureTrackNormal_M.getSize().y;

                    m_textureTrackNormal_M.sprite.setPosition(x + (m_textureTrackNormal_L.getSize().x * m_textureTrackNormal_L.sprite.getScale().x), y);
                    m_textureTrackHover_M.sprite.setPosition(x + (m_textureTrackHover_L.getSize().x * m_textureTrackHover_L.sprite.getScale().x), y);

                    m_textureTrackNormal_R.sprite.setPosition(m_textureTrackNormal_M.sprite.getPosition().x + (m_textureTrackNormal_M.sprite.getTextureRect().width * scalingY), y);
                    m_textureTrackHover_R.sprite.setPosition(m_textureTrackHover_M.sprite.getPosition().x + (m_textureTrackHover_M.sprite.getTextureRect().width * scalingY), y);
                }
                else // The middle image isn't drawn
                {
                    m_textureTrackNormal_R.sprite.setPosition(x + (m_textureTrackNormal_L.getSize().x * m_textureTrackNormal_L.sprite.getScale().x), y);
                    m_textureTrackHover_R.sprite.setPosition(x + (m_textureTrackHover_L.getSize().x * m_textureTrackHover_L.sprite.getScale().x), y);
                }
            }
        }
        else // The images aren't split
        {
            m_textureTrackNormal_M.sprite.setPosition(x, y);
            m_textureTrackHover_M.sprite.setPosition(x, y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Set the size of the slider
        m_size.x = width;
        m_size.y = height;

        // A negative size is not allowed for this widget
        if (m_size.x < 0) m_size.x = -m_size.x;
        if (m_size.y < 0) m_size.y = -m_size.y;

        // Apply the miniumum size and set the texture rect of the middle image
        if (m_splitImage)
        {
            float scaling;
            if (m_verticalImage)
            {
                if (m_verticalScroll)
                {
                    scaling = m_size.x / m_textureTrackNormal_M.getSize().x;
                    float minimumHeight = (m_textureTrackNormal_L.getSize().y + m_textureTrackNormal_R.getSize().y) * scaling;

                    if (m_size.y < minimumHeight)
                        m_size.y = minimumHeight;

                    m_textureTrackNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, m_textureTrackNormal_M.getSize().x, static_cast<int>((m_size.y - minimumHeight) / scaling)));
                    m_textureTrackHover_M.sprite.setTextureRect(sf::IntRect(0, 0, m_textureTrackHover_M.getSize().x, static_cast<int>((m_size.y - minimumHeight) / scaling)));
                }
                else
                {
                    scaling = m_size.y / m_textureTrackNormal_M.getSize().x;
                    float minimumWidth = (m_textureTrackNormal_L.getSize().x + m_textureTrackNormal_R.getSize().x) * scaling;

                    if (m_size.x < minimumWidth)
                        m_size.x = minimumWidth;

                    m_textureTrackNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, m_textureTrackNormal_M.getSize().x, static_cast<int>((m_size.x - minimumWidth) / scaling)));
                    m_textureTrackHover_M.sprite.setTextureRect(sf::IntRect(0, 0, m_textureTrackNormal_M.getSize().x, static_cast<int>((m_size.x - minimumWidth) / scaling)));
                }
            }
            else
            {
                if (m_verticalScroll)
                {
                    scaling = m_size.x / m_textureTrackNormal_M.getSize().y;
                    float minimumHeight = (m_textureTrackNormal_L.getSize().y + m_textureTrackNormal_R.getSize().y) * scaling;

                    if (m_size.y < minimumHeight)
                        m_size.y = minimumHeight;

                    m_textureTrackNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_size.y - minimumHeight) / scaling), m_textureTrackNormal_M.getSize().y));
                    m_textureTrackHover_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_size.y - minimumHeight) / scaling), m_textureTrackHover_M.getSize().y));
                }
                else
                {
                    scaling = m_size.y / m_textureTrackNormal_M.getSize().y;
                    float minimumWidth = (m_textureTrackNormal_L.getSize().x + m_textureTrackNormal_R.getSize().x) * scaling;

                    if (m_size.x < minimumWidth)
                        m_size.x = minimumWidth;

                    m_textureTrackNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_size.x - minimumWidth) / scaling), m_textureTrackNormal_M.getSize().y));
                    m_textureTrackHover_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_size.x - minimumWidth) / scaling), m_textureTrackHover_M.getSize().y));
                }
            }

            m_textureTrackNormal_L.sprite.setScale(scaling, scaling);
            m_textureTrackHover_L.sprite.setScale(scaling, scaling);

            m_textureTrackNormal_M.sprite.setScale(scaling, scaling);
            m_textureTrackHover_M.sprite.setScale(scaling, scaling);

            m_textureTrackNormal_R.sprite.setScale(scaling, scaling);
            m_textureTrackHover_R.sprite.setScale(scaling, scaling);
        }
        else // The image is not split
        {
            if (m_verticalImage == m_verticalScroll)
            {
                m_textureTrackNormal_M.sprite.setScale(m_size.x / m_textureTrackNormal_M.getSize().x, m_size.y / m_textureTrackNormal_M.getSize().y);
                m_textureTrackHover_M.sprite.setScale(m_size.x / m_textureTrackHover_M.getSize().x, m_size.y / m_textureTrackHover_M.getSize().y);
            }
            else
            {
                m_textureTrackNormal_M.sprite.setScale(m_size.y / m_textureTrackNormal_M.getSize().x, m_size.x / m_textureTrackNormal_M.getSize().y);
                m_textureTrackHover_M.sprite.setScale(m_size.y / m_textureTrackHover_M.getSize().x, m_size.x / m_textureTrackHover_M.getSize().y);
            }
        }

        // Apply the scaling to the thumb image
        if (m_verticalImage)
        {
            if (m_verticalScroll)
            {
                m_textureThumbNormal.sprite.setScale(m_size.x / m_textureTrackNormal_M.getSize().x, m_size.x / m_textureTrackNormal_M.getSize().x);
                m_textureThumbHover.sprite.setScale(m_size.x / m_textureTrackHover_M.getSize().x, m_size.x / m_textureTrackHover_M.getSize().x);
            }
            else // Slider is displayed horizontal
            {
                m_textureThumbNormal.sprite.setScale(m_size.y / m_textureTrackNormal_M.getSize().x, m_size.y / m_textureTrackNormal_M.getSize().x);
                m_textureThumbHover.sprite.setScale(m_size.y / m_textureTrackHover_M.getSize().x, m_size.y / m_textureTrackHover_M.getSize().x);
            }
        }
        else // Slider image lies horizontal
        {
            if (m_verticalScroll)
            {
                m_textureThumbNormal.sprite.setScale(m_size.x / m_textureTrackNormal_M.getSize().y, m_size.x / m_textureTrackNormal_M.getSize().y);
                m_textureThumbHover.sprite.setScale(m_size.x / m_textureTrackHover_M.getSize().y, m_size.x / m_textureTrackHover_M.getSize().y);
            }
            else // Slider is displayed horizontal
            {
                m_textureThumbNormal.sprite.setScale(m_size.y / m_textureTrackNormal_M.getSize().y, m_size.y / m_textureTrackNormal_M.getSize().y);
                m_textureThumbHover.sprite.setScale(m_size.y / m_textureTrackHover_M.getSize().y, m_size.y / m_textureTrackHover_M.getSize().y);
            }
        }

        // Set the thumb size
        if (m_verticalImage == m_verticalScroll)
        {
            if (m_verticalScroll)
            {
                m_thumbSize.x = (m_size.x / m_textureTrackNormal_M.getSize().x) * m_textureThumbNormal.getSize().x;
                m_thumbSize.y = (m_size.x / m_textureTrackNormal_M.getSize().x) * m_textureThumbNormal.getSize().y;
            }
            else
            {
                m_thumbSize.x = (m_size.y / m_textureTrackNormal_M.getSize().y) * m_textureThumbNormal.getSize().x;
                m_thumbSize.y = (m_size.y / m_textureTrackNormal_M.getSize().y) * m_textureThumbNormal.getSize().y;
            }
        }
        else // m_verticalImage != m_verticalScroll
        {
            if (m_verticalScroll)
            {
                m_thumbSize.x = (m_size.x / m_textureTrackNormal_M.getSize().y) * m_textureThumbNormal.getSize().y;
                m_thumbSize.y = (m_size.x / m_textureTrackNormal_M.getSize().y) * m_textureThumbNormal.getSize().x;
            }
            else
            {
                m_thumbSize.x = (m_size.y / m_textureTrackNormal_M.getSize().x) * m_textureThumbNormal.getSize().y;
                m_thumbSize.y = (m_size.y / m_textureTrackNormal_M.getSize().x) * m_textureThumbNormal.getSize().x;
            }
        }

        // Recalculate the position of the images
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider::getSize() const
    {
        if (m_loaded)
            return sf::Vector2f(m_size.x, m_size.y);
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The maximum can't be below the minimum
        if (m_maximum < m_minimum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_maximum = maximum;
        else
            m_maximum = 1;

        // The minimum can't be below the maximum
        if (m_minimum > m_maximum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setValue(unsigned int value)
    {
        if (m_value != value)
        {
            // Set the new value
            m_value = value;

            // When the value is below the minimum or above the maximum then adjust it
            if (m_value < m_minimum)
                m_value = m_minimum;
            else if (m_value > m_maximum)
                m_value = m_maximum;

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[ValueChanged].empty() == false)
            {
                m_callback.trigger = ValueChanged;
                m_callback.value   = static_cast<int>(m_value);
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setVerticalScroll(bool verticalScroll)
    {
        // Only continue when the value changed
        if (m_verticalScroll != verticalScroll)
        {
            // Change the internal value
            m_verticalScroll = verticalScroll;

            // Swap the width and height if needed
            if (m_verticalScroll)
            {
                if (m_size.x > m_size.y)
                    setSize(m_size.y, m_size.x);
                else
                    setSize(m_size.x, m_size.y);
            }
            else // The slider lies horizontal
            {
                if (m_size.y > m_size.x)
                    setSize(m_size.y, m_size.x);
                else
                    setSize(m_size.x, m_size.y);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getVerticalScroll() const
    {
        return m_verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_textureTrackNormal_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackHover_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackNormal_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackHover_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackNormal_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackHover_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureThumbNormal.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureThumbHover.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_loaded == false)
            return false;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Calculate the thumb position
        float thumbLeft;
        float thumbTop;
        if (m_verticalScroll)
        {
            thumbLeft = (m_size.x - m_thumbSize.x) / 2.0f;
            thumbTop = ((static_cast<float>(m_value - m_minimum) / (m_maximum - m_minimum)) * m_size.y) - (m_thumbSize.y / 2.0f);
        }
        else // The slider lies horizontal
        {
            thumbLeft = ((static_cast<float>(m_value - m_minimum) / (m_maximum - m_minimum)) * m_size.x) - (m_thumbSize.x / 2.0f);
            thumbTop = (m_size.y - m_thumbSize.y) / 2.0f;
        }

        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(position.x + thumbLeft, position.y + thumbTop, m_thumbSize.x, m_thumbSize.y).contains(x, y))
        {
            m_mouseDownOnThumb = true;
            m_mouseDownOnThumbPos.x = x - position.x - thumbLeft;
            m_mouseDownOnThumbPos.y = y - position.y - thumbTop;
            return true;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Check if the mouse is on top of the track
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x, m_size.y)).contains(x, y))
            return true;

        if (m_mouseHover)
            mouseLeftWidget();

        // The mouse is not on top of the slider
        m_mouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMousePressed(float x, float y)
    {
        m_mouseDown = true;

        // Refresh the value
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseReleased(float, float)
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseMoved(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_loaded == false)
            return;

        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // Check in which direction the slider goes
            if (m_verticalScroll)
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((y - m_mouseDownOnThumbPos.y + (m_thumbSize.y / 2.0f) - position.y) > 0)
                        setValue(static_cast <unsigned int> ((((y - m_mouseDownOnThumbPos.y + (m_thumbSize.y / 2.0f) - position.y) / m_size.y) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                    else
                        setValue(m_minimum);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if ((y - position.y) > 0)
                        setValue(static_cast <unsigned int> ((((y - position.y) / m_size.y) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                    else // The position is negative, the calculation can't be done (but is not needed)
                        setValue(m_minimum);
                }
            }
            else // the slider lies horizontal
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((x - m_mouseDownOnThumbPos.x + (m_thumbSize.x / 2.0f) - position.x) > 0)
                        setValue(static_cast <unsigned int> ((((x - m_mouseDownOnThumbPos.x + (m_thumbSize.x / 2.0f) - position.x) / m_size.x) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                    else
                        setValue(m_minimum);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if (x - position.x > 0)
                        setValue(static_cast<unsigned int>((((x - position.x) / m_size.x) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                    else // The position is negative, the calculation can't be done (but is not needed)
                        setValue(m_minimum);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseWheelMoved(int delta, int, int)
    {
        if (static_cast<int>(m_value) - delta < static_cast<int>(m_minimum))
            setValue(m_minimum);
        else
            setValue(static_cast<unsigned int>(m_value - delta));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::widgetFocused()
    {
        // A slider can't be focused (yet)
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "minimum")
        {
            setMinimum(std::stoi(value));
        }
        else if (property == "maximum")
        {
            setMaximum(std::stoi(value));
        }
        else if (property == "value")
        {
            setValue(std::stoi(value));
        }
        else if (property == "verticalscroll")
        {
            if ((value == "true") || (value == "True"))
                setVerticalScroll(true);
            else if ((value == "false") || (value == "False"))
                setVerticalScroll(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'VerticalScroll' property.");
        }
        else if (property == "callback")
        {
            Widget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "ValueChanged") || (*it == "valuechanged"))
                    bindCallback(ValueChanged);
            }
        }
        else // The property didn't match
            return Widget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "minimum")
            value = std::to_string(getMinimum());
        else if (property == "maximum")
            value = std::to_string(getMaximum());
        else if (property == "value")
            value = std::to_string(getValue());
        else if (property == "verticalscroll")
            value = m_verticalScroll ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(ValueChanged) != m_callbackFunctions.end()) && (m_callbackFunctions.at(ValueChanged).size() == 1) && (m_callbackFunctions.at(ValueChanged).front() == nullptr))
                callbacks.push_back("ValueChanged");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            return Widget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Slider::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Minimum", "uint"));
        list.push_back(std::pair<std::string, std::string>("Maximum", "uint"));
        list.push_back(std::pair<std::string, std::string>("Value", "uint"));
        list.push_back(std::pair<std::string, std::string>("VerticalScroll", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the slider wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Check if the image is split
        if (m_splitImage)
        {
            // Set the rotation
            if ((m_verticalImage == true) && (m_verticalScroll == false))
            {
                states.transform.rotate(-90,
                                        getPosition().x + m_textureTrackNormal_L.getSize().x * 0.5f * (m_size.y / m_textureTrackNormal_L.getSize().x),
                                        getPosition().y + m_textureTrackNormal_L.getSize().x * 0.5f * (m_size.y / m_textureTrackNormal_M.getSize().x));
            }
            else if ((m_verticalImage == false) && (m_verticalScroll == true))
            {
                states.transform.rotate(90,
                                        getPosition().x + m_textureTrackNormal_L.getSize().y * 0.5f * (m_size.x / m_textureTrackNormal_L.getSize().y),
                                        getPosition().y + m_textureTrackNormal_L.getSize().y * 0.5f * (m_size.x / m_textureTrackNormal_L.getSize().y));
            }

            if (m_separateHoverImage)
            {
                if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_textureTrackHover_L, states);
                    target.draw(m_textureTrackHover_M, states);
                    target.draw(m_textureTrackHover_R, states);
                }
                else
                {
                    target.draw(m_textureTrackNormal_L, states);
                    target.draw(m_textureTrackNormal_M, states);
                    target.draw(m_textureTrackNormal_R, states);
                }
            }
            else // The hover image is drawn on top of the normal one
            {
                target.draw(m_textureTrackNormal_L, states);
                target.draw(m_textureTrackNormal_M, states);
                target.draw(m_textureTrackNormal_R, states);

                // When the mouse is on top of the button then draw an extra image
                if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_textureTrackHover_L, states);
                    target.draw(m_textureTrackHover_M, states);
                    target.draw(m_textureTrackHover_R, states);
                }
            }
        }
        else // The image is not split
        {
            // Set the rotation
            if ((m_verticalImage == true) && (m_verticalScroll == false))
            {
                states.transform.rotate(-90,
                                        getPosition().x + m_textureTrackNormal_M.getSize().x * 0.5f * (m_size.y / m_textureTrackNormal_M.getSize().x),
                                        getPosition().y + m_textureTrackNormal_M.getSize().x * 0.5f * (m_size.y / m_textureTrackNormal_M.getSize().x));
            }
            else if ((m_verticalImage == false) && (m_verticalScroll == true))
            {
                states.transform.rotate(90,
                                        getPosition().x + m_textureTrackNormal_M.getSize().y * 0.5f * (m_size.x / m_textureTrackNormal_M.getSize().y),
                                        getPosition().y + m_textureTrackNormal_M.getSize().y * 0.5f * (m_size.x / m_textureTrackNormal_M.getSize().y));
            }

            if (m_separateHoverImage)
            {
                if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                    target.draw(m_textureTrackHover_M, states);
                else
                    target.draw(m_textureTrackNormal_M, states);
            }
            else
            {
                // Draw the normal track image
                target.draw(m_textureTrackNormal_M, states);

                // When the mouse is on top of the slider then draw the hover image
                if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                    target.draw(m_textureTrackHover_M, states);
            }
        }

        // Reset the transform
        states.transform = oldTransform;
        states.transform *= getTransform();

        // The thumb will be on a different position when we are scrolling vertically or not
        if (m_verticalScroll)
        {
            // Set the translation and scale for the thumb
            states.transform.translate((m_size.x - m_thumbSize.x) * 0.5f,
                                       ((static_cast<float>(m_value - m_minimum) / (m_maximum - m_minimum)) * m_size.y) - (m_thumbSize.y * 0.5f));
        }
        else // the slider lies horizontal
        {
            // Set the translation and scale for the thumb
            states.transform.translate(((static_cast<float>(m_value - m_minimum) / (m_maximum - m_minimum)) * m_size.x) - (m_thumbSize.x * 0.5f),
                                        (m_size.y - m_thumbSize.y) * 0.5f);
        }

        // It is possible that the image is not drawn in the same direction than the loaded image
        if ((m_verticalImage == true) && (m_verticalScroll == false))
        {
            states.transform.rotate(-90,
                                    m_textureThumbNormal.getSize().x * 0.5f * (m_size.y / m_textureTrackNormal_M.getSize().x),
                                    m_textureThumbNormal.getSize().x * 0.5f * (m_size.y / m_textureTrackNormal_M.getSize().x));
        }
        else if ((m_verticalImage == false) && (m_verticalScroll == true))
        {
            states.transform.rotate(90,
                                    m_textureThumbNormal.getSize().y * 0.5f * (m_size.x / m_textureTrackNormal_M.getSize().y),
                                    m_textureThumbNormal.getSize().y * 0.5f * (m_size.x / m_textureTrackNormal_M.getSize().y));
        }

        // Draw the normal thumb image
        target.draw(m_textureThumbNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
            target.draw(m_textureThumbHover, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
