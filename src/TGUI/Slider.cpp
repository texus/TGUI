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
    m_separateHoverImage (copy.m_separateHoverImage)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureTrackNormal, m_textureTrackNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureTrackHover, m_textureTrackHover);
        TGUI_TextureManager.copyTexture(copy.m_textureThumbNormal, m_textureThumbNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureThumbHover, m_textureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::~Slider()
    {
        if (m_textureTrackNormal.getData() != nullptr) TGUI_TextureManager.removeTexture(m_textureTrackNormal);
        if (m_textureTrackHover.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackHover);
        if (m_textureThumbNormal.getData() != nullptr) TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureThumbHover);
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
            std::swap(m_separateHoverImage,   temp.m_separateHoverImage);
            std::swap(m_textureTrackNormal,   temp.m_textureTrackNormal);
            std::swap(m_textureTrackHover,    temp.m_textureTrackHover);
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

    void Slider::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Remove all textures if they were loaded before
        if (m_textureTrackNormal.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackNormal);
        if (m_textureTrackHover.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackHover);
        if (m_textureThumbNormal.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureThumbHover);

        // Open the config file
        ConfigFile configFile(m_loadedConfigFile, "Slider");

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
            else
                throw Exception("Unrecognized property '" + it->first + "' in section Slider in " + m_loadedConfigFile + ".");
        }

        // Make sure the required textures were loaded
        if ((m_textureTrackNormal.getData() == nullptr) && (m_textureThumbNormal.getData() == nullptr))
            throw Exception("Not all needed images were loaded for the slider. Is the Slider section in " + m_loadedConfigFile + " complete?");

        setSize(m_textureTrackNormal.getImageSize().x, m_textureTrackNormal.getImageSize().y);
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

        m_textureTrackNormal.setPosition(x, y);
        m_textureTrackHover.setPosition(x, y);

        if (m_verticalScroll)
        {
            m_textureThumbNormal.setPosition(x + ((getSize().x - getThumbSize().x) / 2.0f),
                                             y - (getThumbSize().y / 2.0f) + (getSize().y / (m_maximum - m_minimum) * (m_value - m_minimum)));
        }
        else
        {
            m_textureThumbNormal.setPosition(x - (getThumbSize().x / 2.0f) + (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum)),
                                             y + ((getSize().y - getThumbSize().y) / 2.0f));
        }

        m_textureThumbHover.setPosition(m_textureThumbNormal.getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(float width, float height)
    {
        if (m_verticalImage == m_verticalScroll)
        {
            m_textureTrackNormal.setSize(width, height);

            if (m_verticalScroll)
            {
                m_textureThumbNormal.setSize(width / m_textureTrackNormal.getImageSize().x * m_textureThumbNormal.getImageSize().x,
                                             width / m_textureTrackNormal.getImageSize().x * m_textureThumbNormal.getImageSize().y);
            }
            else
            {
                m_textureThumbNormal.setSize(height / m_textureTrackNormal.getImageSize().y * m_textureThumbNormal.getImageSize().x,
                                             height / m_textureTrackNormal.getImageSize().y * m_textureThumbNormal.getImageSize().y);
            }
        }
        else
        {
            m_textureTrackNormal.setSize(height, width);

            if (m_verticalScroll)
            {
                m_textureThumbNormal.setSize(width / m_textureTrackNormal.getImageSize().y * m_textureThumbNormal.getImageSize().x,
                                             width / m_textureTrackNormal.getImageSize().y * m_textureThumbNormal.getImageSize().y);
            }
            else
            {
                m_textureThumbNormal.setSize(height / m_textureTrackNormal.getImageSize().x * m_textureThumbNormal.getImageSize().x,
                                             height / m_textureTrackNormal.getImageSize().x * m_textureThumbNormal.getImageSize().y);
            }
        }

        m_textureTrackHover.setSize(m_textureTrackNormal.getSize().x, m_textureTrackNormal.getSize().y);
        m_textureThumbHover.setSize(m_textureThumbNormal.getSize().x, m_textureThumbNormal.getSize().y);

        // Recalculate the position of the images
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider::getSize() const
    {
        if (m_verticalImage == m_verticalScroll)
            return m_textureTrackNormal.getSize();
        else
            return sf::Vector2f(m_textureTrackNormal.getSize().y, m_textureTrackNormal.getSize().x);
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

        // Recalculate the position of the thumb image
        setPosition(getPosition());
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

        // Recalculate the position of the thumb image
        setPosition(getPosition());
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

            // Recalculate the position of the thumb image
            setPosition(getPosition());
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

        m_textureTrackNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackHover.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureThumbNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureThumbHover.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the thumb
        if (sf::FloatRect(m_textureThumbNormal.getPosition().x, m_textureThumbNormal.getPosition().y, getThumbSize().x, getThumbSize().y).contains(x, y))
        {
            m_mouseDownOnThumb = true;
            m_mouseDownOnThumbPos.x = x - m_textureThumbNormal.getPosition().x;
            m_mouseDownOnThumbPos.y = y - m_textureThumbNormal.getPosition().y;
            return true;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Check if the mouse is on top of the track
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
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
        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // Check in which direction the slider goes
            if (m_verticalScroll)
            {
                // Check if the click occured on the track
                if (!m_mouseDownOnThumb)
                {
                    m_mouseDownOnThumb = true;
                    m_mouseDownOnThumbPos.x = x - m_textureThumbNormal.getPosition().x;
                    m_mouseDownOnThumbPos.y = getThumbSize().y / 2.0f;
                }

                // Set the new value
                if ((y - m_mouseDownOnThumbPos.y + (getThumbSize().y / 2.0f) - getPosition().y) > 0)
                    setValue(static_cast <unsigned int> ((((y + (getThumbSize().y / 2.0f) - m_mouseDownOnThumbPos.y - getPosition().y) / getSize().y) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                else
                    setValue(m_minimum);
            }
            else // the slider lies horizontal
            {
                // Check if the click occured on the track
                if (!m_mouseDownOnThumb)
                {
                    m_mouseDownOnThumb = true;
                    m_mouseDownOnThumbPos.x = getThumbSize().x / 2.0f;
                    m_mouseDownOnThumbPos.y = y - m_textureThumbNormal.getPosition().y;
                }

                // Set the new value
                if ((x - m_mouseDownOnThumbPos.x + (getThumbSize().x / 2.0f) - getPosition().x) > 0)
                    setValue(static_cast <unsigned int> ((((x + (getThumbSize().x / 2.0f) - m_mouseDownOnThumbPos.x - getPosition().x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum + 0.5f));
                else
                    setValue(m_minimum);
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

    void Slider::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "minimum")
        {
            setMinimum(tgui::stoi(value));
        }
        else if (property == "maximum")
        {
            setMaximum(tgui::stoi(value));
        }
        else if (property == "value")
        {
            setValue(tgui::stoi(value));
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
        else // The property didn't match
            Widget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "minimum")
            value = tgui::to_string(getMinimum());
        else if (property == "maximum")
            value = tgui::to_string(getMaximum());
        else if (property == "value")
            value = tgui::to_string(getValue());
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
            Widget::getProperty(property, value);
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

    sf::Vector2f Slider::getThumbSize() const
    {
        if (m_verticalImage == m_verticalScroll)
            return m_textureThumbNormal.getSize();
        else
            return sf::Vector2f(m_textureThumbNormal.getSize().y, m_textureThumbNormal.getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_separateHoverImage)
        {
            if (m_mouseHover)
            {
                if (m_textureTrackHover.getData())
                    target.draw(m_textureTrackHover, states);

                if (m_textureThumbHover.getData())
                    target.draw(m_textureThumbHover, states);
            }
            else
            {
                target.draw(m_textureTrackNormal, states);
                target.draw(m_textureThumbNormal, states);
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
            }
            else
                target.draw(m_textureThumbNormal, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
