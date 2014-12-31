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


#include <SFML/OpenGL.hpp>

#include <TGUI/Container.hpp>
#include <TGUI/Slider2d.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::Slider2d() :
    m_Minimum            (-1, -1),
    m_Maximum            (1, 1),
    m_Value              (0, 0),
    m_ReturnThumbToCenter(false),
    m_FixedThumbSize     (true),
    m_SeparateHoverImage (false)
    {
        m_Callback.widgetType = Type_Slider2d;
        m_DraggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::Slider2d(const Slider2d& copy) :
    ClickableWidget      (copy),
    m_LoadedConfigFile   (copy.m_LoadedConfigFile),
    m_Minimum            (copy.m_Minimum),
    m_Maximum            (copy.m_Maximum),
    m_Value              (copy.m_Value),
    m_ReturnThumbToCenter(copy.m_ReturnThumbToCenter),
    m_FixedThumbSize     (copy.m_FixedThumbSize),
    m_SeparateHoverImage (copy.m_SeparateHoverImage)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackNormal, m_TextureTrackNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureTrackHover, m_TextureTrackHover);
        TGUI_TextureManager.copyTexture(copy.m_TextureThumbNormal, m_TextureThumbNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureThumbHover, m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::~Slider2d()
    {
        // Remove all the textures
        if (m_TextureTrackNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d& Slider2d::operator= (const Slider2d& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Slider2d temp(right);
            this->ClickableWidget::operator=(right);

            std::swap(m_LoadedConfigFile,    temp.m_LoadedConfigFile);
            std::swap(m_Minimum,             temp.m_Minimum);
            std::swap(m_Maximum,             temp.m_Maximum);
            std::swap(m_Value,               temp.m_Value);
            std::swap(m_ReturnThumbToCenter, temp.m_ReturnThumbToCenter);
            std::swap(m_FixedThumbSize,      temp.m_FixedThumbSize);
            std::swap(m_TextureTrackNormal,  temp.m_TextureTrackNormal);
            std::swap(m_TextureTrackHover,   temp.m_TextureTrackHover);
            std::swap(m_TextureThumbNormal,  temp.m_TextureThumbNormal);
            std::swap(m_TextureThumbHover,   temp.m_TextureThumbHover);
            std::swap(m_SeparateHoverImage,  temp.m_SeparateHoverImage);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d* Slider2d::clone()
    {
        return new Slider2d(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2d::load(const std::string& configFileFilename)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Remove all textures if they were loaded before
        if (m_TextureTrackNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureTrackNormal);
        if (m_TextureTrackHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureTrackHover);
        if (m_TextureThumbNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureThumbNormal);
        if (m_TextureThumbHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureThumbHover);

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
        if (!configFile.read("Slider2d", properties, values))
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
            else if (property == "tracknormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Slider2d in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureTrackHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Slider2d in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Slider2d in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Slider2d in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Slider2d in " + m_LoadedConfigFile + ".");
        }

        // Make sure the required textures were loaded
        if ((m_TextureTrackNormal.data != nullptr) && (m_TextureThumbNormal.data != nullptr))
        {
            // Set the size of the slider
            m_Size = sf::Vector2f(m_TextureTrackNormal.getSize());
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider2d section in " + m_LoadedConfigFile + " complete?");
            return false;
        }

        // Check if optional textures were loaded
        if ((m_TextureTrackHover.data != nullptr) && (m_TextureThumbHover.data != nullptr))
        {
            m_WidgetPhase |= WidgetPhase_Hover;
        }

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Slider2d::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Store the size of the slider
        m_Size.x = width;
        m_Size.y = height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setMinimum(const sf::Vector2f& minimum)
    {
        // Set the new minimum
        m_Minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_Minimum.x > m_Maximum.x)
            m_Maximum.x = m_Minimum.x;
        if (m_Minimum.y > m_Maximum.y)
            m_Maximum.y = m_Minimum.y;

        // When the value is below the minimum then adjust it
        if (m_Value.x < m_Minimum.x)
            m_Value.x = m_Minimum.x;
        if (m_Value.y < m_Minimum.y)
            m_Value.y = m_Minimum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setMaximum(const sf::Vector2f& maximum)
    {
        // Set the new maximum
        m_Maximum = maximum;

        // The maximum can never be below the minimum
        if (m_Maximum.x < m_Minimum.x)
            m_Minimum.x = m_Maximum.x;
        if (m_Maximum.y < m_Minimum.y)
            m_Minimum.y = m_Maximum.y;

        // When the value is above the maximum then adjust it
        if (m_Value.x > m_Maximum.x)
            m_Value.x = m_Maximum.x;
        if (m_Value.y > m_Maximum.y)
            m_Value.y = m_Maximum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setValue(const sf::Vector2f& value)
    {
        // Set the new value
        m_Value = value;

        // When the value is below the minimum or above the maximum then adjust it
        if (m_Value.x < m_Minimum.x)
            m_Value.x = m_Minimum.x;
        else if (m_Value.x > m_Maximum.x)
            m_Value.x = m_Maximum.x;

        if (m_Value.y < m_Minimum.y)
            m_Value.y = m_Minimum.y;
        else if (m_Value.y > m_Maximum.y)
            m_Value.y = m_Maximum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider2d::getMinimum() const
    {
        return m_Minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider2d::getMaximum() const
    {
        return m_Maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider2d::getValue() const
    {
        return m_Value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setFixedThumbSize(bool fixedSize)
    {
        m_FixedThumbSize = fixedSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2d::getFixedThumbSize() const
    {
        return m_FixedThumbSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::enableThumbCenter(bool autoCenterThumb)
    {
        m_ReturnThumbToCenter = autoCenterThumb;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::centerThumb()
    {
        setValue(sf::Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        m_TextureThumbNormal.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureThumbHover.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTrackNormal.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureTrackHover.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::leftMousePressed(float x, float y)
    {
        ClickableWidget::leftMousePressed(x, y);

        // Refresh the value
        mouseMoved(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::leftMouseReleased(float x, float y)
    {
        ClickableWidget::leftMouseReleased(x, y);

        if (m_ReturnThumbToCenter)
        {
            setValue(sf::Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));

            if (m_CallbackFunctions[ThumbReturnedToCenter].empty() == false)
            {
                m_Callback.trigger = ThumbReturnedToCenter;
                m_Callback.value2d = m_Value;
                addCallback();
            }
        }
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::mouseMoved(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        if (m_MouseHover == false)
            mouseEnteredWidget();

        m_MouseHover = true;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Remember the old value
        sf::Vector2f oldValue = m_Value;

        // Check if the mouse button is down
        if (m_MouseDown)
        {
            // If the position is positive then calculate the correct value
            if ((y - position.y) > 0)
                m_Value.y = ((y - position.y) / m_Size.y) * (m_Maximum.y - m_Minimum.y) + m_Minimum.y;
            else // The position is negative, the calculation can't be done (but is not needed)
                m_Value.y = m_Minimum.y;

            // If the position is positive then calculate the correct value
            if ((x - position.x) > 0)
                m_Value.x = ((x - position.x) / m_Size.x) * (m_Maximum.x - m_Minimum.x) + m_Minimum.x;
            else // The position is negative, the calculation can't be done (but is not needed)
                m_Value.x = m_Minimum.x;

            // Set the new value, making sure that it lies within the minimum and maximum
            setValue(m_Value);

            // Add the callback (if the user requested it)
            if ((oldValue != m_Value) && (m_CallbackFunctions[ValueChanged].empty() == false))
            {
                m_Callback.trigger = ValueChanged;
                m_Callback.value2d = m_Value;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::widgetFocused()
    {
        // A slider can't be focused (yet)
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_ReturnThumbToCenter)
        {
            if (m_Value != sf::Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f))
            {
                setValue(sf::Vector2f((m_Maximum.x + m_Minimum.x) * 0.5f, (m_Maximum.y + m_Minimum.y) * 0.5f));

                if (m_CallbackFunctions[ThumbReturnedToCenter].empty() == false)
                {
                    m_Callback.trigger = ThumbReturnedToCenter;
                    m_Callback.value2d = m_Value;
                    addCallback();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2d::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "minimum")
        {
            if (value.length() >= 5)
            {
                if ((value[0] == '(') && (value[value.length()-1] == ')'))
                {
                    std::string::size_type commaPos = value.find(',');
                    if ((commaPos != std::string::npos) && (value.find(',', commaPos + 1) == std::string::npos))
                    {
                        setMinimum(sf::Vector2f(static_cast<float>(atof(value.substr(1, commaPos-1).c_str())),
                                                static_cast<float>(atof(value.substr(commaPos+1, value.length()-commaPos-2).c_str()))));
                    }
                    else
                        TGUI_OUTPUT("TGUI error: Failed to parse 'Minimum' property.");
                }
                else
                    TGUI_OUTPUT("TGUI error: Failed to parse 'Minimum' property.");
            }
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Minimum' property.");
        }
        else if (property == "maximum")
        {
            if (value.length() >= 5)
            {
                if ((value[0] == '(') && (value[value.length()-1] == ')'))
                {
                    std::string::size_type commaPos = value.find(',');
                    if ((commaPos != std::string::npos) && (value.find(',', commaPos + 1) == std::string::npos))
                    {
                        setMaximum(sf::Vector2f(static_cast<float>(atof(value.substr(1, commaPos-1).c_str())),
                                                static_cast<float>(atof(value.substr(commaPos+1, value.length()-commaPos-2).c_str()))));
                    }
                    else
                        TGUI_OUTPUT("TGUI error: Failed to parse 'Maximum' property.");
                }
                else
                    TGUI_OUTPUT("TGUI error: Failed to parse 'Maximum' property.");
            }
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Maximum' property.");
        }
        else if (property == "value")
        {
            if (value.length() >= 5)
            {
                if ((value[0] == '(') && (value[value.length()-1] == ')'))
                {
                    std::string::size_type commaPos = value.find(',');
                    if ((commaPos != std::string::npos) && (value.find(',', commaPos + 1) == std::string::npos))
                    {
                        setValue(sf::Vector2f(static_cast<float>(atof(value.substr(1, commaPos-1).c_str())),
                                              static_cast<float>(atof(value.substr(commaPos+1, value.length()-commaPos-2).c_str()))));
                    }
                    else
                        TGUI_OUTPUT("TGUI error: Failed to parse 'Value' property.");
                }
                else
                    TGUI_OUTPUT("TGUI error: Failed to parse 'Value' property.");
            }
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Value' property.");
        }
        else if (property == "fixedthumbsize")
        {
            if ((value == "true") || (value == "True"))
                setFixedThumbSize(true);
            else if ((value == "false") || (value == "False"))
                setFixedThumbSize(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'FixedThumbSize' property.");
        }
        else if (property == "enablethumbcenter")
        {
            if ((value == "true") || (value == "True"))
                enableThumbCenter(true);
            else if ((value == "false") || (value == "False"))
                enableThumbCenter(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'EnableThumbCenter' property.");
        }
        else if (property == "callback")
        {
            ClickableWidget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "ValueChanged") || (*it == "valuechanged"))
                    bindCallback(ValueChanged);
                else if ((*it == "ThumbReturnedToCenter") || (*it == "thumbreturnedtocenter"))
                    bindCallback(ThumbReturnedToCenter);
            }
        }
        else // The property didn't match
            return ClickableWidget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2d::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "minimum")
            value = "(" + to_string(getMinimum().x) + "," + to_string(getMinimum().y) + ")";
        else if (property == "maximum")
            value = "(" + to_string(getMaximum().x) + "," + to_string(getMaximum().y) + ")";
        else if (property == "value")
            value = "(" + to_string(getValue().x) + "," + to_string(getValue().y) + ")";
        else if (property == "fixedthumbsize")
            value = m_FixedThumbSize ? "true" : "false";
        else if (property == "enablethumbcenter")
            value = m_ReturnThumbToCenter ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(ValueChanged) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(ValueChanged).size() == 1) && (m_CallbackFunctions.at(ValueChanged).front() == nullptr))
                callbacks.push_back("ValueChanged");
            if ((m_CallbackFunctions.find(ThumbReturnedToCenter) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(ThumbReturnedToCenter).size() == 1) && (m_CallbackFunctions.at(ThumbReturnedToCenter).front() == nullptr))
                callbacks.push_back("ThumbReturnedToCenter");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            return ClickableWidget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Slider2d::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Minimum", "custom"));
        list.push_back(std::pair<std::string, std::string>("Maximum", "custom"));
        list.push_back(std::pair<std::string, std::string>("Value", "custom"));
        list.push_back(std::pair<std::string, std::string>("FixedThumbSize", "bool"));
        list.push_back(std::pair<std::string, std::string>("EnableThumbCenter", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the slider wasn't loaded correctly
        if (m_Loaded == false)
            return;

        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = sf::Vector2f(((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + m_Size.x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                        (getAbsolutePosition().y + m_Size.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        // Adjust the transformation
        states.transform *= getTransform();

        // Calculate the scale of the slider
        sf::Vector2f scaling;
        scaling.x = m_Size.x / m_TextureTrackNormal.getSize().x;
        scaling.y = m_Size.y / m_TextureTrackNormal.getSize().y;

        // Set the scale of the slider
        states.transform.scale(scaling);

        // Draw the track image
        if (m_SeparateHoverImage)
        {
            if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                target.draw(m_TextureTrackHover, states);
            else
                target.draw(m_TextureTrackNormal, states);
        }
        else // The hover image should be drawn on top of the normal image
        {
            target.draw(m_TextureTrackNormal, states);

            if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                target.draw(m_TextureTrackHover, states);
        }

        // Undo the scale
        states.transform.scale(1.f / scaling.x, 1.f / scaling.y);

        // Check if the thumb should be scaled together with the slider
        if (m_FixedThumbSize)
        {
            states.transform.translate((((m_Value.x - m_Minimum.x) / (m_Maximum.x - m_Minimum.x)) * m_TextureTrackNormal.getSize().x * scaling.x) - (m_TextureThumbNormal.getSize().x * 0.5f),
                                       (((m_Value.y - m_Minimum.y) / (m_Maximum.y - m_Minimum.y)) * m_TextureTrackNormal.getSize().y * scaling.y) - (m_TextureThumbNormal.getSize().y * 0.5f));
        }
        else // The thumb must be scaled
        {
            states.transform.translate((((m_Value.x - m_Minimum.x) / (m_Maximum.x - m_Minimum.x)) * m_TextureTrackNormal.getSize().x * scaling.x) - (m_TextureThumbNormal.getSize().x * 0.5f * scaling.y),
                                       (((m_Value.y - m_Minimum.y) / (m_Maximum.y - m_Minimum.y)) * m_TextureTrackNormal.getSize().y * scaling.y) - (m_TextureThumbNormal.getSize().y * 0.5f * scaling.x));

            // Set the scale for the thumb
            states.transform.scale(scaling);
        }

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the widget outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

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

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
