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
#include <TGUI/Knob.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    const float pi = 3.14159265358979f;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool compareFloats(float x, float y)
    {
        if (std::abs(x - y) < 0.00000001f)
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob()
    {
        m_callback.widgetType = Type_Knob;
        m_draggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // If the knob was loaded before then remove the old textures first
        if (m_backgroundTexture.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_backgroundTexture);
        if (m_foregroundTexture.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_foregroundTexture);

        // Open the config file
        ConfigFile configFile(m_loadedConfigFile, "Knob");

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "backgroundimage")
                configFile.readTexture(it, configFileFolder, m_backgroundTexture);
            else if (it->first == "foregroundimage")
                configFile.readTexture(it, configFileFolder, m_foregroundTexture);
            else if (it->first == "imagerotation")
                m_imageRotation = tgui::stof(it->second);
            else
                throw Exception("Unrecognized property '" + it->first + "' in section Knob in " + m_loadedConfigFile + ".");
        }

        // Make sure the required textures was loaded
        if ((m_backgroundTexture.getData() == nullptr) || (m_foregroundTexture.getData() == nullptr))
            throw Exception("Not all needed images were loaded for the knob. Is the Knob section in " + m_loadedConfigFile + " complete?");

        m_foregroundTexture.setRotation(m_startRotation - m_imageRotation);

        setSize(m_backgroundTexture.getImageSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setPosition(const sf::Vector2f& position)
    {
        Transformable::setPosition(position);

        m_backgroundTexture.setPosition(position);
        m_foregroundTexture.setPosition(position.x + ((m_backgroundTexture.getSize().x - m_foregroundTexture.getSize().x) / 2.0f),
                                        position.y + ((m_backgroundTexture.getSize().y - m_foregroundTexture.getSize().x) / 2.0f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setSize(const sf::Vector2f& size)
    {
        m_backgroundTexture.setSize(size);
        m_foregroundTexture.setSize({m_foregroundTexture.getImageSize().x / m_backgroundTexture.getImageSize().x * size.x,
                                     m_foregroundTexture.getImageSize().y / m_backgroundTexture.getImageSize().y * size.y});

        // Recalculate the position of the images
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setStartRotation(float startRotation)
    {
        while (startRotation >= 360)
            startRotation -= 360;
        while (startRotation < 0)
            startRotation += 360;

        m_startRotation = startRotation;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setEndRotation(float endRotation)
    {
        while (endRotation >= 360)
            endRotation -= 360;
        while (endRotation < 0)
            endRotation += 360;

        m_endRotation = endRotation;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMinimum(int minimum)
    {
        if (m_minimum != minimum)
        {
            // Set the new minimum
            m_minimum = minimum;

            // The maximum can't be below the minimum
            if (m_maximum < m_minimum)
                m_maximum = m_minimum;

            // When the value is below the minimum then adjust it
            if (m_value < m_minimum)
                setValue(m_minimum);

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMaximum(int maximum)
    {
        if (m_maximum != maximum)
        {
            // Set the new maximum
            if (maximum > 0)
                m_maximum = maximum;
            else
                m_maximum = 1;

            // The minimum can't be below the maximum
            if (m_minimum > m_maximum)
                m_minimum = m_maximum;

            // When the value is above the maximum then adjust it
            if (m_value > m_maximum)
                setValue(m_maximum);

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setValue(int value)
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

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();

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

    void Knob::setClockwiseTurning(bool clockwise)
    {
        m_clockwiseTurning = clockwise;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_backgroundTexture.setColor(sf::Color(255, 255, 255, m_opacity));
        m_foregroundTexture.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the widget
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
        {
            // Only return true when the pixel under the mouse isn't transparent
            if (!m_backgroundTexture.isTransparentPixel(x, y))
                return true;
        }

        if (m_mouseHover)
            mouseLeftWidget();

        m_mouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag
        m_mouseDown = true;

        // Change the value of the knob depending on where you clicked
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMouseReleased(float, float)
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::mouseMoved(float x, float y)
    {
        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;

        sf::Vector2f centerPosition = m_backgroundTexture.getPosition() + (m_backgroundTexture.getSize() / 2.0f);

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // Find out the direction that the knob should now point
            float angle;
            if (compareFloats(x, centerPosition.x))
            {
                if (y > centerPosition.y)
                    angle = 270;
                else if (y < centerPosition.y)
                    angle = 90;
                else // You were able to click in the exact center, ignore this click
                    return;
            }
            else
            {
                angle = std::atan2(centerPosition.y - y, x - centerPosition.x) * 180.0f / pi;
                if (angle < 0)
                    angle += 360;
            }

            // The angle might lie on a part where it isn't allowed
            if (angle > m_startRotation)
            {
                if ((angle < m_endRotation) && (m_clockwiseTurning))
                {
                    if ((angle - m_startRotation) <= (m_endRotation - angle))
                        angle = m_startRotation;
                    else
                        angle = m_endRotation;
                }
                else if (angle > m_endRotation)
                {
                    if (((m_startRotation > m_endRotation) && (m_clockwiseTurning))
                     || ((m_startRotation < m_endRotation) && (!m_clockwiseTurning)))
                    {
                        if (std::min(angle - m_startRotation, 360 - angle + m_startRotation) <= std::min(angle - m_endRotation, 360 - angle + m_endRotation))
                            angle = m_startRotation;
                        else
                            angle = m_endRotation;
                    }
                }
            }
            else if (angle < m_startRotation)
            {
                if (angle < m_endRotation)
                {
                    if (((m_startRotation > m_endRotation) && (m_clockwiseTurning))
                     || ((m_startRotation < m_endRotation) && (!m_clockwiseTurning)))
                    {
                        if (std::min(m_startRotation - angle, 360 - m_startRotation + angle) <= std::min(m_endRotation - angle, 360 -m_endRotation + angle))
                            angle = m_startRotation;
                        else
                            angle = m_endRotation;
                    }
                }
                else if ((angle > m_endRotation) && (!m_clockwiseTurning))
                {
                    if ((m_startRotation - angle) <= (angle - m_endRotation))
                        angle = m_startRotation;
                    else
                        angle = m_endRotation;
                }
            }

            // Give the image the correct rotation
            if (m_imageRotation > angle)
                m_foregroundTexture.setRotation(m_imageRotation - angle);
            else
                m_foregroundTexture.setRotation(360 - angle + m_imageRotation);

            // Calculate the difference in degrees between the start and end rotation
            float allowedAngle = 0;
            if (compareFloats(m_startRotation, m_endRotation))
                allowedAngle = 360;
            else
            {
                if (((m_endRotation > m_startRotation) && (m_clockwiseTurning))
                 || ((m_endRotation < m_startRotation) && (!m_clockwiseTurning)))
                {
                    allowedAngle = 360 - std::abs(m_endRotation - m_startRotation);
                }
                else if (((m_endRotation > m_startRotation) && (!m_clockwiseTurning))
                      || ((m_endRotation < m_startRotation) && (m_clockwiseTurning)))
                {
                    allowedAngle = std::abs(m_endRotation - m_startRotation);
                }
            }

            // Calculate the right value
            if (m_clockwiseTurning)
            {
                if (angle < m_startRotation)
                    setValue(static_cast<int>(((m_startRotation - angle) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    if (compareFloats(angle, m_startRotation))
                        setValue(m_minimum);
                    else
                        setValue(static_cast<int>((((360.0 - angle) + m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
            else // counter-clockwise
            {
                if (angle >= m_startRotation)
                    setValue(static_cast<int>(((angle - m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    setValue(static_cast<int>(((angle + (360.0 - m_startRotation)) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::widgetFocused()
    {
        // A knob can't be focused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setProperty(std::string property, const std::string& value)
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
        else if (property == "startrotation")
        {
            m_startRotation = tgui::stof(value);
        }
        else if (property == "endrotation")
        {
            m_endRotation = tgui::stof(value);
        }
        else if (property == "clockwiseturning")
        {
            if ((value == "true") || (value == "True"))
                setClockwiseTurning(true);
            else if ((value == "false") || (value == "False"))
                setClockwiseTurning(false);
            else
                throw Exception("Failed to parse 'ClockwiseTurning' property.");
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

    void Knob::getProperty(std::string property, std::string& value) const
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
        else if (property == "startrotation")
            value = tgui::to_string(m_startRotation);
        else if (property == "endrotation")
            value = tgui::to_string(m_endRotation);
        else if (property == "clockwiseturning")
            value = m_clockwiseTurning ? "true" : "false";
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

    std::list< std::pair<std::string, std::string> > Knob::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Minimum", "int"));
        list.push_back(std::pair<std::string, std::string>("Maximum", "int"));
        list.push_back(std::pair<std::string, std::string>("Value", "int"));
        list.push_back(std::pair<std::string, std::string>("StartRotation", "float"));
        list.push_back(std::pair<std::string, std::string>("EndRotation", "float"));
        list.push_back(std::pair<std::string, std::string>("ClockwiseTurning", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::recalculateRotation()
    {
        // Calculate the difference in degrees between the start and end rotation
        float allowedAngle = 0;
        if (compareFloats(m_startRotation, m_endRotation))
            allowedAngle = 360;
        else
        {
            if (((m_endRotation > m_startRotation) && (m_clockwiseTurning))
             || ((m_endRotation < m_startRotation) && (!m_clockwiseTurning)))
            {
                allowedAngle = 360 - std::abs(m_endRotation - m_startRotation);
            }
            else if (((m_endRotation > m_startRotation) && (!m_clockwiseTurning))
                  || ((m_endRotation < m_startRotation) && (m_clockwiseTurning)))
            {
                allowedAngle = std::abs(m_endRotation - m_startRotation);
            }
        }

        // Calculate the angle for the direction of the knob
        float angle;
        if (m_clockwiseTurning)
        {
            if (m_value == m_minimum)
                angle = m_startRotation;
            else
                angle = m_startRotation - (((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)) * allowedAngle);
        }
        else // counter-clockwise
        {
            angle = (((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)) * allowedAngle) + m_startRotation;
        }

        // Give the image the correct rotation
        if (m_imageRotation > angle)
            m_foregroundTexture.setRotation(m_imageRotation - angle);
        else
            m_foregroundTexture.setRotation(360 - angle + m_imageRotation);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_backgroundTexture, states);
        target.draw(m_foregroundTexture, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
