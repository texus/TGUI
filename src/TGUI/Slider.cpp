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
#include <TGUI/Slider.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider() :
    m_MouseDownOnThumb    (false),
    m_Minimum             ( 0),
    m_Maximum             (10),
    m_Value               ( 0),
    m_VerticalScroll      (true),
    m_VerticalImage       (true),
    m_SplitImage          (false),
    m_SeparateHoverImage  (false)
    {
        m_Callback.widgetType = Type_Slider;
        m_DraggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider(const Slider& copy) :
    Widget               (copy),
    m_LoadedConfigFile   (copy.m_LoadedConfigFile),
    m_MouseDownOnThumb   (copy.m_MouseDownOnThumb),
    m_MouseDownOnThumbPos(copy.m_MouseDownOnThumbPos),
    m_Minimum            (copy.m_Minimum),
    m_Maximum            (copy.m_Maximum),
    m_Value              (copy.m_Value),
    m_VerticalScroll     (copy.m_VerticalScroll),
    m_VerticalImage      (copy.m_VerticalImage),
    m_SplitImage         (copy.m_SplitImage),
    m_SeparateHoverImage (copy.m_SeparateHoverImage),
    m_Size               (copy.m_Size),
    m_ThumbSize          (copy.m_ThumbSize)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal_L, m_TextureTrackNormal_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover_L, m_TextureTrackHover_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal_M, m_TextureTrackNormal_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover_M, m_TextureTrackHover_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal_R, m_TextureTrackNormal_R);
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover_R, m_TextureTrackHover_R);
        TGUI_TextureManager.copyTexture(copy.m_TextureThumbNormal, m_TextureThumbNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureThumbHover, m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::~Slider()
    {
        if (m_TextureTrackNormal_L.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_L);
        if (m_TextureTrackHover_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_L);
        if (m_TextureTrackNormal_M.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_M);
        if (m_TextureTrackHover_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_M);
        if (m_TextureTrackNormal_R.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_R);
        if (m_TextureTrackHover_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_R);
        if (m_TextureThumbNormal.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider& Slider::operator= (const Slider& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Slider temp(right);
            this->Widget::operator=(right);

            std::swap(m_LoadedConfigFile,     temp.m_LoadedConfigFile);
            std::swap(m_MouseDownOnThumb,     temp.m_MouseDownOnThumb);
            std::swap(m_MouseDownOnThumbPos,  temp.m_MouseDownOnThumbPos);
            std::swap(m_Minimum,              temp.m_Minimum);
            std::swap(m_Maximum,              temp.m_Maximum);
            std::swap(m_Value,                temp.m_Value);
            std::swap(m_VerticalScroll,       temp.m_VerticalScroll);
            std::swap(m_VerticalImage,        temp.m_VerticalImage);
            std::swap(m_SplitImage,           temp.m_SplitImage);
            std::swap(m_SeparateHoverImage,   temp.m_SeparateHoverImage);
            std::swap(m_Size,                 temp.m_Size);
            std::swap(m_ThumbSize,            temp.m_ThumbSize);
            std::swap(m_TextureTrackNormal_L, temp.m_TextureTrackNormal_L);
            std::swap(m_TextureTrackHover_L,  temp.m_TextureTrackHover_L);
            std::swap(m_TextureTrackNormal_M, temp.m_TextureTrackNormal_M);
            std::swap(m_TextureTrackHover_M,  temp.m_TextureTrackHover_M);
            std::swap(m_TextureTrackNormal_R, temp.m_TextureTrackNormal_R);
            std::swap(m_TextureTrackHover_R,  temp.m_TextureTrackHover_R);
            std::swap(m_TextureThumbNormal,   temp.m_TextureThumbNormal);
            std::swap(m_TextureThumbHover,    temp.m_TextureThumbHover);
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
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Remove all textures if they were loaded before
        if (m_TextureTrackNormal_L.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_L);
        if (m_TextureTrackHover_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_L);
        if (m_TextureTrackNormal_M.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_M);
        if (m_TextureTrackHover_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_M);
        if (m_TextureTrackNormal_R.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureTrackNormal_R);
        if (m_TextureTrackHover_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureTrackHover_R);
        if (m_TextureThumbNormal.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureThumbHover);

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
        if (!configFile.read("Slider", properties, values))
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
            else if (property == "verticalscroll")
            {
                m_VerticalScroll = configFile.readBool(value, false);
                m_VerticalImage = m_VerticalScroll;
            }
            else if (property == "tracknormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_L in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_M in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "tracknormalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_R in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_L in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_M in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_R in section Slider in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Slider in " + m_LoadedConfigFile + ".");
        }

        // Check if the image is split
        if (m_SplitImage)
        {
            // Make sure the required textures were loaded
            if ((m_TextureTrackNormal_L.data != nullptr) && (m_TextureTrackNormal_M.data != nullptr)
             && (m_TextureTrackNormal_R.data != nullptr) && (m_TextureThumbNormal.data != nullptr))
            {
                // Set the size of the slider
                if (m_VerticalImage)
                    m_Size = sf::Vector2f(static_cast<float>(m_TextureTrackNormal_M.getSize().x), static_cast<float>(m_TextureTrackNormal_L.getSize().y + m_TextureTrackNormal_M.getSize().y + m_TextureTrackNormal_R.getSize().y));
                else
                    m_Size = sf::Vector2f(static_cast<float>(m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_M.getSize().x + m_TextureTrackNormal_R.getSize().x), static_cast<float>(m_TextureTrackNormal_M.getSize().y));

                // Set the thumb size
                m_ThumbSize = sf::Vector2f(m_TextureThumbNormal.getSize());

                m_TextureTrackNormal_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureTrackHover_L.data != nullptr) && (m_TextureTrackHover_M.data != nullptr)
             && (m_TextureTrackHover_R.data != nullptr) && (m_TextureThumbHover.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Hover;

                m_TextureTrackHover_M.data->texture.setRepeated(true);
            }
        }
        else // The image isn't split
        {
            // Make sure the required textures were loaded
            if ((m_TextureTrackNormal_M.data != nullptr) && (m_TextureThumbNormal.data != nullptr))
            {
                // Set the size of the slider
                m_Size = sf::Vector2f(m_TextureTrackNormal_M.getSize());

                // Set the thumb size
                m_ThumbSize = sf::Vector2f(m_TextureThumbNormal.getSize());
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureTrackHover_M.data != nullptr) && (m_TextureThumbHover.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Hover;
            }
        }

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Slider::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setPosition(float x, float y)
    {
        Widget::setPosition(x, y);

        if (m_SplitImage)
        {
            m_TextureTrackNormal_L.sprite.setPosition(x, y);
            m_TextureTrackHover_L.sprite.setPosition(x, y);

            // Swap the width and height meanings depending on how the slider lies
            float width;
            float height;
            if (m_VerticalScroll)
            {
                width = m_Size.x;
                height = m_Size.y;
            }
            else
            {
                width = m_Size.y;
                height = m_Size.x;
            }

            if (m_VerticalImage)
            {
                // Check if the middle image may be drawn
                if ((m_TextureTrackNormal_M.sprite.getScale().x * (m_TextureTrackNormal_L.getSize().y + m_TextureTrackNormal_R.getSize().y)) < height)
                {
                    float scalingX = width / m_TextureTrackNormal_M.getSize().x;

                    m_TextureTrackNormal_M.sprite.setPosition(x, y + (m_TextureTrackNormal_L.getSize().y * m_TextureTrackNormal_L.sprite.getScale().y));
                    m_TextureTrackHover_M.sprite.setPosition(x, y + (m_TextureTrackHover_L.getSize().y * m_TextureTrackHover_L.sprite.getScale().y));

                    m_TextureTrackNormal_R.sprite.setPosition(x, m_TextureTrackNormal_M.sprite.getPosition().y + (m_TextureTrackNormal_M.sprite.getTextureRect().height * scalingX));
                    m_TextureTrackHover_R.sprite.setPosition(x, m_TextureTrackHover_M.sprite.getPosition().y + (m_TextureTrackHover_M.sprite.getTextureRect().height * scalingX));
                }
                else // The middle image isn't drawn
                {
                    m_TextureTrackNormal_R.sprite.setPosition(x, y + (m_TextureTrackNormal_L.getSize().y * m_TextureTrackNormal_L.sprite.getScale().y));
                    m_TextureTrackHover_R.sprite.setPosition(x, y + (m_TextureTrackHover_L.getSize().y * m_TextureTrackHover_L.sprite.getScale().y));
                }
            }
            else // The slider image lies vertical
            {
                // Check if the middle image may be drawn
                if ((m_TextureTrackNormal_M.sprite.getScale().y * (m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_R.getSize().x)) < height)
                {
                    float scalingY = width / m_TextureTrackNormal_M.getSize().y;

                    m_TextureTrackNormal_M.sprite.setPosition(x + (m_TextureTrackNormal_L.getSize().x * m_TextureTrackNormal_L.sprite.getScale().x), y);
                    m_TextureTrackHover_M.sprite.setPosition(x + (m_TextureTrackHover_L.getSize().x * m_TextureTrackHover_L.sprite.getScale().x), y);

                    m_TextureTrackNormal_R.sprite.setPosition(m_TextureTrackNormal_M.sprite.getPosition().x + (m_TextureTrackNormal_M.sprite.getTextureRect().width * scalingY), y);
                    m_TextureTrackHover_R.sprite.setPosition(m_TextureTrackHover_M.sprite.getPosition().x + (m_TextureTrackHover_M.sprite.getTextureRect().width * scalingY), y);
                }
                else // The middle image isn't drawn
                {
                    m_TextureTrackNormal_R.sprite.setPosition(x + (m_TextureTrackNormal_L.getSize().x * m_TextureTrackNormal_L.sprite.getScale().x), y);
                    m_TextureTrackHover_R.sprite.setPosition(x + (m_TextureTrackHover_L.getSize().x * m_TextureTrackHover_L.sprite.getScale().x), y);
                }
            }
        }
        else // The images aren't split
        {
            m_TextureTrackNormal_M.sprite.setPosition(x, y);
            m_TextureTrackHover_M.sprite.setPosition(x, y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the size of the slider
        m_Size.x = width;
        m_Size.y = height;

        // A negative size is not allowed for this widget
        if (m_Size.x < 0) m_Size.x = -m_Size.x;
        if (m_Size.y < 0) m_Size.y = -m_Size.y;

        // Apply the miniumum size and set the texture rect of the middle image
        if (m_SplitImage)
        {
            float scaling;
            if (m_VerticalImage)
            {
                if (m_VerticalScroll)
                {
                    scaling = m_Size.x / m_TextureTrackNormal_M.getSize().x;
                    float minimumHeight = (m_TextureTrackNormal_L.getSize().y + m_TextureTrackNormal_R.getSize().y) * scaling;

                    if (m_Size.y < minimumHeight)
                        m_Size.y = minimumHeight;

                    m_TextureTrackNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, m_TextureTrackNormal_M.getSize().x, static_cast<int>((m_Size.y - minimumHeight) / scaling)));
                    m_TextureTrackHover_M.sprite.setTextureRect(sf::IntRect(0, 0, m_TextureTrackHover_M.getSize().x, static_cast<int>((m_Size.y - minimumHeight) / scaling)));
                }
                else
                {
                    scaling = m_Size.y / m_TextureTrackNormal_M.getSize().x;
                    float minimumWidth = (m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_R.getSize().x) * scaling;

                    if (m_Size.x < minimumWidth)
                        m_Size.x = minimumWidth;

                    m_TextureTrackNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, m_TextureTrackNormal_M.getSize().x, static_cast<int>((m_Size.x - minimumWidth) / scaling)));
                    m_TextureTrackHover_M.sprite.setTextureRect(sf::IntRect(0, 0, m_TextureTrackNormal_M.getSize().x, static_cast<int>((m_Size.x - minimumWidth) / scaling)));
                }
            }
            else
            {
                if (m_VerticalScroll)
                {
                    scaling = m_Size.x / m_TextureTrackNormal_M.getSize().y;
                    float minimumHeight = (m_TextureTrackNormal_L.getSize().y + m_TextureTrackNormal_R.getSize().y) * scaling;

                    if (m_Size.y < minimumHeight)
                        m_Size.y = minimumHeight;

                    m_TextureTrackNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.y - minimumHeight) / scaling), m_TextureTrackNormal_M.getSize().y));
                    m_TextureTrackHover_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.y - minimumHeight) / scaling), m_TextureTrackHover_M.getSize().y));
                }
                else
                {
                    scaling = m_Size.y / m_TextureTrackNormal_M.getSize().y;
                    float minimumWidth = (m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_R.getSize().x) * scaling;

                    if (m_Size.x < minimumWidth)
                        m_Size.x = minimumWidth;

                    m_TextureTrackNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scaling), m_TextureTrackNormal_M.getSize().y));
                    m_TextureTrackHover_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scaling), m_TextureTrackHover_M.getSize().y));
                }
            }

            m_TextureTrackNormal_L.sprite.setScale(scaling, scaling);
            m_TextureTrackHover_L.sprite.setScale(scaling, scaling);

            m_TextureTrackNormal_M.sprite.setScale(scaling, scaling);
            m_TextureTrackHover_M.sprite.setScale(scaling, scaling);

            m_TextureTrackNormal_R.sprite.setScale(scaling, scaling);
            m_TextureTrackHover_R.sprite.setScale(scaling, scaling);
        }
        else // The image is not split
        {
            if (m_VerticalImage == m_VerticalScroll)
            {
                m_TextureTrackNormal_M.sprite.setScale(m_Size.x / m_TextureTrackNormal_M.getSize().x, m_Size.y / m_TextureTrackNormal_M.getSize().y);
                m_TextureTrackHover_M.sprite.setScale(m_Size.x / m_TextureTrackHover_M.getSize().x, m_Size.y / m_TextureTrackHover_M.getSize().y);
            }
            else
            {
                m_TextureTrackNormal_M.sprite.setScale(m_Size.y / m_TextureTrackNormal_M.getSize().x, m_Size.x / m_TextureTrackNormal_M.getSize().y);
                m_TextureTrackHover_M.sprite.setScale(m_Size.y / m_TextureTrackHover_M.getSize().x, m_Size.x / m_TextureTrackHover_M.getSize().y);
            }
        }

        // Apply the scaling to the thumb image
        if (m_VerticalImage)
        {
            if (m_VerticalScroll)
            {
                m_TextureThumbNormal.sprite.setScale(m_Size.x / m_TextureTrackNormal_M.getSize().x, m_Size.x / m_TextureTrackNormal_M.getSize().x);
                m_TextureThumbHover.sprite.setScale(m_Size.x / m_TextureTrackHover_M.getSize().x, m_Size.x / m_TextureTrackHover_M.getSize().x);
            }
            else // Slider is displayed horizontal
            {
                m_TextureThumbNormal.sprite.setScale(m_Size.y / m_TextureTrackNormal_M.getSize().x, m_Size.y / m_TextureTrackNormal_M.getSize().x);
                m_TextureThumbHover.sprite.setScale(m_Size.y / m_TextureTrackHover_M.getSize().x, m_Size.y / m_TextureTrackHover_M.getSize().x);
            }
        }
        else // Slider image lies horizontal
        {
            if (m_VerticalScroll)
            {
                m_TextureThumbNormal.sprite.setScale(m_Size.x / m_TextureTrackNormal_M.getSize().y, m_Size.x / m_TextureTrackNormal_M.getSize().y);
                m_TextureThumbHover.sprite.setScale(m_Size.x / m_TextureTrackHover_M.getSize().y, m_Size.x / m_TextureTrackHover_M.getSize().y);
            }
            else // Slider is displayed horizontal
            {
                m_TextureThumbNormal.sprite.setScale(m_Size.y / m_TextureTrackNormal_M.getSize().y, m_Size.y / m_TextureTrackNormal_M.getSize().y);
                m_TextureThumbHover.sprite.setScale(m_Size.y / m_TextureTrackHover_M.getSize().y, m_Size.y / m_TextureTrackHover_M.getSize().y);
            }
        }

        // Set the thumb size
        if (m_VerticalImage == m_VerticalScroll)
        {
            if (m_VerticalScroll)
            {
                m_ThumbSize.x = (m_Size.x / m_TextureTrackNormal_M.getSize().x) * m_TextureThumbNormal.getSize().x;
                m_ThumbSize.y = (m_Size.x / m_TextureTrackNormal_M.getSize().x) * m_TextureThumbNormal.getSize().y;
            }
            else
            {
                m_ThumbSize.x = (m_Size.y / m_TextureTrackNormal_M.getSize().y) * m_TextureThumbNormal.getSize().x;
                m_ThumbSize.y = (m_Size.y / m_TextureTrackNormal_M.getSize().y) * m_TextureThumbNormal.getSize().y;
            }
        }
        else // m_VerticalImage != m_VerticalScroll
        {
            if (m_VerticalScroll)
            {
                m_ThumbSize.x = (m_Size.x / m_TextureTrackNormal_M.getSize().y) * m_TextureThumbNormal.getSize().y;
                m_ThumbSize.y = (m_Size.x / m_TextureTrackNormal_M.getSize().y) * m_TextureThumbNormal.getSize().x;
            }
            else
            {
                m_ThumbSize.x = (m_Size.y / m_TextureTrackNormal_M.getSize().x) * m_TextureThumbNormal.getSize().y;
                m_ThumbSize.y = (m_Size.y / m_TextureTrackNormal_M.getSize().x) * m_TextureThumbNormal.getSize().x;
            }
        }

        // Recalculate the position of the images
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider::getSize() const
    {
        if (m_Loaded)
            return sf::Vector2f(m_Size.x, m_Size.y);
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_Minimum = minimum;

        // The maximum can't be below the minimum
        if (m_Maximum < m_Minimum)
            m_Maximum = m_Minimum;

        // When the value is below the minimum then adjust it
        if (m_Value < m_Minimum)
            setValue(m_Minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_Maximum = maximum;
        else
            m_Maximum = 1;

        // The minimum can't be below the maximum
        if (m_Minimum > m_Maximum)
            setMinimum(m_Maximum);

        // When the value is above the maximum then adjust it
        if (m_Value > m_Maximum)
            setValue(m_Maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setValue(unsigned int value)
    {
        if (m_Value != value)
        {
            // Set the new value
            m_Value = value;

            // When the value is below the minimum or above the maximum then adjust it
            if (m_Value < m_Minimum)
                m_Value = m_Minimum;
            else if (m_Value > m_Maximum)
                m_Value = m_Maximum;

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[ValueChanged].empty() == false)
            {
                m_Callback.trigger = ValueChanged;
                m_Callback.value   = static_cast<int>(m_Value);
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setVerticalScroll(bool verticalScroll)
    {
        // Only continue when the value changed
        if (m_VerticalScroll != verticalScroll)
        {
            // Change the internal value
            m_VerticalScroll = verticalScroll;

            // Swap the width and height if needed
            if (m_VerticalScroll)
            {
                if (m_Size.x > m_Size.y)
                    setSize(m_Size.y, m_Size.x);
                else
                    setSize(m_Size.x, m_Size.y);
            }
            else // The slider lies horizontal
            {
                if (m_Size.y > m_Size.x)
                    setSize(m_Size.y, m_Size.x);
                else
                    setSize(m_Size.x, m_Size.y);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getMinimum() const
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getMaximum() const
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Slider::getValue() const
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getVerticalScroll() const
    {
        return m_VerticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_TextureTrackNormal_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTrackHover_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTrackNormal_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTrackHover_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTrackNormal_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTrackHover_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureThumbNormal.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureThumbHover.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Calculate the thumb position
        float thumbLeft;
        float thumbTop;
        if (m_VerticalScroll)
        {
            thumbLeft = (m_Size.x - m_ThumbSize.x) / 2.0f;
            thumbTop = ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.y) - (m_ThumbSize.y / 2.0f);
        }
        else // The slider lies horizontal
        {
            thumbLeft = ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.x) - (m_ThumbSize.x / 2.0f);
            thumbTop = (m_Size.y - m_ThumbSize.y) / 2.0f;
        }

        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(position.x + thumbLeft, position.y + thumbTop, m_ThumbSize.x, m_ThumbSize.y).contains(x, y))
        {
            m_MouseDownOnThumb = true;
            m_MouseDownOnThumbPos.x = x - position.x - thumbLeft;
            m_MouseDownOnThumbPos.y = y - position.y - thumbTop;
            return true;
        }
        else // The mouse is not on top of the thumb
            m_MouseDownOnThumb = false;

        // Check if the mouse is on top of the track
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
            return true;

        if (m_MouseHover)
            mouseLeftWidget();

        // The mouse is not on top of the slider
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMousePressed(float x, float y)
    {
        m_MouseDown = true;

        // Refresh the value
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseReleased(float, float)
    {
        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseMoved(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        if (m_MouseHover == false)
            mouseEnteredWidget();

        m_MouseHover = true;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Check if the mouse button is down
        if (m_MouseDown)
        {
            // Check in which direction the slider goes
            if (m_VerticalScroll)
            {
                // Check if the thumb is being dragged
                if (m_MouseDownOnThumb)
                {
                    // Set the new value
                    if ((y - m_MouseDownOnThumbPos.y + (m_ThumbSize.y / 2.0f) - position.y) > 0)
                        setValue(static_cast <unsigned int> ((((y - m_MouseDownOnThumbPos.y + (m_ThumbSize.y / 2.0f) - position.y) / m_Size.y) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else
                        setValue(m_Minimum);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if ((y - position.y) > 0)
                        setValue(static_cast <unsigned int> ((((y - position.y) / m_Size.y) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else // The position is negative, the calculation can't be done (but is not needed)
                        setValue(m_Minimum);
                }
            }
            else // the slider lies horizontal
            {
                // Check if the thumb is being dragged
                if (m_MouseDownOnThumb)
                {
                    // Set the new value
                    if ((x - m_MouseDownOnThumbPos.x + (m_ThumbSize.x / 2.0f) - position.x) > 0)
                        setValue(static_cast <unsigned int> ((((x - m_MouseDownOnThumbPos.x + (m_ThumbSize.x / 2.0f) - position.x) / m_Size.x) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else
                        setValue(m_Minimum);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if (x - position.x > 0)
                        setValue(static_cast<unsigned int>((((x - position.x) / m_Size.x) * (m_Maximum - m_Minimum)) + m_Minimum + 0.5f));
                    else // The position is negative, the calculation can't be done (but is not needed)
                        setValue(m_Minimum);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseWheelMoved(int delta, int, int)
    {
        if (static_cast<int>(m_Value) - delta < static_cast<int>(m_Minimum))
            setValue(m_Minimum);
        else
            setValue(static_cast<unsigned int>(m_Value - delta));
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
            setMinimum(atoi(value.c_str()));
        }
        else if (property == "maximum")
        {
            setMaximum(atoi(value.c_str()));
        }
        else if (property == "value")
        {
            setValue(atoi(value.c_str()));
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
            value = to_string(getMinimum());
        else if (property == "maximum")
            value = to_string(getMaximum());
        else if (property == "value")
            value = to_string(getValue());
        else if (property == "verticalscroll")
            value = m_VerticalScroll ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(ValueChanged) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(ValueChanged).size() == 1) && (m_CallbackFunctions.at(ValueChanged).front() == nullptr))
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
        if (m_Loaded == false)
            return;

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Check if the image is split
        if (m_SplitImage)
        {
            // Set the rotation
            if ((m_VerticalImage == true) && (m_VerticalScroll == false))
            {
                states.transform.rotate(-90,
                                        getPosition().x + m_TextureTrackNormal_L.getSize().x * 0.5f * (m_Size.y / m_TextureTrackNormal_L.getSize().x),
                                        getPosition().y + m_TextureTrackNormal_L.getSize().x * 0.5f * (m_Size.y / m_TextureTrackNormal_M.getSize().x));
            }
            else if ((m_VerticalImage == false) && (m_VerticalScroll == true))
            {
                states.transform.rotate(90,
                                        getPosition().x + m_TextureTrackNormal_L.getSize().y * 0.5f * (m_Size.x / m_TextureTrackNormal_L.getSize().y),
                                        getPosition().y + m_TextureTrackNormal_L.getSize().y * 0.5f * (m_Size.x / m_TextureTrackNormal_L.getSize().y));
            }

            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_TextureTrackHover_L, states);
                    target.draw(m_TextureTrackHover_M, states);
                    target.draw(m_TextureTrackHover_R, states);
                }
                else
                {
                    target.draw(m_TextureTrackNormal_L, states);
                    target.draw(m_TextureTrackNormal_M, states);
                    target.draw(m_TextureTrackNormal_R, states);
                }
            }
            else // The hover image is drawn on top of the normal one
            {
                target.draw(m_TextureTrackNormal_L, states);
                target.draw(m_TextureTrackNormal_M, states);
                target.draw(m_TextureTrackNormal_R, states);

                // When the mouse is on top of the button then draw an extra image
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_TextureTrackHover_L, states);
                    target.draw(m_TextureTrackHover_M, states);
                    target.draw(m_TextureTrackHover_R, states);
                }
            }
        }
        else // The image is not split
        {
            // Set the rotation
            if ((m_VerticalImage == true) && (m_VerticalScroll == false))
            {
                states.transform.rotate(-90,
                                        getPosition().x + m_TextureTrackNormal_M.getSize().x * 0.5f * (m_Size.y / m_TextureTrackNormal_M.getSize().x),
                                        getPosition().y + m_TextureTrackNormal_M.getSize().x * 0.5f * (m_Size.y / m_TextureTrackNormal_M.getSize().x));
            }
            else if ((m_VerticalImage == false) && (m_VerticalScroll == true))
            {
                states.transform.rotate(90,
                                        getPosition().x + m_TextureTrackNormal_M.getSize().y * 0.5f * (m_Size.x / m_TextureTrackNormal_M.getSize().y),
                                        getPosition().y + m_TextureTrackNormal_M.getSize().y * 0.5f * (m_Size.x / m_TextureTrackNormal_M.getSize().y));
            }

            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureTrackHover_M, states);
                else
                    target.draw(m_TextureTrackNormal_M, states);
            }
            else
            {
                // Draw the normal track image
                target.draw(m_TextureTrackNormal_M, states);

                // When the mouse is on top of the slider then draw the hover image
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureTrackHover_M, states);
            }
        }

        // Reset the transform
        states.transform = oldTransform;
        states.transform *= getTransform();

        // The thumb will be on a different position when we are scrolling vertically or not
        if (m_VerticalScroll)
        {
            // Set the translation and scale for the thumb
            states.transform.translate((m_Size.x - m_ThumbSize.x) * 0.5f,
                                       ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.y) - (m_ThumbSize.y * 0.5f));
        }
        else // the slider lies horizontal
        {
            // Set the translation and scale for the thumb
            states.transform.translate(((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.x) - (m_ThumbSize.x * 0.5f),
                                        (m_Size.y - m_ThumbSize.y) * 0.5f);
        }

        // It is possible that the image is not drawn in the same direction than the loaded image
        if ((m_VerticalImage == true) && (m_VerticalScroll == false))
        {
            states.transform.rotate(-90,
                                    m_TextureThumbNormal.getSize().x * 0.5f * (m_Size.y / m_TextureTrackNormal_M.getSize().x),
                                    m_TextureThumbNormal.getSize().x * 0.5f * (m_Size.y / m_TextureTrackNormal_M.getSize().x));
        }
        else if ((m_VerticalImage == false) && (m_VerticalScroll == true))
        {
            states.transform.rotate(90,
                                    m_TextureThumbNormal.getSize().y * 0.5f * (m_Size.x / m_TextureTrackNormal_M.getSize().y),
                                    m_TextureThumbNormal.getSize().y * 0.5f * (m_Size.x / m_TextureTrackNormal_M.getSize().y));
        }

        // Draw the normal thumb image
        target.draw(m_TextureThumbNormal, states);

        // When the mouse is on top of the slider then draw the hover image
        if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
            target.draw(m_TextureThumbHover, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
