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

    Slider2d::Slider2d()
    {
        m_callback.widgetType = Type_Slider2d;
        m_draggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Remove all textures if they were loaded before
        if (m_textureTrackNormal.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureTrackNormal);
        if (m_textureTrackHover.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureTrackHover);
        if (m_textureThumbNormal.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureThumbNormal);
        if (m_textureThumbHover.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureThumbHover);

        // Open the config file
        ConfigFile configFile(m_loadedConfigFile, "Slider2d");

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
            else if (it->first == "tracknormalimage")
                configFile.readTexture(it, configFileFolder, m_textureTrackNormal);
            else if (it->first == "trackhoverimage")
                configFile.readTexture(it, configFileFolder, m_textureTrackHover);
            else if (it->first == "thumbnormalimage")
                configFile.readTexture(it, configFileFolder, m_textureThumbNormal);
            else if (it->first == "thumbhoverimage")
                configFile.readTexture(it, configFileFolder, m_textureThumbHover);
            else
                throw Exception("Unrecognized property '" + it->first + "' in section Slider2d in " + m_loadedConfigFile + ".");
        }

        // Make sure the required textures were loaded
        if ((m_textureTrackNormal.getData() == nullptr) || (m_textureThumbNormal.getData() == nullptr))
            throw Exception("Not all needed images were loaded for the slider. Is the Slider2d section in " + m_loadedConfigFile + " complete?");

        // Set the size of the slider
        m_size = sf::Vector2f(m_textureTrackNormal.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::setSize(const sf::Vector2f& size)
    {
        m_size.x = std::abs(size.x);
        m_size.y = std::abs(size.y);
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

        m_textureThumbNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureThumbHover.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureTrackHover.setColor(sf::Color(255, 255, 255, m_opacity));
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

    void Slider2d::setProperty(std::string property, const std::string& value)
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
                        setMinimum(sf::Vector2f(tgui::stof(value.substr(1, commaPos-1)),
                                                tgui::stof(value.substr(commaPos+1, value.length()-commaPos-2))));
                    }
                    else
                        throw Exception("Failed to parse 'Minimum' property.");
                }
                else
                    throw Exception("Failed to parse 'Minimum' property.");
            }
            else
                throw Exception("Failed to parse 'Minimum' property.");
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
                        setMaximum(sf::Vector2f(tgui::stof(value.substr(1, commaPos-1)),
                                                tgui::stof(value.substr(commaPos+1, value.length()-commaPos-2))));
                    }
                    else
                        throw Exception("Failed to parse 'Maximum' property.");
                }
                else
                    throw Exception("Failed to parse 'Maximum' property.");
            }
            else
                throw Exception("Failed to parse 'Maximum' property.");
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
                        setValue(sf::Vector2f(tgui::stof(value.substr(1, commaPos-1)),
                                              tgui::stof(value.substr(commaPos+1, value.length()-commaPos-2))));
                    }
                    else
                        throw Exception("Failed to parse 'Value' property.");
                }
                else
                    throw Exception("Failed to parse 'Value' property.");
            }
            else
                throw Exception("Failed to parse 'Value' property.");
        }
        else if (property == "fixedthumbsize")
        {
            if ((value == "true") || (value == "True"))
                setFixedThumbSize(true);
            else if ((value == "false") || (value == "False"))
                setFixedThumbSize(false);
            else
                throw Exception("Failed to parse 'FixedThumbSize' property.");
        }
        else if (property == "enablethumbcenter")
        {
            if ((value == "true") || (value == "True"))
                enableThumbCenter(true);
            else if ((value == "false") || (value == "False"))
                enableThumbCenter(false);
            else
                throw Exception("Failed to parse 'EnableThumbCenter' property.");
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
            ClickableWidget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider2d::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "minimum")
            value = "(" + tgui::to_string(getMinimum().x) + "," + tgui::to_string(getMinimum().y) + ")";
        else if (property == "maximum")
            value = "(" + tgui::to_string(getMaximum().x) + "," + tgui::to_string(getMaximum().y) + ")";
        else if (property == "value")
            value = "(" + tgui::to_string(getValue().x) + "," + tgui::to_string(getValue().y) + ")";
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
            ClickableWidget::getProperty(property, value);
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
        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = sf::Vector2f(((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + m_size.x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                        (getAbsolutePosition().y + m_size.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

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
            if (m_mouseHover && m_textureTrackHover.getData())
                target.draw(m_textureTrackHover, states);
            else
                target.draw(m_textureTrackNormal, states);
        }
        else // The hover image should be drawn on top of the normal image
        {
            target.draw(m_textureTrackNormal, states);

            if (m_mouseHover && m_textureTrackHover.getData())
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
            if (m_mouseHover && m_textureThumbHover.getData())
                target.draw(m_textureThumbHover, states);
            else
                target.draw(m_textureThumbNormal, states);
        }
        else // The hover image should be drawn on top of the normal image
        {
            target.draw(m_textureThumbNormal, states);

            if (m_mouseHover && m_textureThumbHover.getData())
                target.draw(m_textureThumbHover, states);
        }

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
