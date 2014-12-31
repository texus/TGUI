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


#include <cmath>

#include <TGUI/Container.hpp>
#include <TGUI/Knob.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    const float pi = 3.14159265358979f;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob() :
        m_clockwiseTurning(true),
        m_imageRotation(0),
        m_startRotation(90),
        m_endRotation(90),
        m_minimum(0),
        m_value(0),
        m_maximum(360)
    {
        m_Callback.widgetType = Type_Knob;
        m_DraggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob(const Knob& copy) :
        Widget            (copy),
        m_loadedConfigFile(copy.m_loadedConfigFile),
        m_size            (copy.m_size),
        m_clockwiseTurning(copy.m_clockwiseTurning),
        m_imageRotation   (copy.m_imageRotation),
        m_startRotation   (copy.m_startRotation),
        m_endRotation     (copy.m_endRotation),
        m_minimum         (copy.m_minimum),
        m_value           (copy.m_value),
        m_maximum         (copy.m_maximum)
    {
        TGUI_TextureManager.copyTexture(copy.m_backgroundTexture, m_backgroundTexture);
        TGUI_TextureManager.copyTexture(copy.m_foregroundTexture, m_foregroundTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::~Knob()
    {
        if (m_backgroundTexture.data != nullptr)  TGUI_TextureManager.removeTexture(m_backgroundTexture);
        if (m_foregroundTexture.data != nullptr)  TGUI_TextureManager.removeTexture(m_foregroundTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob& Knob::operator= (const Knob& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Knob temp(right);
            this->Widget::operator=(right);

            std::swap(m_loadedConfigFile,  temp.m_loadedConfigFile);
            std::swap(m_backgroundTexture, temp.m_backgroundTexture);
            std::swap(m_foregroundTexture, temp.m_foregroundTexture);
            std::swap(m_size,              temp.m_size);
            std::swap(m_clockwiseTurning,  temp.m_clockwiseTurning);
            std::swap(m_imageRotation,     temp.m_imageRotation);
            std::swap(m_startRotation,     temp.m_startRotation);
            std::swap(m_endRotation,       temp.m_endRotation);
            std::swap(m_minimum,           temp.m_minimum);
            std::swap(m_value,             temp.m_value);
            std::swap(m_maximum,           temp.m_maximum);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob* Knob::clone()
    {
        return new Knob(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If the knob was loaded before then remove the old textures first
        if (m_backgroundTexture.data != nullptr)  TGUI_TextureManager.removeTexture(m_backgroundTexture);
        if (m_foregroundTexture.data != nullptr)  TGUI_TextureManager.removeTexture(m_foregroundTexture);

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
        if (!configFile.read("Knob", properties, values))
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

            if (property == "backgroundimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_backgroundTexture))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for BackgroundImage in section Knob in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "foregroundimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_foregroundTexture))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ForegroundImage in section Knob in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "imagerotation")
            {
                m_imageRotation = static_cast<float>(atof(value.c_str()));
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Knob in " + m_loadedConfigFile + ".");
        }

        // Make sure the required textures was loaded
        if ((m_backgroundTexture.data != nullptr) && (m_foregroundTexture.data != nullptr))
        {
            // Rotate the image
            m_foregroundTexture.sprite.setOrigin(m_foregroundTexture.getSize().x / 2.0f, m_foregroundTexture.getSize().y / 2.0f);
            m_foregroundTexture.sprite.setRotation(m_startRotation - m_imageRotation);

            m_Loaded = true;
            setSize(static_cast<float>(m_backgroundTexture.getSize().x), static_cast<float>(m_backgroundTexture.getSize().y));
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the knob. Is the Knob section in " + m_loadedConfigFile + " complete?");
            return false;
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Knob::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setPosition(float x, float y)
    {
        Transformable::setPosition(x, y);

        m_backgroundTexture.sprite.setPosition(x, y);
        m_foregroundTexture.sprite.setPosition(x + (m_backgroundTexture.getSize().x / 2.0f),
                                               y + (m_backgroundTexture.getSize().y / 2.0f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setSize(float width, float height)
    {
        // Don't do anything when the knob wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new size of the knob
        m_size.x = width;
        m_size.y = height;

        // A negative size is not allowed for this widget
        if (m_size.x < 0) m_size.x = -m_size.x;
        if (m_size.y < 0) m_size.y = -m_size.y;

        m_backgroundTexture.sprite.setScale(m_size.x / m_backgroundTexture.getSize().x, m_size.y / m_backgroundTexture.getSize().y);
        m_foregroundTexture.sprite.setScale(m_size.x / m_backgroundTexture.getSize().x, m_size.y / m_backgroundTexture.getSize().y);

        // Reposition the foreground image
        m_foregroundTexture.sprite.setPosition(getPosition().x + (m_size.x / 2.0f), getPosition().y + (m_size.y / 2.0f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Knob::getSize() const
    {
        return m_size;
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
            if (m_CallbackFunctions[ValueChanged].empty() == false)
            {
                m_Callback.trigger = ValueChanged;
                m_Callback.value   = static_cast<int>(m_value);
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setClockwiseTurning(bool clockwise)
    {
        m_clockwiseTurning = clockwise;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::getClockwiseTurning()
    {
        return m_clockwiseTurning;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_backgroundTexture.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_foregroundTexture.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the widget
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
        {
            sf::Vector2f scaling;
            scaling.x = m_size.x / m_backgroundTexture.getSize().x;
            scaling.y = m_size.y / m_backgroundTexture.getSize().y;

            // Only return true when the pixel under the mouse isn't transparent
            if (!m_backgroundTexture.isTransparentPixel(static_cast<unsigned int>((x - getPosition().x) / scaling.x), static_cast<unsigned int>((y - getPosition().y) / scaling.y)))
                return true;
        }

        if (m_MouseHover)
            mouseLeftWidget();

        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag
        m_MouseDown = true;

        // Change the value of the knob depending on where you clicked
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMouseReleased(float, float)
    {
        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::mouseMoved(float x, float y)
    {
        // Don't do anything when the knob wasn't loaded correctly
        if (m_Loaded == false)
            return;

        if (m_MouseHover == false)
            mouseEnteredWidget();

        m_MouseHover = true;

        // Get the current position
        sf::Vector2f centerPosition = getPosition() + sf::Vector2f(m_size.x / 2.0f, m_size.y / 2.0f);

        // Check if the mouse button is down
        if (m_MouseDown)
        {
            // Find out the direction that the knob should now point
            float angle;
            if (x == centerPosition.x)
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
                m_foregroundTexture.sprite.setRotation(m_imageRotation - angle);
            else
                m_foregroundTexture.sprite.setRotation(360 - angle + m_imageRotation);

            // Calculate the difference in degrees between the start and end rotation
            float allowedAngle = 0;
            if (m_startRotation == m_endRotation)
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
                    if (angle == m_startRotation)
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

    bool Knob::setProperty(std::string property, const std::string& value)
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
        else if (property == "startrotation")
        {
            m_startRotation = static_cast<float>(atof(value.c_str()));
        }
        else if (property == "endrotation")
        {
            m_endRotation = static_cast<float>(atof(value.c_str()));
        }
        else if (property == "clockwiseturning")
        {
            if ((value == "true") || (value == "True"))
                setClockwiseTurning(true);
            else if ((value == "false") || (value == "False"))
                setClockwiseTurning(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'ClockwiseTurning' property.");
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

    bool Knob::getProperty(std::string property, std::string& value) const
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
        else if (property == "startrotation")
            value = to_string(m_startRotation);
        else if (property == "endrotation")
            value = to_string(m_endRotation);
        else if (property == "clockwiseturning")
            value = m_clockwiseTurning ? "true" : "false";
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
        if (m_startRotation == m_endRotation)
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
            m_foregroundTexture.sprite.setRotation(m_imageRotation - angle);
        else
            m_foregroundTexture.sprite.setRotation(360 - angle + m_imageRotation);
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
