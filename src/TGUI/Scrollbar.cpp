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


#include <TGUI/Scrollbar.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar() :
    m_mouseDownOnThumb  (false),
    m_maximum           (10),
    m_value             ( 0),
    m_lowValue          ( 6),
    m_verticalScroll    (true),
    m_verticalImage     (true),
    m_scrollAmount      ( 1),
    m_autoHide          (true),
    m_mouseDownOnArrow  (false),
    m_splitImage        (false),
    m_separateHoverImage(false)
    {
        m_callback.widgetType = Type_Scrollbar;
        m_draggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar(const Scrollbar& copy) :
    Widget               (copy),
    m_loadedConfigFile   (copy.m_loadedConfigFile),
    m_mouseDownOnThumb   (copy.m_mouseDownOnThumb),
    m_mouseDownOnThumbPos(copy.m_mouseDownOnThumbPos),
    m_maximum            (copy.m_maximum),
    m_value              (copy.m_value),
    m_lowValue           (copy.m_lowValue),
    m_verticalScroll     (copy.m_verticalScroll),
    m_verticalImage      (copy.m_verticalImage),
    m_scrollAmount       (copy.m_scrollAmount),
    m_autoHide           (copy.m_autoHide),
    m_mouseDownOnArrow   (copy.m_mouseDownOnArrow),
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
        TGUI_TextureManager.copyTexture(copy.m_textureArrowUpNormal, m_textureArrowUpNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureArrowUpHover, m_textureArrowUpHover);
        TGUI_TextureManager.copyTexture(copy.m_textureArrowDownNormal, m_textureArrowDownNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureArrowDownHover, m_textureArrowDownHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::~Scrollbar()
    {
        if (m_textureArrowUpNormal.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureArrowUpNormal);
        if (m_textureArrowUpHover.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureArrowUpHover);
        if (m_textureArrowDownNormal.data != nullptr) TGUI_TextureManager.removeTexture(m_textureArrowDownNormal);
        if (m_textureArrowDownHover.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureArrowDownHover);
        if (m_textureTrackNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackNormal_L);
        if (m_textureTrackHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureTrackHover_L);
        if (m_textureTrackNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackNormal_M);
        if (m_textureTrackHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureTrackHover_M);
        if (m_textureTrackNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackNormal_R);
        if (m_textureTrackHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureTrackHover_R);
        if (m_textureThumbNormal.data != nullptr)     TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.data != nullptr)      TGUI_TextureManager.removeTexture(m_textureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar& Scrollbar::operator= (const Scrollbar& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Scrollbar temp(right);
            this->Widget::operator=(right);

            std::swap(m_loadedConfigFile,       temp.m_loadedConfigFile);
            std::swap(m_mouseDownOnThumb,       temp.m_mouseDownOnThumb);
            std::swap(m_mouseDownOnThumbPos,    temp.m_mouseDownOnThumbPos);
            std::swap(m_maximum,                temp.m_maximum);
            std::swap(m_value,                  temp.m_value);
            std::swap(m_lowValue,               temp.m_lowValue);
            std::swap(m_verticalScroll,         temp.m_verticalScroll);
            std::swap(m_verticalImage,          temp.m_verticalImage);
            std::swap(m_scrollAmount,           temp.m_scrollAmount);
            std::swap(m_autoHide,               temp.m_autoHide);
            std::swap(m_mouseDownOnArrow,       temp.m_mouseDownOnArrow);
            std::swap(m_splitImage,             temp.m_splitImage);
            std::swap(m_separateHoverImage,     temp.m_separateHoverImage);
            std::swap(m_size,                   temp.m_size);
            std::swap(m_thumbSize,              temp.m_thumbSize);
            std::swap(m_textureTrackNormal_L,   temp.m_textureTrackNormal_L);
            std::swap(m_textureTrackHover_L,    temp.m_textureTrackHover_L);
            std::swap(m_textureTrackNormal_M,   temp.m_textureTrackNormal_M);
            std::swap(m_textureTrackHover_M,    temp.m_textureTrackHover_M);
            std::swap(m_textureTrackNormal_R,   temp.m_textureTrackNormal_R);
            std::swap(m_textureTrackHover_R,    temp.m_textureTrackHover_R);
            std::swap(m_textureThumbNormal,     temp.m_textureThumbNormal);
            std::swap(m_textureThumbHover,      temp.m_textureThumbHover);
            std::swap(m_textureArrowUpNormal,   temp.m_textureArrowUpNormal);
            std::swap(m_textureArrowUpHover,    temp.m_textureArrowUpHover);
            std::swap(m_textureArrowDownNormal, temp.m_textureArrowDownNormal);
            std::swap(m_textureArrowDownHover,  temp.m_textureArrowDownHover);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar* Scrollbar::clone()
    {
        return new Scrollbar(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_loaded = false;

        // Remove all textures if they were loaded before
        if (m_textureTrackNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackNormal_L);
        if (m_textureTrackHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureTrackHover_L);
        if (m_textureTrackNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackNormal_M);
        if (m_textureTrackHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureTrackHover_M);
        if (m_textureTrackNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackNormal_R);
        if (m_textureTrackHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureTrackHover_R);
        if (m_textureThumbNormal.data != nullptr)     TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.data != nullptr)      TGUI_TextureManager.removeTexture(m_textureThumbHover);
        if (m_textureArrowUpNormal.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureArrowUpNormal);
        if (m_textureArrowUpHover.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureArrowUpHover);
        if (m_textureArrowDownNormal.data != nullptr) TGUI_TextureManager.removeTexture(m_textureArrowDownNormal);
        if (m_textureArrowDownHover.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureArrowDownHover);

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
        if (!configFile.read("Scrollbar", properties, values))
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
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = false;
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowupnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureArrowUpNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpNormalImage in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowuphoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureArrowUpHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpHoverImage in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowdownnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureArrowDownNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownNormalImage in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowdownhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureArrowDownHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownHoverImage in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_L in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_M in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = true;
            }
            else if (property == "tracknormalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_R in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_L in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_M in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_R in section Scrollbar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Scrollbar in " + m_loadedConfigFile + ".");
        }

        // Check if the image is split
        if (m_splitImage)
        {
            TGUI_OUTPUT("TGUI error: SplitImage is not yet supported in Scrollbar.");
            return false;
/*
            // Make sure the required textures were loaded
            if ((m_textureTrackNormal_L.data != nullptr) && (m_textureTrackNormal_M.data != nullptr) && (m_textureTrackNormal_R.data != nullptr)
             && (m_textureThumbNormal.data != nullptr) && (m_textureArrowUpNormal.data != nullptr) && (m_textureArrowDownNormal.data != nullptr))
            {
                m_loaded = true;

                // Set the size of the scrollbar
                if (m_verticalImage)
                    setSize(static_cast<float>(m_textureTrackNormal_M.getSize().x), static_cast<float>(m_textureTrackNormal_L.getSize().y + m_textureTrackNormal_M.getSize().y + m_textureTrackNormal_R.getSize().y));
                else
                    setSize(static_cast<float>(m_textureTrackNormal_L.getSize().x + m_textureTrackNormal_M.getSize().x + m_textureTrackNormal_R.getSize().x), static_cast<float>(m_textureTrackNormal_M.getSize().y));

                // Set the thumb size
                m_thumbSize = sf::Vector2f(m_textureThumbNormal.getSize());
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the scrollbar. Is the Scrollbar section in " + m_loadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_textureTrackHover_L.data != nullptr) && (m_textureTrackHover_M.data != nullptr) && (m_textureTrackHover_R.data != nullptr)
             && (m_textureThumbHover.data != nullptr) && (m_textureArrowUpHover.data != nullptr) && (m_textureArrowDownHover.data != nullptr))
            {
                m_widgetPhase |= WidgetPhase_Hover;
            }
*/
        }
        else // The image isn't split
        {
            // Make sure the required textures were loaded
            if ((m_textureTrackNormal_M.data != nullptr) && (m_textureThumbNormal.data != nullptr)
             && (m_textureArrowUpNormal.data != nullptr) && (m_textureArrowDownNormal.data != nullptr))
            {
                // Set the size of the scrollbar
                m_loaded = true;
                setSize(static_cast<float>(m_textureTrackNormal_M.getSize().x), static_cast<float>(m_textureTrackNormal_M.getSize().y));
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the scrollbar. Is the Scrollbar section in " + m_loadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_textureTrackHover_M.data != nullptr) && (m_textureThumbHover.data != nullptr)
             && (m_textureArrowUpHover.data != nullptr) && (m_textureArrowDownHover.data != nullptr))
            {
                m_widgetPhase |= WidgetPhase_Hover;
            }
        }

        // When there is no error we will return true
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Scrollbar::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setSize(float width, float height)
    {
        // Don't do anything when the scrollbar wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Set the size of the scrollbar
        m_size.x = width;
        m_size.y = height;

        // A negative size is not allowed for this widget
        if (m_size.x < 0) m_size.x = -m_size.x;
        if (m_size.y < 0) m_size.y = -m_size.y;

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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Scrollbar::getSize() const
    {
        if (m_loaded)
            return sf::Vector2f(m_size.x, m_size.y);
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_maximum = maximum;
        else
            m_maximum = 1;

        // When the value is above the maximum then adjust it
        if (m_maximum < m_lowValue)
            setValue(0);
        else if (m_value > m_maximum - m_lowValue)
            setValue(m_maximum - m_lowValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setValue(unsigned int value)
    {
        if (m_value != value)
        {
            // Set the new value
            m_value = value;

            // When the value is above the maximum then adjust it
            if (m_maximum < m_lowValue)
                m_value = 0;
            else if (m_value > m_maximum - m_lowValue)
                m_value = m_maximum - m_lowValue;

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

    void Scrollbar::setLowValue(unsigned int lowValue)
    {
        // Set the new value
        m_lowValue = lowValue;

        // When the value is above the maximum then adjust it
        if (m_maximum < m_lowValue)
            setValue(0);
        else if (m_value > m_maximum - m_lowValue)
            setValue(m_maximum - m_lowValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setVerticalScroll(bool verticalScroll)
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
            else // The scrollbar lies horizontal
            {
                if (m_size.y > m_size.x)
                    setSize(m_size.y, m_size.x);
                else
                    setSize(m_size.x, m_size.y);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getLowValue() const
    {
        return m_lowValue;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getVerticalScroll() const
    {
        return m_verticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setArrowScrollAmount(unsigned int scrollAmount)
    {
        m_scrollAmount = scrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getArrowScrollAmount()
    {
        return m_scrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setAutoHide(bool autoHide)
    {
        m_autoHide = autoHide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getAutoHide() const
    {
        return m_autoHide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setTransparency(unsigned char transparency)
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

        m_textureArrowUpNormal.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureArrowUpHover.sprite.setColor(sf::Color(255, 255, 255, m_opacity));

        m_textureArrowDownNormal.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureArrowDownHover.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the scrollbar wasn't loaded correctly
        if (m_loaded == false)
            return false;

        // Don't make any calculations when no scrollbar is needed
        if ((m_maximum <= m_lowValue) && (m_autoHide == true))
            return false;

        // Check if the mouse is on top of the scrollbar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x, m_size.y)).contains(x, y))
            return true;

        if (m_mouseHover)
            mouseLeftWidget();

        // The mouse is not on top of the scrollbar
        m_mouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMousePressed(float x, float y)
    {
        m_mouseDown = true;
        m_mouseDownOnArrow = false;

        // Make sure you didn't click on one of the arrows
        if (m_verticalScroll)
        {
            float scalingX;
            if (m_verticalImage == m_verticalScroll)
                scalingX = m_size.x / m_textureTrackNormal_M.getSize().x;
            else
                scalingX = m_size.x / m_textureTrackNormal_M.getSize().y;

            // Check if the arrows are drawn at full size
            if (m_size.y > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scalingX)
            {
                // Check if you clicked on one of the arrows
                if (y < getPosition().y + (m_textureArrowUpNormal.getSize().y * scalingX))
                    m_mouseDownOnArrow = true;
                else if (y > getPosition().y + m_size.y - (m_textureArrowUpNormal.getSize().y * scalingX))
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }
        else // The scrollbar lies horizontal
        {
            float scalingY;
            if (m_verticalImage == m_verticalScroll)
                scalingY = m_size.y / m_textureTrackNormal_M.getSize().y;
            else
                scalingY = m_size.y / m_textureTrackNormal_M.getSize().x;

            // Check if the arrows are drawn at full size
            if (m_size.x > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scalingY)
            {
                // Check if you clicked on one of the arrows
                if (x < getPosition().x + (m_textureArrowUpNormal.getSize().y * scalingY))
                    m_mouseDownOnArrow = true;
                else if (x > getPosition().x + m_size.x - (m_textureArrowUpNormal.getSize().y * scalingY))
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }

        sf::Vector2f position = getPosition();
        sf::FloatRect thumbRect = getThumbRect();

        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(position.x + thumbRect.left, position.y + thumbRect.top, thumbRect.width, thumbRect.height).contains(x, y))
        {
            m_mouseDownOnThumbPos.x = x - position.x - thumbRect.left;
            m_mouseDownOnThumbPos.y = y - position.y - thumbRect.top;

            m_mouseDownOnThumb = true;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Refresh the scrollbar value
        if (m_mouseDownOnArrow == false)
            mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMouseReleased(float x, float y)
    {
        // Check if one of the arrows was clicked
        if ((m_mouseDown) && (m_mouseDownOnArrow))
        {
            // Only continue when the calculations can be made
            if (m_maximum > m_lowValue)
            {
                bool valueDown = false;
                bool valueUp = false;

                // Check in which direction the scrollbar lies
                if (m_verticalScroll)
                {
                    float scalingX;
                    if (m_verticalImage == m_verticalScroll)
                        scalingX = m_size.x / m_textureTrackNormal_M.getSize().x;
                    else
                        scalingX = m_size.x / m_textureTrackNormal_M.getSize().y;

                    // Check if the arrows are drawn at full size
                    if (m_size.y > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scalingX)
                    {
                        // Check if you clicked on the top arrow
                        if (y < getPosition().y + (m_textureArrowUpNormal.getSize().y * scalingX))
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (y > getPosition().y + m_size.y - (m_textureArrowUpNormal.getSize().y * scalingX))
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (y < getPosition().y + (m_textureArrowUpNormal.getSize().y * ((m_size.y * 0.5f) / m_textureArrowUpNormal.getSize().y)))
                            valueDown = true;
                        else // You clicked on the bottom arrow
                            valueUp = true;
                    }
                }
                else // The scrollbar lies horizontal
                {
                    float scalingY;
                    if (m_verticalImage == m_verticalScroll)
                        scalingY = m_size.y / m_textureTrackNormal_M.getSize().y;
                    else
                        scalingY = m_size.y / m_textureTrackNormal_M.getSize().x;

                    // Check if the arrows are drawn at full size
                    if (m_size.x > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scalingY)
                    {
                        // Check if you clicked on the left arrow
                        if (x < getPosition().x + (m_textureArrowUpNormal.getSize().y * scalingY))
                            valueDown = true;

                        // Check if you clicked the right arrow
                        else if (x > getPosition().x + m_size.x - (m_textureArrowUpNormal.getSize().y * scalingY))
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (x < getPosition().x + (m_textureArrowUpNormal.getSize().y * ((m_size.x * 0.5f) / m_textureArrowUpNormal.getSize().y)))
                            valueDown = true;
                        else // You clicked on the right arrow
                            valueUp = true;
                    }
                }

                if (valueDown)
                {
                    if (m_value > m_scrollAmount)
                        setValue(m_value - m_scrollAmount);
                    else
                        setValue(0);
                }
                else if (valueUp)
                {
                    if (m_value + m_scrollAmount < m_maximum - m_lowValue + 1)
                        setValue(m_value + m_scrollAmount);
                    else
                        setValue(m_maximum - m_lowValue);
                }
            }
        }

        // The mouse is no longer down
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseMoved(float x, float y)
    {
        // If the scrollbar wasn't loaded then do nothing
        if (m_loaded == false)
            return;

        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;

        // Check if the mouse button went down on top of the track (or thumb)
        if ((m_mouseDown) && (m_mouseDownOnArrow == false))
        {
            // Don't continue if the calculations can't be made
            if ((m_maximum <= m_lowValue) && (m_autoHide == false))
                return;

            // Get the current position
            sf::Vector2f position = getPosition();

            // Check in which direction the scrollbar lies
            if (m_verticalScroll)
            {
                float scalingX;
                if (m_verticalImage == m_verticalScroll)
                    scalingX = m_size.x / m_textureTrackNormal_M.getSize().x;
                else
                    scalingX = m_size.x / m_textureTrackNormal_M.getSize().y;

                // Calculate the arrow height
                float arrowHeight = m_textureArrowUpNormal.getSize().y * scalingX;

                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((y - m_mouseDownOnThumbPos.y - position.y - arrowHeight) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((y - m_mouseDownOnThumbPos.y - position.y - arrowHeight) / (m_size.y - (2 * arrowHeight))) * m_maximum) + 0.5f);

                        // If the value isn't too high then change it
                        if (value <= (m_maximum - m_lowValue))
                            setValue(value);
                        else
                            setValue(m_maximum - m_lowValue);
                    }
                    else // The mouse was above the scrollbar
                        setValue(0);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if (y > position.y + arrowHeight)
                    {
                        // Make sure that you didn't click on the down arrow
                        if (y <= position.y + m_size.y - arrowHeight)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((y - position.y - arrowHeight) / (m_size.y - (2 * arrowHeight))) * m_maximum);

                            // Check if you clicked above the thumb
                            if (value <= m_value)
                            {
                                float subtractValue = m_lowValue / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occured below the thumb
                            {
                                float subtractValue = m_lowValue * 2.0f / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value <= (m_maximum - m_lowValue + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_maximum - m_lowValue);
                            }
                        }
                    }

                    sf::FloatRect thumbRect = getThumbRect();

                    m_mouseDownOnThumbPos.x = x - position.x - thumbRect.left;
                    m_mouseDownOnThumbPos.y = y - position.y - thumbRect.top;
                    m_mouseDownOnThumb = true;
                }
            }
            else // the scrollbar lies horizontal
            {
                float scalingY;
                if (m_verticalImage == m_verticalScroll)
                    scalingY = m_size.y / m_textureTrackNormal_M.getSize().y;
                else
                    scalingY = m_size.y / m_textureTrackNormal_M.getSize().x;

                // Calculate the arrow width
                float arrowWidth = m_textureArrowUpNormal.getSize().y * scalingY;

                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((x - m_mouseDownOnThumbPos.x - position.x - arrowWidth) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((x - m_mouseDownOnThumbPos.x - position.x - arrowWidth) / (m_size.x - (2 * arrowWidth))) * m_maximum) + 0.5f);

                        // Set the new value
                        if (value <= (m_maximum - m_lowValue))
                            setValue(value);
                        else
                            setValue(m_maximum - m_lowValue);
                    }
                    else // The mouse was on the left of the scrollbar
                        setValue(0);
                }
                else // The click occured on the track
                {
                    // If the position is positive then calculate the correct value
                    if (x > position.x + arrowWidth)
                    {
                        // Make sure that you didn't click on the left arrow
                        if (x <= position.x + m_size.x - arrowWidth)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((x - position.x - arrowWidth) / (m_size.x - (2 * arrowWidth))) * m_maximum);

                            // Check if you clicked above the thumb
                            if (value <= m_value)
                            {
                                float subtractValue = m_lowValue / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occured below the thumb
                            {
                                float subtractValue = m_lowValue * 2.0f / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value <= (m_maximum - m_lowValue + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_maximum - m_lowValue);
                            }
                        }
                    }

                    sf::FloatRect thumbRect = getThumbRect();

                    m_mouseDownOnThumbPos.x = x - position.x - thumbRect.left;
                    m_mouseDownOnThumbPos.y = y - position.y - thumbRect.top;
                    m_mouseDownOnThumb = true;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseWheelMoved(int delta, int, int)
    {
        if (static_cast<int>(m_value) - delta < 0)
            setValue(0);
        else
            setValue(static_cast<unsigned int>(m_value - delta));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::widgetFocused()
    {
        // A scrollbar can't be focused (yet)
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "maximum")
        {
            setMaximum(std::stoi(value));
        }
        else if (property == "value")
        {
            setValue(std::stoi(value));
        }
        else if (property == "lowvalue")
        {
            setLowValue(std::stoi(value));
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
        else if (property == "autohide")
        {
            if ((value == "true") || (value == "True"))
                setAutoHide(true);
            else if ((value == "false") || (value == "False"))
                setAutoHide(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'AutoHide' property.");
        }
        else // The property didn't match
            return Widget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "maximum")
            value = std::to_string(getMaximum());
        else if (property == "value")
            value = std::to_string(getValue());
        else if (property == "lowvalue")
            value = std::to_string(getLowValue());
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
        else if (property == "autohide")
            value = m_autoHide ? "true" : "false";
        else // The property didn't match
            return Widget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Scrollbar::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Maximum", "uint"));
        list.push_back(std::pair<std::string, std::string>("Value", "uint"));
        list.push_back(std::pair<std::string, std::string>("LowValue", "uint"));
        list.push_back(std::pair<std::string, std::string>("VerticalScroll", "bool"));
        list.push_back(std::pair<std::string, std::string>("AutoHide", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::FloatRect Scrollbar::getThumbRect()
    {
        sf::FloatRect thumbRect(0, 0, m_thumbSize.x, m_thumbSize.y);

        // The scaling depends on how the scrollbar lies
        if (m_verticalScroll)
        {
            float scalingX;
            if (m_verticalImage == m_verticalScroll)
                scalingX = m_size.x / m_textureTrackNormal_M.getSize().x;
            else
                scalingX = m_size.x / m_textureTrackNormal_M.getSize().y;

            // Check if the arrows are drawn at full size
            if (m_size.y > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scalingX)
            {
                // Calculate the track and thumb height
                float realTrackHeight = m_size.y - ((m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scalingX);
                thumbRect.height = ((static_cast<float>(m_lowValue) / m_maximum) * realTrackHeight);

                // Calculate the top position of the thumb
                thumbRect.top = (m_textureArrowUpNormal.getSize().y * scalingX) + ((static_cast<float>(m_value) / (m_maximum - m_lowValue)) * (realTrackHeight - thumbRect.height));
            }
            else // The arrows are not drawn at full size
            {
                thumbRect.height = 0;
                thumbRect.top = static_cast<float>(m_textureArrowUpNormal.getSize().y);
            }
        }
        else // The scrollbar lies horizontal
        {
            float scalingY;
            if (m_verticalImage == m_verticalScroll)
                scalingY = m_size.y / m_textureTrackNormal_M.getSize().y;
            else
                scalingY = m_size.y / m_textureTrackNormal_M.getSize().x;

            // Check if the arrows are drawn at full size
            if (m_size.x > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scalingY)
            {
                // Calculate the track and thumb height
                float realTrackWidth = m_size.x - ((m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scalingY);
                thumbRect.width = ((static_cast<float>(m_lowValue) / m_maximum) * realTrackWidth);

                // Calculate the left position of the thumb
                thumbRect.left = (m_textureArrowUpNormal.getSize().y * scalingY) + ((static_cast<float>(m_value) / (m_maximum - m_lowValue)) * (realTrackWidth - thumbRect.width));
            }
            else // The arrows are not drawn at full size
            {
                thumbRect.width = 0;
                thumbRect.left = static_cast<float>(m_textureArrowUpNormal.getSize().y);
            }
        }

        return thumbRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // If the scrollbar wasn't loaded then don't draw it
        if (m_loaded == false)
            return;

        // Don't draw the loading bar when it isn't needed
        if ((m_autoHide == true) && (m_maximum <= m_lowValue))
            return;

        // Get the scale factors
        sf::Vector2f scaling;
        if (m_verticalScroll == m_verticalImage)
        {
            scaling.x = m_size.x / m_textureTrackNormal_M.getSize().x;
            scaling.y = m_size.y / m_textureTrackNormal_M.getSize().y;
        }
        else
        {
            scaling.x = m_size.x / m_textureTrackNormal_M.getSize().y;
            scaling.y = m_size.y / m_textureTrackNormal_M.getSize().x;
        }

        // Remember the current transformation
        states.transform *= getTransform();
        sf::Transform oldTransform = states.transform;

        // Set the scale of the track image
        states.transform.scale(scaling.x, scaling.y);

        // Set the rotation
        if (m_verticalScroll != m_verticalImage)
            states.transform.rotate(-90, m_textureTrackNormal_M.getSize().x * 0.5f, m_textureTrackNormal_M.getSize().x * 0.5f);

        // Draw the track image
        if (m_separateHoverImage)
        {
            if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                target.draw(m_textureTrackHover_M, states);
            else
                target.draw(m_textureTrackNormal_M, states);
        }
        else // The hover image should be drawn on top of the normal image
        {
            target.draw(m_textureTrackNormal_M, states);

            if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                target.draw(m_textureTrackHover_M, states);
        }

        // Reset the transformation (in case there was any rotation)
        states.transform = oldTransform;

        // The calculation depends on the direction of the scrollbar
        if (m_verticalScroll)
        {
            // Check if the arrows can be drawn at full size
            if (m_size.y > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scaling.x)
            {
                // Scale the arrow
                states.transform.scale(scaling.x, scaling.x);

                // Draw the first arrow
                if (m_separateHoverImage)
                {
                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureArrowUpHover, states);
                    else
                        target.draw(m_textureArrowUpNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_textureArrowUpNormal, states);

                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureArrowUpHover, states);
                }

                // Calculate the real track height (height without the arrows)
                float realTrackHeight = m_size.y - ((m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scaling.x);

                // Calculate the scaling factor
                float scaleY;
                if ((m_autoHide == false) && (m_maximum <= m_lowValue))
                    scaleY = realTrackHeight / m_thumbSize.y;
                else
                    scaleY = (((static_cast<float>(m_lowValue) / m_maximum)) * realTrackHeight) / m_thumbSize.y;

                // Set the correct transformations for the thumb
                if (m_verticalImage)
                {
                    states.transform.translate(0, m_textureArrowUpNormal.getSize().y + (m_value * (realTrackHeight / m_maximum)) / scaling.x);
                    states.transform.scale(1, scaleY);
                }
                else // The original image lies horizontal as well
                {
                    states.transform.rotate(90, m_textureThumbNormal.getSize().y * 0.5f, m_textureThumbNormal.getSize().y * 0.5f);
                    states.transform.translate(m_textureArrowUpNormal.getSize().y + (m_value * (realTrackHeight / m_maximum)) / scaling.x, 0);
                    states.transform.scale(scaleY, 1);
                }

                // Draw the thumb image
                if (m_separateHoverImage)
                {
                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureThumbHover, states);
                    else
                        target.draw(m_textureThumbNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_textureThumbNormal, states);

                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureThumbHover, states);
                }

                // Set the transformation of the second arrow
                states.transform = oldTransform;
                states.transform.translate(0, m_size.y - (m_textureArrowDownNormal.getSize().y * scaling.x));
                states.transform.scale(scaling.x, scaling.x);
            }
            else // The arrows can't be drawn at full size
            {
                // Scale the arrow
                states.transform.scale(scaling.x, (m_size.y * 0.5f) / m_textureArrowUpNormal.getSize().y);

                // Draw the first arrow
                if (m_separateHoverImage)
                {
                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureArrowUpHover, states);
                    else
                        target.draw(m_textureArrowUpNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_textureArrowUpNormal, states);

                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureArrowUpHover, states);
                }

                // Change the position of the second arrow
                states.transform.translate(0, static_cast<float>(m_textureArrowUpNormal.getSize().y));
            }
        }
        else // The scrollbar lies horizontal
        {
            // Check if the arrows can be drawn at full size
            if (m_size.x > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scaling.y)
            {
                // Scale the arrow
                states.transform.scale(scaling.y, scaling.y);

                // Rotate the arrow
                states.transform.rotate(-90, m_textureArrowUpNormal.getSize().x * 0.5f, m_textureArrowUpNormal.getSize().x * 0.5f);

                // Draw the first arrow
                if (m_separateHoverImage)
                {
                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureArrowUpHover, states);
                    else
                        target.draw(m_textureArrowUpNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_textureArrowUpNormal, states);

                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureArrowUpHover, states);
                }

                // Calculate the real track width (width without the arrows)
                float realTrackWidth = m_size.x - ((m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y) * scaling.y);

                // Calculate the scaling factor
                float scaleX;
                if ((m_autoHide == false) && (m_maximum <= m_lowValue))
                    scaleX = realTrackWidth / m_thumbSize.x;
                else
                    scaleX = (((static_cast<float>(m_lowValue) / m_maximum)) * realTrackWidth) / m_thumbSize.x;

                // Set the correct transformations for the thumb
                if (m_verticalImage)
                {
                    states.transform.translate(0, m_textureArrowUpNormal.getSize().y + (m_value * (realTrackWidth / m_maximum)) / scaling.y);
                    states.transform.scale(1, scaleX);
                }
                else // The original image lies horizontal as well
                {
                    states.transform.rotate(90, m_textureThumbNormal.getSize().y * 0.5f, m_textureThumbNormal.getSize().y * 0.5f);
                    states.transform.translate(m_textureArrowUpNormal.getSize().y + (m_value * (realTrackWidth / m_maximum)) / scaling.y, 0);
                    states.transform.scale(scaleX, 1);
                }

                // Draw the thumb image
                if (m_separateHoverImage)
                {
                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureThumbHover, states);
                    else
                        target.draw(m_textureThumbNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_textureThumbNormal, states);

                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureThumbHover, states);
                }

                // Set the transformation of the second arrow
                states.transform = oldTransform;
                states.transform.translate(m_size.x - (m_textureArrowDownNormal.getSize().y * scaling.y), 0);
                states.transform.scale(scaling.y, scaling.y);
                states.transform.rotate(-90, m_textureArrowUpNormal.getSize().x * 0.5f, m_textureArrowUpNormal.getSize().x * 0.5f);
            }
            else // The arrows can't be drawn at full size
            {
                // Scale the arrow
                states.transform.scale((m_size.x * 0.5f) / m_textureArrowUpNormal.getSize().y, scaling.y);

                // Rotate the arrow
                states.transform.rotate(-90, m_textureArrowUpNormal.getSize().x * 0.5f, m_textureArrowUpNormal.getSize().x * 0.5f);

                // Draw the first arrow
                if (m_separateHoverImage)
                {
                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureArrowUpHover, states);
                    else
                        target.draw(m_textureArrowUpNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_textureArrowUpNormal, states);

                    if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                        target.draw(m_textureArrowUpHover, states);
                }

                // Set the translation of the second arrow
                states.transform.translate(0, static_cast<float>(m_textureArrowDownNormal.getSize().y));
            }
        }

        // Draw the second arrow
        if (m_separateHoverImage)
        {
            if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                target.draw(m_textureArrowDownHover, states);
            else
                target.draw(m_textureArrowDownNormal, states);
        }
        else // The hover image should be drawn on top of the normal image
        {
            target.draw(m_textureArrowDownNormal, states);

            if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                target.draw(m_textureArrowDownHover, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
