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


#include <TGUI/Scrollbar.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar() :
    m_MouseDownOnThumb  (false),
    m_Maximum           (10),
    m_Value             ( 0),
    m_LowValue          ( 6),
    m_VerticalScroll    (true),
    m_VerticalImage     (true),
    m_ScrollAmount      ( 1),
    m_AutoHide          (true),
    m_MouseDownOnArrow  (false),
    m_SplitImage        (false),
    m_SeparateHoverImage(false)
    {
        m_Callback.widgetType = Type_Scrollbar;
        m_DraggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar(const Scrollbar& copy) :
    Widget               (copy),
    m_LoadedConfigFile   (copy.m_LoadedConfigFile),
    m_MouseDownOnThumb   (copy.m_MouseDownOnThumb),
    m_MouseDownOnThumbPos(copy.m_MouseDownOnThumbPos),
    m_Maximum            (copy.m_Maximum),
    m_Value              (copy.m_Value),
    m_LowValue           (copy.m_LowValue),
    m_VerticalScroll     (copy.m_VerticalScroll),
    m_VerticalImage      (copy.m_VerticalImage),
    m_ScrollAmount       (copy.m_ScrollAmount),
    m_AutoHide           (copy.m_AutoHide),
    m_MouseDownOnArrow   (copy.m_MouseDownOnArrow),
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
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowUpNormal, m_TextureArrowUpNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowUpHover, m_TextureArrowUpHover);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowDownNormal, m_TextureArrowDownNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowDownHover, m_TextureArrowDownHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::~Scrollbar()
    {
        if (m_TextureArrowUpNormal.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureArrowUpNormal);
        if (m_TextureArrowUpHover.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureArrowUpHover);
        if (m_TextureArrowDownNormal.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureArrowDownNormal);
        if (m_TextureArrowDownHover.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureArrowDownHover);
        if (m_TextureTrackNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTrackNormal_L);
        if (m_TextureTrackHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureTrackHover_L);
        if (m_TextureTrackNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTrackNormal_M);
        if (m_TextureTrackHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureTrackHover_M);
        if (m_TextureTrackNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTrackNormal_R);
        if (m_TextureTrackHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureTrackHover_R);
        if (m_TextureThumbNormal.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover.data != nullptr)      TGUI_TextureManager.removeTexture(m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar& Scrollbar::operator= (const Scrollbar& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Scrollbar temp(right);
            this->Widget::operator=(right);

            std::swap(m_LoadedConfigFile,       temp.m_LoadedConfigFile);
            std::swap(m_MouseDownOnThumb,       temp.m_MouseDownOnThumb);
            std::swap(m_MouseDownOnThumbPos,    temp.m_MouseDownOnThumbPos);
            std::swap(m_Maximum,                temp.m_Maximum);
            std::swap(m_Value,                  temp.m_Value);
            std::swap(m_LowValue,               temp.m_LowValue);
            std::swap(m_VerticalScroll,         temp.m_VerticalScroll);
            std::swap(m_VerticalImage,          temp.m_VerticalImage);
            std::swap(m_ScrollAmount,           temp.m_ScrollAmount);
            std::swap(m_AutoHide,               temp.m_AutoHide);
            std::swap(m_MouseDownOnArrow,       temp.m_MouseDownOnArrow);
            std::swap(m_SplitImage,             temp.m_SplitImage);
            std::swap(m_SeparateHoverImage,     temp.m_SeparateHoverImage);
            std::swap(m_Size,                   temp.m_Size);
            std::swap(m_ThumbSize,              temp.m_ThumbSize);
            std::swap(m_TextureTrackNormal_L,   temp.m_TextureTrackNormal_L);
            std::swap(m_TextureTrackHover_L,    temp.m_TextureTrackHover_L);
            std::swap(m_TextureTrackNormal_M,   temp.m_TextureTrackNormal_M);
            std::swap(m_TextureTrackHover_M,    temp.m_TextureTrackHover_M);
            std::swap(m_TextureTrackNormal_R,   temp.m_TextureTrackNormal_R);
            std::swap(m_TextureTrackHover_R,    temp.m_TextureTrackHover_R);
            std::swap(m_TextureThumbNormal,     temp.m_TextureThumbNormal);
            std::swap(m_TextureThumbHover,      temp.m_TextureThumbHover);
            std::swap(m_TextureArrowUpNormal,   temp.m_TextureArrowUpNormal);
            std::swap(m_TextureArrowUpHover,    temp.m_TextureArrowUpHover);
            std::swap(m_TextureArrowDownNormal, temp.m_TextureArrowDownNormal);
            std::swap(m_TextureArrowDownHover,  temp.m_TextureArrowDownHover);
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
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Remove all textures if they were loaded before
        if (m_TextureTrackNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTrackNormal_L);
        if (m_TextureTrackHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureTrackHover_L);
        if (m_TextureTrackNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTrackNormal_M);
        if (m_TextureTrackHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureTrackHover_M);
        if (m_TextureTrackNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTrackNormal_R);
        if (m_TextureTrackHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureTrackHover_R);
        if (m_TextureThumbNormal.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover.data != nullptr)      TGUI_TextureManager.removeTexture(m_TextureThumbHover);
        if (m_TextureArrowUpNormal.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureArrowUpNormal);
        if (m_TextureArrowUpHover.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureArrowUpHover);
        if (m_TextureArrowDownNormal.data != nullptr) TGUI_TextureManager.removeTexture(m_TextureArrowDownNormal);
        if (m_TextureArrowDownHover.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureArrowDownHover);

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
        if (!configFile.read("Scrollbar", properties, values))
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
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowupnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowUpNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpNormalImage in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowuphoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowUpHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpHoverImage in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowdownnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowDownNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownNormalImage in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowdownhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowDownHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownHoverImage in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_L in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_M in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "tracknormalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_R in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_L in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_M in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_R in section Scrollbar in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Scrollbar in " + m_LoadedConfigFile + ".");
        }

        // Check if the image is split
        if (m_SplitImage)
        {
            TGUI_OUTPUT("TGUI error: SplitImage is not yet supported in Scrollbar.");
            return false;
/*
            // Make sure the required textures were loaded
            if ((m_TextureTrackNormal_L.data != nullptr) && (m_TextureTrackNormal_M.data != nullptr) && (m_TextureTrackNormal_R.data != nullptr)
             && (m_TextureThumbNormal.data != nullptr) && (m_TextureArrowUpNormal.data != nullptr) && (m_TextureArrowDownNormal.data != nullptr))
            {
                m_Loaded = true;

                // Set the size of the scrollbar
                if (m_VerticalImage)
                    setSize(static_cast<float>(m_TextureTrackNormal_M.getSize().x), static_cast<float>(m_TextureTrackNormal_L.getSize().y + m_TextureTrackNormal_M.getSize().y + m_TextureTrackNormal_R.getSize().y));
                else
                    setSize(static_cast<float>(m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_M.getSize().x + m_TextureTrackNormal_R.getSize().x), static_cast<float>(m_TextureTrackNormal_M.getSize().y));

                // Set the thumb size
                m_ThumbSize = sf::Vector2f(m_TextureThumbNormal.getSize());
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the scrollbar. Is the Scrollbar section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureTrackHover_L.data != nullptr) && (m_TextureTrackHover_M.data != nullptr) && (m_TextureTrackHover_R.data != nullptr)
             && (m_TextureThumbHover.data != nullptr) && (m_TextureArrowUpHover.data != nullptr) && (m_TextureArrowDownHover.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Hover;
            }
*/
        }
        else // The image isn't split
        {
            // Make sure the required textures were loaded
            if ((m_TextureTrackNormal_M.data != nullptr) && (m_TextureThumbNormal.data != nullptr)
             && (m_TextureArrowUpNormal.data != nullptr) && (m_TextureArrowDownNormal.data != nullptr))
            {
                // Set the size of the scrollbar
                m_Loaded = true;
                setSize(static_cast<float>(m_TextureTrackNormal_M.getSize().x), static_cast<float>(m_TextureTrackNormal_M.getSize().y));
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the scrollbar. Is the Scrollbar section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureTrackHover_M.data != nullptr) && (m_TextureThumbHover.data != nullptr)
             && (m_TextureArrowUpHover.data != nullptr) && (m_TextureArrowDownHover.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Hover;
            }
        }

        // When there is no error we will return true
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Scrollbar::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setSize(float width, float height)
    {
        // Don't do anything when the scrollbar wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the size of the scrollbar
        m_Size.x = width;
        m_Size.y = height;

        // A negative size is not allowed for this widget
        if (m_Size.x < 0) m_Size.x = -m_Size.x;
        if (m_Size.y < 0) m_Size.y = -m_Size.y;

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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Scrollbar::getSize() const
    {
        if (m_Loaded)
            return sf::Vector2f(m_Size.x, m_Size.y);
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_Maximum = maximum;
        else
            m_Maximum = 1;

        // When the value is above the maximum then adjust it
        if (m_Maximum < m_LowValue)
            setValue(0);
        else if (m_Value > m_Maximum - m_LowValue)
            setValue(m_Maximum - m_LowValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setValue(unsigned int value)
    {
        if (m_Value != value)
        {
            // Set the new value
            m_Value = value;

            // When the value is above the maximum then adjust it
            if (m_Maximum < m_LowValue)
                m_Value = 0;
            else if (m_Value > m_Maximum - m_LowValue)
                m_Value = m_Maximum - m_LowValue;

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

    void Scrollbar::setLowValue(unsigned int lowValue)
    {
        // Set the new value
        m_LowValue = lowValue;

        // When the value is above the maximum then adjust it
        if (m_Maximum < m_LowValue)
            setValue(0);
        else if (m_Value > m_Maximum - m_LowValue)
            setValue(m_Maximum - m_LowValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setVerticalScroll(bool verticalScroll)
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
            else // The scrollbar lies horizontal
            {
                if (m_Size.y > m_Size.x)
                    setSize(m_Size.y, m_Size.x);
                else
                    setSize(m_Size.x, m_Size.y);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getMaximum() const
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getValue() const
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getLowValue() const
    {
        return m_LowValue;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getVerticalScroll() const
    {
        return m_VerticalScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setArrowScrollAmount(unsigned int scrollAmount)
    {
        m_ScrollAmount = scrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getArrowScrollAmount()
    {
        return m_ScrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setAutoHide(bool autoHide)
    {
        m_AutoHide = autoHide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getAutoHide() const
    {
        return m_AutoHide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setTransparency(unsigned char transparency)
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

        m_TextureArrowUpNormal.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureArrowUpHover.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));

        m_TextureArrowDownNormal.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureArrowDownHover.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the scrollbar wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Don't make any calculations when no scrollbar is needed
        if ((m_Maximum <= m_LowValue) && (m_AutoHide == true))
            return false;

        // Check if the mouse is on top of the scrollbar
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
            return true;

        if (m_MouseHover)
            mouseLeftWidget();

        // The mouse is not on top of the scrollbar
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMousePressed(float x, float y)
    {
        m_MouseDown = true;
        m_MouseDownOnArrow = false;

        // Make sure you didn't click on one of the arrows
        if (m_VerticalScroll)
        {
            float scalingX;
            if (m_VerticalImage == m_VerticalScroll)
                scalingX = m_Size.x / m_TextureTrackNormal_M.getSize().x;
            else
                scalingX = m_Size.x / m_TextureTrackNormal_M.getSize().y;

            // Check if the arrows are drawn at full size
            if (m_Size.y > (m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scalingX)
            {
                // Check if you clicked on one of the arrows
                if (y < getPosition().y + (m_TextureArrowUpNormal.getSize().y * scalingX))
                    m_MouseDownOnArrow = true;
                else if (y > getPosition().y + m_Size.y - (m_TextureArrowUpNormal.getSize().y * scalingX))
                    m_MouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_MouseDownOnArrow = true;
        }
        else // The scrollbar lies horizontal
        {
            float scalingY;
            if (m_VerticalImage == m_VerticalScroll)
                scalingY = m_Size.y / m_TextureTrackNormal_M.getSize().y;
            else
                scalingY = m_Size.y / m_TextureTrackNormal_M.getSize().x;

            // Check if the arrows are drawn at full size
            if (m_Size.x > (m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scalingY)
            {
                // Check if you clicked on one of the arrows
                if (x < getPosition().x + (m_TextureArrowUpNormal.getSize().y * scalingY))
                    m_MouseDownOnArrow = true;
                else if (x > getPosition().x + m_Size.x - (m_TextureArrowUpNormal.getSize().y * scalingY))
                    m_MouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_MouseDownOnArrow = true;
        }

        sf::Vector2f position = getPosition();
        sf::FloatRect thumbRect = getThumbRect();

        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(position.x + thumbRect.left, position.y + thumbRect.top, thumbRect.width, thumbRect.height).contains(x, y))
        {
            m_MouseDownOnThumbPos.x = x - position.x - thumbRect.left;
            m_MouseDownOnThumbPos.y = y - position.y - thumbRect.top;

            m_MouseDownOnThumb = true;
        }
        else // The mouse is not on top of the thumb
            m_MouseDownOnThumb = false;

        // Refresh the scrollbar value
        if (m_MouseDownOnArrow == false)
            mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMouseReleased(float x, float y)
    {
        // Check if one of the arrows was clicked
        if ((m_MouseDown) && (m_MouseDownOnArrow))
        {
            // Only continue when the calculations can be made
            if (m_Maximum > m_LowValue)
            {
                bool valueDown = false;
                bool valueUp = false;

                // Check in which direction the scrollbar lies
                if (m_VerticalScroll)
                {
                    float scalingX;
                    if (m_VerticalImage == m_VerticalScroll)
                        scalingX = m_Size.x / m_TextureTrackNormal_M.getSize().x;
                    else
                        scalingX = m_Size.x / m_TextureTrackNormal_M.getSize().y;

                    // Check if the arrows are drawn at full size
                    if (m_Size.y > (m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scalingX)
                    {
                        // Check if you clicked on the top arrow
                        if (y < getPosition().y + (m_TextureArrowUpNormal.getSize().y * scalingX))
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (y > getPosition().y + m_Size.y - (m_TextureArrowUpNormal.getSize().y * scalingX))
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (y < getPosition().y + (m_TextureArrowUpNormal.getSize().y * ((m_Size.y * 0.5f) / m_TextureArrowUpNormal.getSize().y)))
                            valueDown = true;
                        else // You clicked on the bottom arrow
                            valueUp = true;
                    }
                }
                else // The scrollbar lies horizontal
                {
                    float scalingY;
                    if (m_VerticalImage == m_VerticalScroll)
                        scalingY = m_Size.y / m_TextureTrackNormal_M.getSize().y;
                    else
                        scalingY = m_Size.y / m_TextureTrackNormal_M.getSize().x;

                    // Check if the arrows are drawn at full size
                    if (m_Size.x > (m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scalingY)
                    {
                        // Check if you clicked on the left arrow
                        if (x < getPosition().x + (m_TextureArrowUpNormal.getSize().y * scalingY))
                            valueDown = true;

                        // Check if you clicked the right arrow
                        else if (x > getPosition().x + m_Size.x - (m_TextureArrowUpNormal.getSize().y * scalingY))
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (x < getPosition().x + (m_TextureArrowUpNormal.getSize().y * ((m_Size.x * 0.5f) / m_TextureArrowUpNormal.getSize().y)))
                            valueDown = true;
                        else // You clicked on the right arrow
                            valueUp = true;
                    }
                }

                if (valueDown)
                {
                    if (m_Value > m_ScrollAmount)
                        setValue(m_Value - m_ScrollAmount);
                    else
                        setValue(0);
                }
                else if (valueUp)
                {
                    if (m_Value + m_ScrollAmount < m_Maximum - m_LowValue + 1)
                        setValue(m_Value + m_ScrollAmount);
                    else
                        setValue(m_Maximum - m_LowValue);
                }
            }
        }

        // The mouse is no longer down
        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseMoved(float x, float y)
    {
        // If the scrollbar wasn't loaded then do nothing
        if (m_Loaded == false)
            return;

        if (m_MouseHover == false)
            mouseEnteredWidget();

        m_MouseHover = true;

        // Check if the mouse button went down on top of the track (or thumb)
        if ((m_MouseDown) && (m_MouseDownOnArrow == false))
        {
            // Don't continue if the calculations can't be made
            if ((m_Maximum <= m_LowValue) && (m_AutoHide == false))
                return;

            // Get the current position
            sf::Vector2f position = getPosition();

            // Check in which direction the scrollbar lies
            if (m_VerticalScroll)
            {
                float scalingX;
                if (m_VerticalImage == m_VerticalScroll)
                    scalingX = m_Size.x / m_TextureTrackNormal_M.getSize().x;
                else
                    scalingX = m_Size.x / m_TextureTrackNormal_M.getSize().y;

                // Calculate the arrow height
                float arrowHeight = m_TextureArrowUpNormal.getSize().y * scalingX;

                // Check if the thumb is being dragged
                if (m_MouseDownOnThumb)
                {
                    // Set the new value
                    if ((y - m_MouseDownOnThumbPos.y - position.y - arrowHeight) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((y - m_MouseDownOnThumbPos.y - position.y - arrowHeight) / (m_Size.y - (2 * arrowHeight))) * m_Maximum) + 0.5f);

                        // If the value isn't too high then change it
                        if (value <= (m_Maximum - m_LowValue))
                            setValue(value);
                        else
                            setValue(m_Maximum - m_LowValue);
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
                        if (y <= position.y + m_Size.y - arrowHeight)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((y - position.y - arrowHeight) / (m_Size.y - (2 * arrowHeight))) * m_Maximum);

                            // Check if you clicked above the thumb
                            if (value <= m_Value)
                            {
                                float subtractValue = m_LowValue / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occured below the thumb
                            {
                                float subtractValue = m_LowValue * 2.0f / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value <= (m_Maximum - m_LowValue + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_Maximum - m_LowValue);
                            }
                        }
                    }

                    sf::FloatRect thumbRect = getThumbRect();

                    m_MouseDownOnThumbPos.x = x - position.x - thumbRect.left;
                    m_MouseDownOnThumbPos.y = y - position.y - thumbRect.top;
                    m_MouseDownOnThumb = true;
                }
            }
            else // the scrollbar lies horizontal
            {
                float scalingY;
                if (m_VerticalImage == m_VerticalScroll)
                    scalingY = m_Size.y / m_TextureTrackNormal_M.getSize().y;
                else
                    scalingY = m_Size.y / m_TextureTrackNormal_M.getSize().x;

                // Calculate the arrow width
                float arrowWidth = m_TextureArrowUpNormal.getSize().y * scalingY;

                // Check if the thumb is being dragged
                if (m_MouseDownOnThumb)
                {
                    // Set the new value
                    if ((x - m_MouseDownOnThumbPos.x - position.x - arrowWidth) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((x - m_MouseDownOnThumbPos.x - position.x - arrowWidth) / (m_Size.x - (2 * arrowWidth))) * m_Maximum) + 0.5f);

                        // Set the new value
                        if (value <= (m_Maximum - m_LowValue))
                            setValue(value);
                        else
                            setValue(m_Maximum - m_LowValue);
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
                        if (x <= position.x + m_Size.x - arrowWidth)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((x - position.x - arrowWidth) / (m_Size.x - (2 * arrowWidth))) * m_Maximum);

                            // Check if you clicked above the thumb
                            if (value <= m_Value)
                            {
                                float subtractValue = m_LowValue / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occured below the thumb
                            {
                                float subtractValue = m_LowValue * 2.0f / 3.0f;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value <= (m_Maximum - m_LowValue + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_Maximum - m_LowValue);
                            }
                        }
                    }

                    sf::FloatRect thumbRect = getThumbRect();

                    m_MouseDownOnThumbPos.x = x - position.x - thumbRect.left;
                    m_MouseDownOnThumbPos.y = y - position.y - thumbRect.top;
                    m_MouseDownOnThumb = true;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseWheelMoved(int delta, int, int)
    {
        if (static_cast<int>(m_Value) - static_cast<int>(delta * m_ScrollAmount) < 0)
            setValue(0);
        else
            setValue(static_cast<unsigned int>(m_Value - (delta * m_ScrollAmount)));
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
            setMaximum(atoi(value.c_str()));
        }
        else if (property == "value")
        {
            setValue(atoi(value.c_str()));
        }
        else if (property == "lowvalue")
        {
            setLowValue(atoi(value.c_str()));
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
            value = to_string(getMaximum());
        else if (property == "value")
            value = to_string(getValue());
        else if (property == "lowvalue")
            value = to_string(getLowValue());
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
        else if (property == "autohide")
            value = m_AutoHide ? "true" : "false";
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
        sf::FloatRect thumbRect(0, 0, m_ThumbSize.x, m_ThumbSize.y);

        // The scaling depends on how the scrollbar lies
        if (m_VerticalScroll)
        {
            float scalingX;
            if (m_VerticalImage == m_VerticalScroll)
                scalingX = m_Size.x / m_TextureTrackNormal_M.getSize().x;
            else
                scalingX = m_Size.x / m_TextureTrackNormal_M.getSize().y;

            // Check if the arrows are drawn at full size
            if (m_Size.y > (m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scalingX)
            {
                // Calculate the track and thumb height
                float realTrackHeight = m_Size.y - ((m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scalingX);
                thumbRect.height = ((static_cast<float>(m_LowValue) / m_Maximum) * realTrackHeight);

                // Calculate the top position of the thumb
                thumbRect.top = (m_TextureArrowUpNormal.getSize().y * scalingX) + ((static_cast<float>(m_Value) / (m_Maximum - m_LowValue)) * (realTrackHeight - thumbRect.height));
            }
            else // The arrows are not drawn at full size
            {
                thumbRect.height = 0;
                thumbRect.top = static_cast<float>(m_TextureArrowUpNormal.getSize().y);
            }
        }
        else // The scrollbar lies horizontal
        {
            float scalingY;
            if (m_VerticalImage == m_VerticalScroll)
                scalingY = m_Size.y / m_TextureTrackNormal_M.getSize().y;
            else
                scalingY = m_Size.y / m_TextureTrackNormal_M.getSize().x;

            // Check if the arrows are drawn at full size
            if (m_Size.x > (m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scalingY)
            {
                // Calculate the track and thumb height
                float realTrackWidth = m_Size.x - ((m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scalingY);
                thumbRect.width = ((static_cast<float>(m_LowValue) / m_Maximum) * realTrackWidth);

                // Calculate the left position of the thumb
                thumbRect.left = (m_TextureArrowUpNormal.getSize().y * scalingY) + ((static_cast<float>(m_Value) / (m_Maximum - m_LowValue)) * (realTrackWidth - thumbRect.width));
            }
            else // The arrows are not drawn at full size
            {
                thumbRect.width = 0;
                thumbRect.left = static_cast<float>(m_TextureArrowUpNormal.getSize().y);
            }
        }

        return thumbRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // If the scrollbar wasn't loaded then don't draw it
        if (m_Loaded == false)
            return;

        // Don't draw the loading bar when it isn't needed
        if ((m_AutoHide == true) && (m_Maximum <= m_LowValue))
            return;

        // Get the scale factors
        sf::Vector2f scaling;
        if (m_VerticalScroll == m_VerticalImage)
        {
            scaling.x = m_Size.x / m_TextureTrackNormal_M.getSize().x;
            scaling.y = m_Size.y / m_TextureTrackNormal_M.getSize().y;
        }
        else
        {
            scaling.x = m_Size.x / m_TextureTrackNormal_M.getSize().y;
            scaling.y = m_Size.y / m_TextureTrackNormal_M.getSize().x;
        }

        // Remember the current transformation
        states.transform *= getTransform();
        sf::Transform oldTransform = states.transform;

        // Set the scale of the track image
        states.transform.scale(scaling.x, scaling.y);

        // Set the rotation
        if (m_VerticalScroll != m_VerticalImage)
            states.transform.rotate(-90, m_TextureTrackNormal_M.getSize().x * 0.5f, m_TextureTrackNormal_M.getSize().x * 0.5f);

        // Draw the track image
        if (m_SeparateHoverImage)
        {
            if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                target.draw(m_TextureTrackHover_M, states);
            else
                target.draw(m_TextureTrackNormal_M, states);
        }
        else // The hover image should be drawn on top of the normal image
        {
            target.draw(m_TextureTrackNormal_M, states);

            if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                target.draw(m_TextureTrackHover_M, states);
        }

        // Reset the transformation (in case there was any rotation)
        states.transform = oldTransform;

        // The calculation depends on the direction of the scrollbar
        if (m_VerticalScroll)
        {
            // Check if the arrows can be drawn at full size
            if (m_Size.y > (m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scaling.x)
            {
                // Scale the arrow
                states.transform.scale(scaling.x, scaling.x);

                // Draw the first arrow
                if (m_SeparateHoverImage)
                {
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureArrowUpHover, states);
                    else
                        target.draw(m_TextureArrowUpNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_TextureArrowUpNormal, states);

                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureArrowUpHover, states);
                }

                // Calculate the real track height (height without the arrows)
                float realTrackHeight = m_Size.y - ((m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scaling.x);

                // Calculate the scaling factor
                float scaleY;
                if ((m_AutoHide == false) && (m_Maximum <= m_LowValue))
                    scaleY = realTrackHeight / m_ThumbSize.y;
                else
                    scaleY = (((static_cast<float>(m_LowValue) / m_Maximum)) * realTrackHeight) / m_ThumbSize.y;

                // Set the correct transformations for the thumb
                if (m_VerticalImage)
                {
                    states.transform.translate(0, m_TextureArrowUpNormal.getSize().y + (m_Value * (realTrackHeight / m_Maximum)) / scaling.x);
                    states.transform.scale(1, scaleY);
                }
                else // The original image lies horizontal as well
                {
                    states.transform.rotate(90, m_TextureThumbNormal.getSize().y * 0.5f, m_TextureThumbNormal.getSize().y * 0.5f);
                    states.transform.translate(m_TextureArrowUpNormal.getSize().y + (m_Value * (realTrackHeight / m_Maximum)) / scaling.x, 0);
                    states.transform.scale(scaleY, 1);
                }

                // Draw the thumb image
                if (m_SeparateHoverImage)
                {
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureThumbHover, states);
                    else
                        target.draw(m_TextureThumbNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_TextureThumbNormal, states);

                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureThumbHover, states);
                }

                // Set the transformation of the second arrow
                states.transform = oldTransform;
                states.transform.translate(0, m_Size.y - (m_TextureArrowDownNormal.getSize().y * scaling.x));
                states.transform.scale(scaling.x, scaling.x);
            }
            else // The arrows can't be drawn at full size
            {
                // Scale the arrow
                states.transform.scale(scaling.x, (m_Size.y * 0.5f) / m_TextureArrowUpNormal.getSize().y);

                // Draw the first arrow
                if (m_SeparateHoverImage)
                {
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureArrowUpHover, states);
                    else
                        target.draw(m_TextureArrowUpNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_TextureArrowUpNormal, states);

                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureArrowUpHover, states);
                }

                // Change the position of the second arrow
                states.transform.translate(0, static_cast<float>(m_TextureArrowUpNormal.getSize().y));
            }
        }
        else // The scrollbar lies horizontal
        {
            // Check if the arrows can be drawn at full size
            if (m_Size.x > (m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scaling.y)
            {
                // Scale the arrow
                states.transform.scale(scaling.y, scaling.y);

                // Rotate the arrow
                states.transform.rotate(-90, m_TextureArrowUpNormal.getSize().x * 0.5f, m_TextureArrowUpNormal.getSize().x * 0.5f);

                // Draw the first arrow
                if (m_SeparateHoverImage)
                {
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureArrowUpHover, states);
                    else
                        target.draw(m_TextureArrowUpNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_TextureArrowUpNormal, states);

                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureArrowUpHover, states);
                }

                // Calculate the real track width (width without the arrows)
                float realTrackWidth = m_Size.x - ((m_TextureArrowUpNormal.getSize().y + m_TextureArrowDownNormal.getSize().y) * scaling.y);

                // Calculate the scaling factor
                float scaleX;
                if ((m_AutoHide == false) && (m_Maximum <= m_LowValue))
                    scaleX = realTrackWidth / m_ThumbSize.x;
                else
                    scaleX = (((static_cast<float>(m_LowValue) / m_Maximum)) * realTrackWidth) / m_ThumbSize.x;

                // Set the correct transformations for the thumb
                if (m_VerticalImage)
                {
                    states.transform.translate(0, m_TextureArrowUpNormal.getSize().y + (m_Value * (realTrackWidth / m_Maximum)) / scaling.y);
                    states.transform.scale(1, scaleX);
                }
                else // The original image lies horizontal as well
                {
                    states.transform.rotate(90, m_TextureThumbNormal.getSize().y * 0.5f, m_TextureThumbNormal.getSize().y * 0.5f);
                    states.transform.translate(m_TextureArrowUpNormal.getSize().y + (m_Value * (realTrackWidth / m_Maximum)) / scaling.y, 0);
                    states.transform.scale(scaleX, 1);
                }

                // Draw the thumb image
                if (m_SeparateHoverImage)
                {
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureThumbHover, states);
                    else
                        target.draw(m_TextureThumbNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_TextureThumbNormal, states);

                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureThumbHover, states);
                }

                // Set the transformation of the second arrow
                states.transform = oldTransform;
                states.transform.translate(m_Size.x - (m_TextureArrowDownNormal.getSize().y * scaling.y), 0);
                states.transform.scale(scaling.y, scaling.y);
                states.transform.rotate(-90, m_TextureArrowUpNormal.getSize().x * 0.5f, m_TextureArrowUpNormal.getSize().x * 0.5f);
            }
            else // The arrows can't be drawn at full size
            {
                // Scale the arrow
                states.transform.scale((m_Size.x * 0.5f) / m_TextureArrowUpNormal.getSize().y, scaling.y);

                // Rotate the arrow
                states.transform.rotate(-90, m_TextureArrowUpNormal.getSize().x * 0.5f, m_TextureArrowUpNormal.getSize().x * 0.5f);

                // Draw the first arrow
                if (m_SeparateHoverImage)
                {
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureArrowUpHover, states);
                    else
                        target.draw(m_TextureArrowUpNormal, states);
                }
                else // The hover image should be drawn on top of the normal image
                {
                    target.draw(m_TextureArrowUpNormal, states);

                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureArrowUpHover, states);
                }

                // Set the translation of the second arrow
                states.transform.translate(0, static_cast<float>(m_TextureArrowDownNormal.getSize().y));
            }
        }

        // Draw the second arrow
        if (m_SeparateHoverImage)
        {
            if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                target.draw(m_TextureArrowDownHover, states);
            else
                target.draw(m_TextureArrowDownNormal, states);
        }
        else // The hover image should be drawn on top of the normal image
        {
            target.draw(m_TextureArrowDownNormal, states);

            if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                target.draw(m_TextureArrowDownHover, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
