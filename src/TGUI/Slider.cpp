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


#include <TGUI/Container.hpp>
#include <TGUI/Slider.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider() :
    m_MouseDownOnThumb    (false),
    m_Minimum             (  0),
    m_Maximum             (100),
    m_Value               (  0),
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

        // Copy the textures
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
        // Remove all the textures
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
        m_LoadedConfigFile = configFileFilename;

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
        if (!configFile.open(configFileFilename))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + configFileFilename + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("Slider", properties, values))
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
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Slider in " + configFileFilename + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Slider in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Slider in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Slider in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_L in section Slider in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "tracknormalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_M in section Slider in " + configFileFilename + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "tracknormalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage_R in section Slider in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_L in section Slider in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_M in section Slider in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage_R in section Slider in " + configFileFilename + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Slider in " + configFileFilename + ".");
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
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider section in " + configFileFilename + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureTrackHover_L.data != nullptr) && (m_TextureTrackHover_M.data != nullptr)
             && (m_TextureTrackHover_R.data != nullptr) && (m_TextureThumbHover.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Hover;
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
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider section in " + configFileFilename + " complete?");
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

    const std::string& Slider::getLoadedConfigFile()
    {
        return m_LoadedConfigFile;
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
        else
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

        // When the value is below the minimum then adjust it
        if (m_Value < m_Minimum)
            setValue(m_Minimum);

        // The maximum can't be below the minimum
        if (m_Maximum < m_Minimum)
            m_Maximum = m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_Maximum = maximum;
        else
            m_Maximum = 1;

        // When the value is above the maximum then adjust it
        if (m_Value > m_Maximum)
            setValue(m_Maximum);

        // The minimum can't be below the maximum
        if (m_Minimum > m_Maximum)
            m_Minimum = m_Maximum;
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
                    m_Size = sf::Vector2f(m_Size.y, m_Size.x);
            }
            else // The slider lies horizontal
            {
                if (m_Size.y > m_Size.x)
                    m_Size = sf::Vector2f(m_Size.y, m_Size.x);
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
            else
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getVerticalScroll() const
    {
        return m_VerticalScroll;
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

        // Calculate the thumb size and position
        float thumbWidth, thumbHeight;
        float thumbLeft,  thumbTop;

        // Get the current position
        sf::Vector2f position = getPosition();

        // The size is different when the image is rotated
        if (m_VerticalImage == m_VerticalScroll)
        {
            thumbWidth = m_ThumbSize.x;
            thumbHeight = m_ThumbSize.y;
        }
        else
        {
            thumbWidth = m_ThumbSize.y;
            thumbHeight = m_ThumbSize.x;
        }

        // Calculate the thumb position
        if (m_VerticalScroll)
        {
            thumbLeft = (m_Size.x - thumbWidth) / 2.0f;
            thumbTop = ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.y) - (thumbHeight / 2.0f);
        }
        else // The slider lies horizontal
        {
            thumbLeft = ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.x) - (thumbWidth / 2.0f);
            thumbTop = (m_Size.y - thumbHeight) / 2.0f;
        }

        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(position.x + thumbLeft, position.y + thumbTop, thumbWidth, thumbHeight).contains(x, y))
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

    void Slider::keyPressed(sf::Keyboard::Key)
    {
        /// TODO: Respond on arrow presses
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
        m_Parent->unfocusWidget(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::setProperty(const std::string& property, const std::string& value)
    {
        if (!Widget::setProperty(property, value))
        {
            if (property == "ConfigFile")
            {
                load(value);
            }
            else if (property == "Minimum")
            {
                setMinimum(atoi(value.c_str()));
            }
            else if (property == "Maximum")
            {
                setMaximum(atoi(value.c_str()));
            }
            else if (property == "Value")
            {
                setValue(atoi(value.c_str()));
            }
            else if (property == "VerticalScroll")
            {
                if ((value == "true") || (value == "True"))
                    setVerticalScroll(true);
                else if ((value == "false") || (value == "False"))
                    setVerticalScroll(false);
                else
                    TGUI_OUTPUT("TGUI error: Failed to parse 'VerticalScroll' property.");
            }
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getProperty(const std::string& property, std::string& value)
    {
        if (!Widget::getProperty(property, value))
        {
            if (property == "ConfigFile")
                value = getLoadedConfigFile();
            else if (property == "Minimum")
                value = to_string(getMinimum());
            else if (property == "Maximum")
                value = to_string(getMaximum());
            else if (property == "Value")
                value = to_string(getValue());
            else if (property == "VerticalScroll")
                value = m_VerticalScroll ? "true" : "false";
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        sf::Vector2f scaling;

        // Apply the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Check if the image is split
        if (m_SplitImage)
        {
            // Get the scale factors
            if (m_VerticalScroll == m_VerticalImage)
            {
                scaling.x = m_Size.x / (m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_M.getSize().x + m_TextureTrackNormal_R.getSize().x);
                scaling.y = m_Size.y / m_TextureTrackNormal_M.getSize().y;
            }
            else
            {
                // Check in what direction the slider should rotate
                if ((m_VerticalImage == true) && (m_VerticalScroll == false))
                {
                    // Set the rotation
                    states.transform.rotate(-90,
                                            (m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_M.getSize().x + m_TextureTrackNormal_R.getSize().x) * 0.5f,
                                            m_TextureTrackNormal_M.getSize().x * 0.5f);
                }
                else // if ((m_VerticalImage == false) && (m_VerticalScroll == true))
                {
                    // Set the rotation
                    states.transform.rotate(90,
                                            (m_TextureTrackNormal_L.getSize().y + m_TextureTrackNormal_M.getSize().y + m_TextureTrackNormal_R.getSize().y) * 0.5f,
                                            m_TextureTrackNormal_M.getSize().y * 0.5f);
                }

                scaling.x = m_Size.x / (m_TextureTrackNormal_L.getSize().y + m_TextureTrackNormal_M.getSize().y + m_TextureTrackNormal_R.getSize().y);
                scaling.y = m_Size.y / m_TextureTrackNormal_M.getSize().x;
            }

            // Set the scale for the left image
            states.transform.scale(scaling.y, scaling.y);

            // Draw the left image
            {
                // Check if there is a separate hover image
                if (m_SeparateHoverImage)
                {
                    // Draw the correct image
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureTrackHover_L, states);
                    else
                        target.draw(m_TextureTrackNormal_L, states);
                }
                else
                {
                    // Draw the normal track image
                    target.draw(m_TextureTrackNormal_L, states);

                    // When the mouse is on top of the slider then draw the hover image
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureTrackHover_L, states);
                }

                // When the slider is focused then draw an extra image
//                if ((m_Focused) && (m_WidgetPhase & WidgetPhase_Focused))
//                    target.draw(m_SpriteFocused_L, states);
            }

            // Check if the middle image may be drawn
            if ((scaling.y * (m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_R.getSize().x))
                < scaling.x * (m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_M.getSize().x + m_TextureTrackNormal_R.getSize().x))
            {
                // Calculate the scale for our middle image
                float scaleX = (((m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_M.getSize().x + m_TextureTrackNormal_R.getSize().x)  * scaling.x)
                                 - ((m_TextureTrackNormal_L.getSize().x + m_TextureTrackNormal_R.getSize().x) * scaling.y))
                               / m_TextureTrackNormal_M.getSize().x;

                // Put the middle image on the correct position
                states.transform.translate(static_cast<float>(m_TextureTrackNormal_L.getSize().x), 0);

                // Set the scale for the middle image
                states.transform.scale(scaleX / scaling.y, 1);

                // Draw the middle image
                {
                    // Check if there is a separate hover image
                    if (m_SeparateHoverImage)
                    {
                        // Draw the correct image
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

                    // When the slider is focused then draw an extra image
//                    if ((m_Focused) && (m_WidgetPhase & WidgetPhase_Focused))
//                        target.draw(m_SpriteFocused_M, states);
                }

                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureTrackNormal_M.getSize().x), 0);

                // Set the scale for the right image
                states.transform.scale(scaling.y / scaleX, 1);
            }
            else // The middle image is not drawn
                states.transform.translate(static_cast<float>(m_TextureTrackNormal_L.getSize().x), 0);

            // Draw the right image
            {
                // Check if there is a separate hover image
                if (m_SeparateHoverImage)
                {
                    // Draw the correct image
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureTrackHover_R, states);
                    else
                        target.draw(m_TextureTrackNormal_R, states);
                }
                else
                {
                    // Draw the normal track image
                    target.draw(m_TextureTrackNormal_R, states);

                    // When the mouse is on top of the slider then draw the hover image
                    if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                        target.draw(m_TextureTrackHover_R, states);
                }

                // When the slider is focused then draw an extra image
//                if ((m_Focused) && (m_WidgetPhase & WidgetPhase_Focused))
//                    target.draw(m_SpriteFocused_R, states);
            }
        }
        else // The image is not split
        {
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
                if ((m_VerticalImage == true) && (m_VerticalScroll == false))
                    states.transform.rotate(-90, m_TextureTrackNormal_M.getSize().x * 0.5f, m_TextureTrackNormal_M.getSize().x * 0.5f);
                else // if ((m_VerticalImage == false) && (m_VerticalScroll == true))
                    states.transform.rotate(90, m_TextureTrackNormal_M.getSize().y * 0.5f, m_TextureTrackNormal_M.getSize().y * 0.5f);
            }

            // Draw the normal track image
            target.draw(m_TextureTrackNormal_M, states);

            // When the mouse is on top of the slider then draw the hover image
            if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                target.draw(m_TextureTrackHover_M, states);
        }

        // Reset the transform
        states.transform = oldTransform;

        // The thumb will be on a different position when we are scrolling vertically or not
        if (m_VerticalScroll)
        {
            // Set the translation and scale for the thumb
            states.transform.translate(static_cast<int>(m_Size.x - m_ThumbSize.x) * 0.5f,
                                       ((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.y) - (m_ThumbSize.y * 0.5f));

            // Set the scale for the thumb
            states.transform.scale(scaling.x, scaling.x);
        }
        else // the slider lies horizontal
        {
            // Set the translation and scale for the thumb
            states.transform.translate(((static_cast<float>(m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * m_Size.x) - (m_ThumbSize.x * 0.5f),
                                        (m_Size.y - m_ThumbSize.y) * 0.5f);

            // Set the scale for the thumb
            states.transform.scale(scaling.y, scaling.y);
        }

        // It is possible that the image is not drawn in the same direction than the loaded image
        if ((m_VerticalImage == true) && (m_VerticalScroll == false))
        {
            // Set the rotation
            states.transform.rotate(-90, m_TextureThumbNormal.getSize().x * 0.5f, m_TextureThumbNormal.getSize().x * 0.5f);
        }
        else if ((m_VerticalImage == false) && (m_VerticalScroll == true))
        {
            // Set the rotation
            states.transform.rotate(90, m_TextureThumbNormal.getSize().y * 0.5f, m_TextureThumbNormal.getSize().y * 0.5f);
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
