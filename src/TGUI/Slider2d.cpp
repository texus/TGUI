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


#include <SFML/OpenGL.hpp>

#include <TGUI/Container.hpp>
#include <TGUI/Slider2d.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::Slider2d() :
    m_minimum            (-1, -1),
    m_maximum            (1, 1),
    m_value              (0, 0),
    m_returnThumbToCenter(false),
    m_fixedThumbSize     (true),
    m_separateHoverImage (false)
    {
        m_callback.widgetType = Type_Slider2d;
        m_draggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::Slider2d(const Slider2d& copy) :
    ClickableWidget      (copy),
    m_loadedConfigFile   (copy.m_loadedConfigFile),
    m_minimum            (copy.m_minimum),
    m_maximum            (copy.m_maximum),
    m_value              (copy.m_value),
    m_returnThumbToCenter(copy.m_returnThumbToCenter),
    m_fixedThumbSize     (copy.m_fixedThumbSize),
    m_separateHoverImage (copy.m_separateHoverImage)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureTrackNormal, m_textureTrackNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureTrackHover, m_textureTrackHover);
        TGUI_TextureManager.copyTexture(copy.m_textureThumbNormal, m_textureThumbNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureThumbHover, m_textureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d::~Slider2d()
    {
        // Remove all the textures
        if (m_textureTrackNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackNormal);
        if (m_textureTrackHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackHover);
        if (m_textureThumbNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureThumbHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider2d& Slider2d::operator= (const Slider2d& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Slider2d temp(right);
            this->ClickableWidget::operator=(right);

            std::swap(m_loadedConfigFile,    temp.m_loadedConfigFile);
            std::swap(m_minimum,             temp.m_minimum);
            std::swap(m_maximum,             temp.m_maximum);
            std::swap(m_value,               temp.m_value);
            std::swap(m_returnThumbToCenter, temp.m_returnThumbToCenter);
            std::swap(m_fixedThumbSize,      temp.m_fixedThumbSize);
            std::swap(m_textureTrackNormal,  temp.m_textureTrackNormal);
            std::swap(m_textureTrackHover,   temp.m_textureTrackHover);
            std::swap(m_textureThumbNormal,  temp.m_textureThumbNormal);
            std::swap(m_textureThumbHover,   temp.m_textureThumbHover);
            std::swap(m_separateHoverImage,  temp.m_separateHoverImage);
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
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_loaded = false;

        // Remove all textures if they were loaded before
        if (m_textureTrackNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackNormal);
        if (m_textureTrackHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackHover);
        if (m_textureThumbNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_textureThumbHover);

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
        if (!configFile.read("Slider2d", properties, values))
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
            else if (property == "tracknormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackNormalImage in section Slider2d in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "trackhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureTrackHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for TrackHoverImage in section Slider2d in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureThumbNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbNormalImage in section Slider2d in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "thumbhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureThumbHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ThumbHoverImage in section Slider2d in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Slider2d in " + m_loadedConfigFile + ".");
        }

        // Make sure the required textures were loaded
        if ((m_textureTrackNormal.data != nullptr) && (m_textureThumbNormal.data != nullptr))
        {
            // Set the size of the slider
            m_size = sf::Vector2f(m_textureTrackNormal.getSize());
        }
        else
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the slider. Is the Slider2d section in " + m_loadedConfigFile + " complete?");
            return false;
        }

        // Check if optional textures were loaded
        if ((m_textureTrackHover.data != nullptr) && (m_textureThumbHover.data != nullptr))
        {
            m_widgetPhase |= WidgetPhase_Hover;
        }

        return m_loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Slider2d::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setSize(float width, float height)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_loaded == false)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Store the size of the slider
        m_size.x = width;
        m_size.y = height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setMinimum(const sf::Vector2f& minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum.x > m_maximum.x)
            m_maximum.x = m_minimum.x;
        if (m_minimum.y > m_maximum.y)
            m_maximum.y = m_minimum.y;

        // When the value is below the minimum then adjust it
        if (m_value.x < m_minimum.x)
            m_value.x = m_minimum.x;
        if (m_value.y < m_minimum.y)
            m_value.y = m_minimum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setMaximum(const sf::Vector2f& maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum.x < m_minimum.x)
            m_minimum.x = m_maximum.x;
        if (m_maximum.y < m_minimum.y)
            m_minimum.y = m_maximum.y;

        // When the value is above the maximum then adjust it
        if (m_value.x > m_maximum.x)
            m_value.x = m_maximum.x;
        if (m_value.y > m_maximum.y)
            m_value.y = m_maximum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setValue(const sf::Vector2f& value)
    {
        // Set the new value
        m_value = value;

        // When the value is below the minimum or above the maximum then adjust it
        if (m_value.x < m_minimum.x)
            m_value.x = m_minimum.x;
        else if (m_value.x > m_maximum.x)
            m_value.x = m_maximum.x;

        if (m_value.y < m_minimum.y)
            m_value.y = m_minimum.y;
        else if (m_value.y > m_maximum.y)
            m_value.y = m_maximum.y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider2d::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider2d::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Slider2d::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setFixedThumbSize(bool fixedSize)
    {
        m_fixedThumbSize = fixedSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider2d::getFixedThumbSize() const
    {
        return m_fixedThumbSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::enableThumbCenter(bool autoCenterThumb)
    {
        m_returnThumbToCenter = autoCenterThumb;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::centerThumb()
    {
        setValue(sf::Vector2f((m_maximum.x + m_minimum.x) * 0.5f, (m_maximum.y + m_minimum.y) * 0.5f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        m_textureThumbNormal.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureThumbHover.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackNormal.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackHover.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
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

        if (m_returnThumbToCenter)
        {
            setValue(sf::Vector2f((m_maximum.x + m_minimum.x) * 0.5f, (m_maximum.y + m_minimum.y) * 0.5f));

            if (m_callbackFunctions[ThumbReturnedToCenter].empty() == false)
            {
                m_callback.trigger = ThumbReturnedToCenter;
                m_callback.value2d = m_value;
                addCallback();
            }
		}
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::mouseMoved(float x, float y)
    {
        // Don't do anything when the slider wasn't loaded correctly
        if (m_loaded == false)
            return;

        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Remember the old value
        sf::Vector2f oldValue = m_value;

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // If the position is positive then calculate the correct value
            if ((y - position.y) > 0)
                m_value.y = ((y - position.y) / m_size.y) * (m_maximum.y - m_minimum.y) + m_minimum.y;
            else // The position is negative, the calculation can't be done (but is not needed)
                m_value.y = m_minimum.y;

            // If the position is positive then calculate the correct value
            if ((x - position.x) > 0)
                m_value.x = ((x - position.x) / m_size.x) * (m_maximum.x - m_minimum.x) + m_minimum.x;
            else // The position is negative, the calculation can't be done (but is not needed)
                m_value.x = m_minimum.x;

            // Set the new value, making sure that it lies within the minimum and maximum
            setValue(m_value);

            // Add the callback (if the user requested it)
            if ((oldValue != m_value) && (m_callbackFunctions[ValueChanged].empty() == false))
            {
                m_callback.trigger = ValueChanged;
                m_callback.value2d = m_value;
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
		m_mouseDown = false;

		if (m_returnThumbToCenter)
		{
		    if (m_value != sf::Vector2f((m_maximum.x + m_minimum.x) * 0.5f, (m_maximum.y + m_minimum.y) * 0.5f))
		    {
		        setValue(sf::Vector2f((m_maximum.x + m_minimum.x) * 0.5f, (m_maximum.y + m_minimum.y) * 0.5f));

                if (m_callbackFunctions[ThumbReturnedToCenter].empty() == false)
                {
                    m_callback.trigger = ThumbReturnedToCenter;
                    m_callback.value2d = m_value;
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
                        setMinimum(sf::Vector2f(std::stof(value.substr(1, commaPos-1)),
                                                std::stof(value.substr(commaPos+1, value.length()-commaPos-2))));
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
                        setMaximum(sf::Vector2f(std::stof(value.substr(1, commaPos-1)),
                                                std::stof(value.substr(commaPos+1, value.length()-commaPos-2))));
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
                        setValue(sf::Vector2f(std::stof(value.substr(1, commaPos-1)),
                                              std::stof(value.substr(commaPos+1, value.length()-commaPos-2))));
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
            value = "(" + std::to_string(getMinimum().x) + "," + std::to_string(getMinimum().y) + ")";
        else if (property == "maximum")
            value = "(" + std::to_string(getMaximum().x) + "," + std::to_string(getMaximum().y) + ")";
        else if (property == "value")
            value = "(" + std::to_string(getValue().x) + "," + std::to_string(getValue().y) + ")";
        else if (property == "fixedthumbsize")
            value = m_fixedThumbSize ? "true" : "false";
        else if (property == "enablethumbcenter")
            value = m_returnThumbToCenter ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(ValueChanged) != m_callbackFunctions.end()) && (m_callbackFunctions.at(ValueChanged).size() == 1) && (m_callbackFunctions.at(ValueChanged).front() == nullptr))
                callbacks.push_back("ValueChanged");
            if ((m_callbackFunctions.find(ThumbReturnedToCenter) != m_callbackFunctions.end()) && (m_callbackFunctions.at(ThumbReturnedToCenter).size() == 1) && (m_callbackFunctions.at(ThumbReturnedToCenter).front() == nullptr))
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
        if (m_loaded == false)
            return;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = states.transform.transformPoint(getPosition() - target.getView().getCenter() + (target.getView().getSize() / 2.f));
        sf::Vector2f bottomRightPosition = states.transform.transformPoint(getPosition() + sf::Vector2f(m_size) - target.getView().getCenter() + (target.getView().getSize() / 2.f));

        // Adjust the transformation
        states.transform *= getTransform();

        // Calculate the scale of the slider
        sf::Vector2f scaling;
        scaling.x = m_size.x / m_textureTrackNormal.getSize().x;
        scaling.y = m_size.y / m_textureTrackNormal.getSize().y;

        // Set the scale of the slider
        states.transform.scale(scaling);

        // Draw the track image
        if (m_separateHoverImage)
        {
            if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                target.draw(m_textureTrackHover, states);
            else
                target.draw(m_textureTrackNormal, states);
        }
        else // The hover image should be drawn on top of the normal image
        {
            target.draw(m_textureTrackNormal, states);

            if ((m_mouseHover) && (m_widgetPhase & WidgetPhase_Hover))
                target.draw(m_textureTrackHover, states);
        }

        // Undo the scale
        states.transform.scale(1.f / scaling.x, 1.f / scaling.y);

        // Check if the thumb should be scaled together with the slider
        if (m_fixedThumbSize)
        {
            states.transform.translate((((m_value.x - m_minimum.x) / (m_maximum.x - m_minimum.x)) * m_textureTrackNormal.getSize().x * scaling.x) - (m_textureThumbNormal.getSize().x * 0.5f),
                                       (((m_value.y - m_minimum.y) / (m_maximum.y - m_minimum.y)) * m_textureTrackNormal.getSize().y * scaling.y) - (m_textureThumbNormal.getSize().y * 0.5f));
        }
        else // The thumb must be scaled
        {
            states.transform.translate((((m_value.x - m_minimum.x) / (m_maximum.x - m_minimum.x)) * m_textureTrackNormal.getSize().x * scaling.x) - (m_textureThumbNormal.getSize().x * 0.5f * scaling.y),
                                       (((m_value.y - m_minimum.y) / (m_maximum.y - m_minimum.y)) * m_textureTrackNormal.getSize().y * scaling.y) - (m_textureThumbNormal.getSize().y * 0.5f * scaling.x));

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

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
