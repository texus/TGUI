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


#include <TGUI/Scrollbar.hpp>

/// \todo Support SplitImage.
/// \todo Arrow images should be allowed to point left and right. This will mess up most of the current calculations.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar() :
    m_LowValue          (6),
    m_AutoHide          (true),
    m_MouseDownOnArrow  (false)
    {
        m_Callback.widgetType = Type_Scrollbar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar(const Scrollbar& copy) :
    Slider            (copy),
    m_LowValue        (copy.m_LowValue),
    m_AutoHide        (copy.m_AutoHide),
    m_MouseDownOnArrow(copy.m_MouseDownOnArrow)
    {
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar& Scrollbar::operator= (const Scrollbar& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Scrollbar temp(right);
            this->Slider::operator=(right);

            std::swap(m_LowValue,               temp.m_LowValue);
            std::swap(m_AutoHide,               temp.m_AutoHide);
            std::swap(m_MouseDownOnArrow,       temp.m_MouseDownOnArrow);
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
        m_LoadedConfigFile = configFileFilename;

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
        if (!configFile.open(configFileFilename))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + configFileFilename + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("Scrollbar", properties, values))
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
            else if (property == "verticalscroll")
            {
                m_VerticalScroll = configFile.readBool(value, false);
                m_VerticalImage = m_VerticalScroll;
            }
            else if (property == "tracknormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowupnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowUpNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpNormalImage in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowuphoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowUpHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpHoverImage in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowdownnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowDownNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownNormalImage in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowdownhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowDownHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownHoverImage in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_L in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_M in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "tracknormalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_R in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_L in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_M in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_R in section Scrollbar in " + configFileFilename + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Scrollbar in " + configFileFilename + ".");
        }

        // Check if the image is split
        if (m_SplitImage)
        {
            TGUI_OUTPUT("TGUI error: SplitImage is not supported in Scrollbar.");
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
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the scrollbar. Is the Scrollbar section in " + configFileFilename + " complete?");
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
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the scrollbar. Is the Scrollbar section in " + configFileFilename + " complete?");
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

    void Scrollbar::setMinimum(unsigned int)
    {
        // Do nothing. The minimum may not be changed.
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setMaximum(unsigned int maximum)
    {
        Slider::setMaximum(maximum);

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

    unsigned int Scrollbar::getLowValue() const
    {
        return m_LowValue;
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
        Slider::setTransparency(transparency);

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
        {
            // Get the current position
            sf::Vector2f position = getPosition();

            float thumbLeft = 0;
            float thumbTop = 0;

            // Calculate the thumb size
            float thumbWidth = m_ThumbSize.x;
            float thumbHeight = m_ThumbSize.y;

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
                    thumbHeight = ((static_cast<float>(m_LowValue) / m_Maximum) * realTrackHeight);

                    // Calculate the top position of the thumb
                    thumbTop = (m_TextureArrowUpNormal.getSize().y * scalingX) + ((static_cast<float>(m_Value) / (m_Maximum - m_LowValue)) * (realTrackHeight - thumbHeight));
                }
                else // The arrows are not drawn at full size
                {
                    thumbHeight = 0;
                    thumbTop = static_cast<float>(m_TextureArrowUpNormal.getSize().y);
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
                    thumbWidth = ((static_cast<float>(m_LowValue) / m_Maximum) * realTrackWidth);

                    // Calculate the left position of the thumb
                    thumbLeft = (m_TextureArrowUpNormal.getSize().y * scalingY) + ((static_cast<float>(m_Value) / (m_Maximum - m_LowValue)) * (realTrackWidth - thumbWidth));
                }
                else // The arrows are not drawn at full size
                {
                    thumbWidth = 0;
                    thumbLeft = static_cast<float>(m_TextureArrowUpNormal.getSize().y);
                }
            }

            // Check if the mouse is on top of the thumb
            if (sf::FloatRect(position.x + thumbLeft, position.y + thumbTop, thumbWidth, thumbHeight).contains(x, y))
            {
                if (m_MouseDown == false)
                {
                    m_MouseDownOnThumbPos.x = x - position.x - thumbLeft;
                    m_MouseDownOnThumbPos.y = y - position.y - thumbTop;
                }

                m_MouseDownOnThumb = true;
                return true;
            }
            else // The mouse is not on top of the thumb
                m_MouseDownOnThumb = false;

            return true;
        }

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
                        {
                            if (m_Value > 0)
                                setValue(m_Value - 1);
                        }

                        // Check if you clicked the down arrow
                        else if (y > getPosition().y + m_Size.y - (m_TextureArrowUpNormal.getSize().y * scalingX))
                        {
                            if (m_Value < (m_Maximum - m_LowValue))
                                setValue(m_Value + 1);
                        }
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (y < getPosition().y + (m_TextureArrowUpNormal.getSize().y * ((m_Size.y * 0.5f) / m_TextureArrowUpNormal.getSize().y)))
                        {
                            if (m_Value > 0)
                                setValue(m_Value - 1);
                        }
                        else // You clicked on the bottom arrow
                        {
                            if (m_Value < (m_Maximum - m_LowValue))
                                setValue(m_Value + 1);
                        }
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
                        {
                            if (m_Value > 0)
                                setValue(m_Value - 1);
                        }

                        // Check if you clicked the right arrow
                        else if (x > getPosition().x + m_Size.x - (m_TextureArrowUpNormal.getSize().y * scalingY))
                        {
                            if (m_Value < (m_Maximum - m_LowValue))
                                setValue(m_Value + 1);
                        }
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (x < getPosition().x + (m_TextureArrowUpNormal.getSize().y * ((m_Size.x * 0.5f) / m_TextureArrowUpNormal.getSize().y)))
                        {
                            if (m_Value > 0)
                                setValue(m_Value - 1);
                        }
                        else // You clicked on the right arrow
                        {
                            if (m_Value < (m_Maximum - m_LowValue))
                                setValue(m_Value + 1);
                        }
                    }
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
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::setProperty(std::string property, const std::string& value)
    {
        if (!Slider::setProperty(property, value))
        {
            std::transform(property.begin(), property.end(), property.begin(), std::ptr_fun<int, int>(std::tolower));

            if (property == "lowvalue")
            {
                setLowValue(atoi(value.c_str()));
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
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getProperty(std::string property, std::string& value) const
    {
        if (!Slider::getProperty(property, value))
        {
            std::transform(property.begin(), property.end(), property.begin(), std::ptr_fun<int, int>(std::tolower));

            if (property == "lowvalue")
                value = to_string(getLowValue());
            else if (property == "autohide")
                value = m_AutoHide ? "true" : "false";
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Scrollbar::getPropertyList() const
    {
        auto list = Slider::getPropertyList();
        list.insert(list.end(), {
                                    {"LowValue", "uint"},
                                    {"AutoHide", "bool"}
                                });
        return list;
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

        sf::Vector2f scaling;

        // Apply the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Get the scale factors
        if (m_VerticalScroll == m_VerticalImage)
        {
            // Set the scaling
            scaling.x = m_Size.x / m_TextureTrackNormal_M.getSize().x;
            scaling.y = m_Size.y / m_TextureTrackNormal_M.getSize().y;
            states.transform.scale(scaling);
        }
        else
        {
            // Set the scaling
            scaling.x = m_Size.x / m_TextureTrackNormal_M.getSize().y;
            scaling.y = m_Size.y / m_TextureTrackNormal_M.getSize().x;
            states.transform.scale(scaling);

            // Set the rotation
            states.transform.rotate(-90, m_TextureTrackNormal_M.getSize().x * 0.5f, m_TextureTrackNormal_M.getSize().x * 0.5f);
        }

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

                // Reset the transformation
                states.transform = oldTransform;

                // Change the position of the second arrow
                states.transform.translate(0, m_Size.y - (m_TextureArrowDownNormal.getSize().y * scaling.x));

                // Set the scale of the arrow
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

                // Reset the transformation
                states.transform = oldTransform;

                // Change the position of the second arrow
                states.transform.translate(0, m_Size.y - (m_TextureArrowDownNormal.getSize().y * scaling.x));

                // Set the scale of the arrow
                states.transform.scale(scaling.x, (m_Size.y * 0.5f) / m_TextureArrowUpNormal.getSize().y);
            }

            // Draw the second arrow
            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureArrowUpHover, states);
                else
                    target.draw(m_TextureArrowDownNormal, states);
            }
            else // The hover image should be drawn on top of the normal image
            {
                target.draw(m_TextureArrowDownNormal, states);

                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureArrowUpHover, states);
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

                // Reset the transformation
                states.transform = oldTransform;

                // Change the position of the second arrow
                states.transform.translate(m_Size.x - (m_TextureArrowDownNormal.getSize().y * scaling.y), 0);

                // Set the scale of the arrow
                states.transform.scale(scaling.y, scaling.y);
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

                // Reset the transformation
                states.transform = oldTransform;

                // Change the position of the second arrow
                states.transform.translate(m_Size.x - (m_TextureArrowDownNormal.getSize().y * scaling.y), 0);

                // Set the scale of the arrow
                states.transform.scale((m_Size.x * 0.5f) / m_TextureArrowUpNormal.getSize().y, scaling.y);
            }

            // Rotate the arrow
            states.transform.rotate(-90, m_TextureArrowUpNormal.getSize().x * 0.5f, m_TextureArrowUpNormal.getSize().x * 0.5f);

            // Draw the second arrow
            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureArrowUpHover, states);
                else
                    target.draw(m_TextureArrowDownNormal, states);
            }
            else // The hover image should be drawn on top of the normal image
            {
                target.draw(m_TextureArrowDownNormal, states);

                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureArrowUpHover, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
