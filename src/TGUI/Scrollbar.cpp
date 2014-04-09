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
    m_separateHoverImage (copy.m_separateHoverImage)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureTrackNormal, m_textureTrackNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureTrackHover, m_textureTrackHover);
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
        if (m_textureArrowUpNormal.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureArrowUpNormal);
        if (m_textureArrowUpHover.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureArrowUpHover);
        if (m_textureArrowDownNormal.getData() != nullptr) TGUI_TextureManager.removeTexture(m_textureArrowDownNormal);
        if (m_textureArrowDownHover.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureArrowDownHover);
        if (m_textureTrackNormal.getData() != nullptr)     TGUI_TextureManager.removeTexture(m_textureTrackNormal);
        if (m_textureTrackHover.getData() != nullptr)      TGUI_TextureManager.removeTexture(m_textureTrackHover);
        if (m_textureThumbNormal.getData() != nullptr)     TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.getData() != nullptr)      TGUI_TextureManager.removeTexture(m_textureThumbHover);
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
            std::swap(m_separateHoverImage,     temp.m_separateHoverImage);
            std::swap(m_textureTrackNormal,     temp.m_textureTrackNormal);
            std::swap(m_textureTrackHover,      temp.m_textureTrackHover);
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

    void Scrollbar::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Remove all textures if they were loaded before
        if (m_textureTrackNormal.getData() != nullptr)     TGUI_TextureManager.removeTexture(m_textureTrackNormal);
        if (m_textureTrackHover.getData() != nullptr)      TGUI_TextureManager.removeTexture(m_textureTrackHover);
        if (m_textureThumbNormal.getData() != nullptr)     TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.getData() != nullptr)      TGUI_TextureManager.removeTexture(m_textureThumbHover);
        if (m_textureArrowUpNormal.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureArrowUpNormal);
        if (m_textureArrowUpHover.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureArrowUpHover);
        if (m_textureArrowDownNormal.getData() != nullptr) TGUI_TextureManager.removeTexture(m_textureArrowDownNormal);
        if (m_textureArrowDownHover.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureArrowDownHover);

        // Open the config file
        ConfigFile configFile(m_loadedConfigFile, "Scrollbar");

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "separatehoverimage")
                m_separateHoverImage = configFile.readBool(it);
            else if (it->first == "verticalimage")
            {
                m_verticalImage = configFile.readBool(it);
                m_verticalScroll = m_verticalImage;
            }
            else if (it->first == "tracknormalimage")
                configFile.readTexture(it, configFileFolder, m_textureTrackNormal);
            else if (it->first == "trackhoverimage")
                configFile.readTexture(it, configFileFolder, m_textureTrackHover);
            else if (it->first == "thumbnormalimage")
                configFile.readTexture(it, configFileFolder, m_textureThumbNormal);
            else if (it->first == "thumbhoverimage")
                configFile.readTexture(it, configFileFolder, m_textureThumbHover);
            else if (it->first == "arrowupnormalimage")
                configFile.readTexture(it, configFileFolder, m_textureArrowUpNormal);
            else if (it->first == "arrowuphoverimage")
                configFile.readTexture(it, configFileFolder, m_textureArrowUpHover);
            else if (it->first == "arrowdownnormalimage")
                configFile.readTexture(it, configFileFolder, m_textureArrowDownNormal);
            else if (it->first == "arrowdownhoverimage")
                configFile.readTexture(it, configFileFolder, m_textureArrowDownHover);
            else
                throw Exception("Unrecognized property '" + it->first + "' in section Scrollbar in " + m_loadedConfigFile + ".");
        }

        // Make sure the required textures were loaded
        if ((m_textureTrackNormal.getData() == nullptr) || (m_textureThumbNormal.getData() == nullptr)
         || (m_textureArrowUpNormal.getData() == nullptr) || (m_textureArrowDownNormal.getData() == nullptr))
            throw Exception("Not all needed images were loaded for the scrollbar. Is the Scrollbar section in " + m_loadedConfigFile + " complete?");

        setSize(m_textureTrackNormal.getImageSize().x, m_textureTrackNormal.getImageSize().y);
        setVerticalScroll(m_verticalScroll);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Scrollbar::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setPosition(float x, float y)
    {
        Widget::setPosition(x, y);

        m_textureTrackNormal.setPosition(x, y);
        m_textureTrackHover.setPosition(x, y);

        m_textureArrowUpNormal.setPosition(x, y);
        m_textureArrowUpHover.setPosition(x, y);

        if (m_verticalScroll)
        {
            float realTrackHeight = getSize().y;
            realTrackHeight -= m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y;

            m_textureThumbNormal.setPosition(x, y + (realTrackHeight - getThumbSize().y) * m_value / (m_maximum - m_lowValue) + m_textureArrowUpNormal.getSize().y);
            m_textureArrowDownNormal.setPosition(x, y + getSize().y - m_textureArrowUpNormal.getSize().y);
        }
        else
        {
            float realTrackWidth = getSize().x;
            realTrackWidth -= m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y;

            m_textureThumbNormal.setPosition(x + (realTrackWidth - getThumbSize().x) * m_value / (m_maximum - m_lowValue) + m_textureArrowUpNormal.getSize().y, y);
            m_textureArrowDownNormal.setPosition(x + getSize().x - m_textureArrowUpNormal.getSize().x, y);
        }

        m_textureThumbHover.setPosition(m_textureThumbNormal.getPosition());
        m_textureArrowDownHover.setPosition(m_textureArrowDownNormal.getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setSize(float width, float height)
    {
        if (m_verticalImage == m_verticalScroll)
            m_textureTrackNormal.setSize(width, height);
        else
            m_textureTrackNormal.setSize(height, width);

        float thumbWidth;
        float realTrackSize;
        if (m_verticalScroll)
        {
            m_textureArrowUpNormal.setSize(width, width * m_textureArrowUpNormal.getImageSize().x / m_textureArrowUpNormal.getImageSize().y);
            m_textureArrowDownNormal.setSize(width, width * m_textureArrowDownNormal.getImageSize().x / m_textureArrowDownNormal.getImageSize().y);

            realTrackSize = getSize().y;
            realTrackSize -= m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y;

            if (m_verticalImage)
                thumbWidth = width;
            else
                thumbWidth = height;
        }
        else
        {
            m_textureArrowUpNormal.setSize(height * m_textureArrowUpNormal.getImageSize().x / m_textureArrowUpNormal.getImageSize().y, height);
            m_textureArrowDownNormal.setSize(height * m_textureArrowDownNormal.getImageSize().x / m_textureArrowDownNormal.getImageSize().y, height);

            realTrackSize = getSize().x;
            realTrackSize -= m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y;

            if (m_verticalImage)
                thumbWidth = height;
            else
                thumbWidth = width;
        }

        if (m_maximum > m_lowValue)
            m_textureThumbNormal.setSize(thumbWidth, realTrackSize * m_lowValue / m_maximum);
        else
            m_textureThumbNormal.setSize(thumbWidth, realTrackSize);

        m_textureTrackHover.setSize(m_textureTrackNormal.getSize().x, m_textureTrackNormal.getSize().y);
        m_textureThumbHover.setSize(m_textureThumbNormal.getSize().x, m_textureThumbNormal.getSize().y);
        m_textureArrowUpHover.setSize(m_textureArrowUpNormal.getSize().x, m_textureArrowUpNormal.getSize().y);
        m_textureArrowDownHover.setSize(m_textureArrowUpNormal.getSize().x, m_textureArrowUpNormal.getSize().y);

        // Recalculate the position of the images
        setPosition(getPosition());

        // Make sure the transparency isn't lost
        setTransparency(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Scrollbar::getSize() const
    {
        if (m_verticalImage == m_verticalScroll)
            return m_textureTrackNormal.getSize();
        else
            return sf::Vector2f(m_textureTrackNormal.getSize().y, m_textureTrackNormal.getSize().x);
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

        // Recalculate the size and position of the thumb image
        setSize(getSize().x, getSize().y);
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

            // Recalculate the size and position of the thumb image
            setSize(getSize().x, getSize().y);
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

        // Recalculate the size and position of the thumb image
        setSize(getSize().x, getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setVerticalScroll(bool verticalScroll)
    {
        // Change the internal value
        m_verticalScroll = verticalScroll;

        if (m_verticalScroll != m_verticalImage)
        {
            m_textureTrackNormal.setRotation(-90);
            m_textureTrackHover.setRotation(-90);
            m_textureThumbNormal.setRotation(-90);
            m_textureThumbHover.setRotation(-90);
        }
        else
        {
            m_textureTrackNormal.setRotation(0);
            m_textureTrackHover.setRotation(0);
            m_textureThumbNormal.setRotation(0);
            m_textureThumbHover.setRotation(0);
        }

        if (m_verticalScroll)
        {
            m_textureArrowUpNormal.setRotation(0);
            m_textureArrowUpHover.setRotation(0);
            m_textureArrowDownNormal.setRotation(0);
            m_textureArrowDownHover.setRotation(0);
        }
        else
        {
            m_textureArrowUpNormal.setRotation(-90);
            m_textureArrowUpHover.setRotation(-90);
            m_textureArrowDownNormal.setRotation(-90);
            m_textureArrowDownHover.setRotation(-90);
        }

        // Swap the width and height if needed
        sf::Vector2f size = getSize();
        if (m_verticalScroll)
        {
            if (size.x > size.y)
                setSize(size.y, size.x);
            else
                setSize(size.x, size.y);
        }
        else // The slider lies horizontal
        {
            if (size.y > size.x)
                setSize(size.y, size.x);
            else
                setSize(size.x, size.y);
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

        m_textureTrackNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackHover.setColor(sf::Color(255, 255, 255, m_opacity));

        m_textureThumbNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureThumbHover.setColor(sf::Color(255, 255, 255, m_opacity));

        m_textureArrowUpNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureArrowUpHover.setColor(sf::Color(255, 255, 255, m_opacity));

        m_textureArrowDownNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureArrowDownHover.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::mouseOnWidget(float x, float y)
    {
        // Don't make any calculations when no scrollbar is needed
        if ((m_maximum <= m_lowValue) && (m_autoHide == true))
            return false;

        // Check if the mouse is on top of the scrollbar
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
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

        if (m_verticalScroll)
        {
            // Check if the arrows are drawn at full size
            if (getSize().y > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y))
            {
                // Check if you clicked on one of the arrows
                if (y < getPosition().y + m_textureArrowUpNormal.getSize().y)
                    m_mouseDownOnArrow = true;
                else if (y > getPosition().y + getSize().y - m_textureArrowUpNormal.getSize().y)
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }
        else
        {
            // Check if the arrows are drawn at full size
            if (getSize().x > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y))
            {
                // Check if you clicked on one of the arrows
                if (x < getPosition().x + m_textureArrowUpNormal.getSize().y)
                    m_mouseDownOnArrow = true;
                else if (x > getPosition().x + getSize().x - m_textureArrowUpNormal.getSize().y)
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }

        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(m_textureThumbNormal.getPosition().x, m_textureThumbNormal.getPosition().y, getThumbSize().x, getThumbSize().y).contains(x, y))
        {
            m_mouseDownOnThumbPos.x = x - m_textureThumbNormal.getPosition().x;
            m_mouseDownOnThumbPos.y = y - m_textureThumbNormal.getPosition().y;

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
                    // Check if the arrows are drawn at full size
                    if (getSize().y > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y))
                    {
                        // Check if you clicked on the top arrow
                        if (y < getPosition().y + m_textureArrowUpNormal.getSize().y)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (y > getPosition().y + getSize().y - m_textureArrowUpNormal.getSize().y)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (y < getPosition().y + (getSize().y * 0.5f))
                            valueDown = true;
                        else // You clicked on the bottom arrow
                            valueUp = true;
                    }
                }
                else // the scrollbar lies horizontal
                {
                    // Check if the arrows are drawn at full size
                    if (getSize().x > (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y))
                    {
                        // Check if you clicked on the top arrow
                        if (x < getPosition().x + m_textureArrowUpNormal.getSize().y)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (x > getPosition().x + getSize().x - m_textureArrowUpNormal.getSize().y)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (x < getPosition().x + (getSize().x * 0.5f))
                            valueDown = true;
                        else // You clicked on the bottom arrow
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
        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;

        // Check if the mouse button went down on top of the track (or thumb)
        if ((m_mouseDown) && (m_mouseDownOnArrow == false))
        {
            // Don't continue if the calculations can't be made
            if ((m_maximum <= m_lowValue) && (m_autoHide == false))
                return;

            // Check in which direction the scrollbar lies
            if (m_verticalScroll)
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((y - m_mouseDownOnThumbPos.y - getPosition().y - m_textureArrowUpNormal.getSize().y) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((y - m_mouseDownOnThumbPos.y - getPosition().y - m_textureArrowUpNormal.getSize().y) / (getSize().y - (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y))) * m_maximum) + 0.5f);

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
                    if (y > getPosition().y + m_textureArrowUpNormal.getSize().y)
                    {
                        // Make sure that you didn't click on the down arrow
                        if (y <= getPosition().y + getSize().y - m_textureArrowUpNormal.getSize().y)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((y - getPosition().y - m_textureArrowUpNormal.getSize().y) / (getSize().y - (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y))) * m_maximum);

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

                    m_mouseDownOnThumbPos.x = x - m_textureThumbNormal.getPosition().x;
                    m_mouseDownOnThumbPos.y = y - m_textureThumbNormal.getPosition().y;
                    m_mouseDownOnThumb = true;
                }
            }
            else // the scrollbar lies horizontal
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((x - m_mouseDownOnThumbPos.x - getPosition().x - m_textureArrowUpNormal.getSize().y) > 0)
                    {
                        // Calculate the new value
                        unsigned int value = static_cast<unsigned int>((((x - m_mouseDownOnThumbPos.x - getPosition().x - m_textureArrowUpNormal.getSize().y) / (getSize().x - (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y))) * m_maximum) + 0.5f);

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
                    if (x > getPosition().x + m_textureArrowUpNormal.getSize().y)
                    {
                        // Make sure that you didn't click on the down arrow
                        if (x <= getPosition().x + getSize().x - m_textureArrowUpNormal.getSize().y)
                        {
                            // Calculate the exact position (a number between 0 and maximum)
                            float value = (((x - getPosition().x - m_textureArrowUpNormal.getSize().y) / (getSize().x - (m_textureArrowUpNormal.getSize().y + m_textureArrowDownNormal.getSize().y))) * m_maximum);

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

                    m_mouseDownOnThumbPos.x = x - m_textureThumbNormal.getPosition().x;
                    m_mouseDownOnThumbPos.y = y - m_textureThumbNormal.getPosition().y;
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

    void Scrollbar::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "maximum")
        {
            setMaximum(tgui::stoi(value));
        }
        else if (property == "value")
        {
            setValue(tgui::stoi(value));
        }
        else if (property == "lowvalue")
        {
            setLowValue(tgui::stoi(value));
        }
        else if (property == "verticalscroll")
        {
            if ((value == "true") || (value == "True"))
                setVerticalScroll(true);
            else if ((value == "false") || (value == "False"))
                setVerticalScroll(false);
            else
                throw Exception("Failed to parse 'VerticalScroll' property.");
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
                throw Exception("Failed to parse 'AutoHide' property.");
        }
        else // The property didn't match
            Widget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "maximum")
            value = tgui::to_string(getMaximum());
        else if (property == "value")
            value = tgui::to_string(getValue());
        else if (property == "lowvalue")
            value = tgui::to_string(getLowValue());
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
            Widget::getProperty(property, value);
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

    sf::Vector2f Scrollbar::getThumbSize() const
    {
        if (m_verticalImage == m_verticalScroll)
            return m_textureThumbNormal.getSize();
        else
            return sf::Vector2f(m_textureThumbNormal.getSize().y, m_textureThumbNormal.getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw the scrollbar when it isn't needed
        if ((m_autoHide == true) && (m_maximum <= m_lowValue))
            return;

        if (m_separateHoverImage)
        {
            if (m_mouseHover)
            {
                if (m_textureTrackHover.getData())
                    target.draw(m_textureTrackHover, states);

                if (m_textureThumbHover.getData())
                    target.draw(m_textureThumbHover, states);

                if (m_textureArrowUpHover.getData())
                    target.draw(m_textureArrowUpHover, states);

                if (m_textureArrowDownHover.getData())
                    target.draw(m_textureArrowDownHover, states);
            }
            else
            {
                target.draw(m_textureTrackNormal, states);
                target.draw(m_textureThumbNormal, states);
                target.draw(m_textureArrowUpNormal, states);
                target.draw(m_textureArrowDownNormal, states);
            }
        }
        else // The hover image is drawn on top of the normal one
        {
            target.draw(m_textureTrackNormal, states);

            if (m_mouseHover)
            {
                if (m_textureTrackHover.getData())
                    target.draw(m_textureTrackHover, states);

                target.draw(m_textureThumbNormal, states);
                if (m_textureThumbHover.getData())
                    target.draw(m_textureThumbHover, states);

                target.draw(m_textureArrowUpNormal, states);
                if (m_textureArrowUpHover.getData())
                    target.draw(m_textureArrowUpHover, states);

                target.draw(m_textureArrowDownNormal, states);
                if (m_textureArrowDownHover.getData())
                    target.draw(m_textureArrowDownHover, states);
            }
            else
            {
                target.draw(m_textureThumbNormal, states);
                target.draw(m_textureArrowUpNormal, states);
                target.draw(m_textureArrowDownNormal, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
