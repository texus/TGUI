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
#include <TGUI/LoadingBar.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar::LoadingBar() :
    m_minimum       (  0),
    m_maximum       (100),
    m_value         (  0),
    m_splitImage    (false),
    m_textSize      (0)
    {
        m_callback.widgetType = Type_LoadingBar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar::LoadingBar(const LoadingBar& copy) :
    ClickableWidget   (copy),
    m_loadedConfigFile(copy.m_loadedConfigFile),
    m_minimum         (copy.m_minimum),
    m_maximum         (copy.m_maximum),
    m_value           (copy.m_value),
    m_splitImage      (copy.m_splitImage),
    m_text            (copy.m_text),
    m_textSize        (copy.m_textSize)
    {
        // Copy the textures
        TGUI_TextureManager.copyTexture(copy.m_textureBack_L, m_textureBack_L);
        TGUI_TextureManager.copyTexture(copy.m_textureBack_M, m_textureBack_M);
        TGUI_TextureManager.copyTexture(copy.m_textureBack_R, m_textureBack_R);
        TGUI_TextureManager.copyTexture(copy.m_textureFront_L, m_textureFront_L);
        TGUI_TextureManager.copyTexture(copy.m_textureFront_M, m_textureFront_M);
        TGUI_TextureManager.copyTexture(copy.m_textureFront_R, m_textureFront_R);

        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar::~LoadingBar()
    {
        if (m_textureBack_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureBack_L);
        if (m_textureBack_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureBack_M);
        if (m_textureBack_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureBack_R);

        if (m_textureFront_L.data != nullptr) TGUI_TextureManager.removeTexture(m_textureFront_L);
        if (m_textureFront_M.data != nullptr) TGUI_TextureManager.removeTexture(m_textureFront_M);
        if (m_textureFront_R.data != nullptr) TGUI_TextureManager.removeTexture(m_textureFront_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar& LoadingBar::operator= (const LoadingBar& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            LoadingBar temp(right);
            this->ClickableWidget::operator=(right);

            std::swap(m_loadedConfigFile, temp.m_loadedConfigFile);
            std::swap(m_minimum,          temp.m_minimum);
            std::swap(m_maximum,          temp.m_maximum);
            std::swap(m_value,            temp.m_value);
            std::swap(m_splitImage,       temp.m_splitImage);
            std::swap(m_textureBack_L,    temp.m_textureBack_L);
            std::swap(m_textureBack_M,    temp.m_textureBack_M);
            std::swap(m_textureBack_R,    temp.m_textureBack_R);
            std::swap(m_textureFront_L,   temp.m_textureFront_L);
            std::swap(m_textureFront_M,   temp.m_textureFront_M);
            std::swap(m_textureFront_R,   temp.m_textureFront_R);
            std::swap(m_text,             temp.m_text);
            std::swap(m_textSize,         temp.m_textSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar* LoadingBar::clone()
    {
        return new LoadingBar(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool LoadingBar::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_loaded = false;

        // Remove all textures if they were loaded before
        if (m_textureBack_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureBack_L);
        if (m_textureBack_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureBack_M);
        if (m_textureBack_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureBack_R);
        if (m_textureFront_L.data != nullptr) TGUI_TextureManager.removeTexture(m_textureFront_L);
        if (m_textureFront_M.data != nullptr) TGUI_TextureManager.removeTexture(m_textureFront_M);
        if (m_textureFront_R.data != nullptr) TGUI_TextureManager.removeTexture(m_textureFront_R);

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
        if (!configFile.read("LoadingBar", properties, values))
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

            if (property == "backimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureBack_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for BackImage in section LoadingBar in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = false;
            }
            else if (property == "frontimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFront_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FrontImage in section LoadingBar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "backimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureBack_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for BackImage_L in section LoadingBar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "backimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureBack_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for BackImage_M in section LoadingBar in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = true;
            }
            else if (property == "backimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureBack_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for BackImage_R in section LoadingBar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "frontimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFront_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FrontImage_L in section LoadingBar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "frontimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFront_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FrontImage_M in section LoadingBar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "frontimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFront_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FrontImage_R in section LoadingBar in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "textcolor")
            {
                setTextColor(extractColor(value));
            }
            else if (property == "textsize")
            {
                setTextSize(std::stoi(value));
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section LoadingBar in " + m_loadedConfigFile + ".");
        }

        // Check if the image is split
        if (m_splitImage)
        {
            // Make sure the required textures were loaded
            if ((m_textureBack_L.data != nullptr) && (m_textureBack_M.data != nullptr) && (m_textureBack_R.data != nullptr)
             && (m_textureFront_L.data != nullptr) && (m_textureFront_M.data != nullptr) && (m_textureFront_R.data != nullptr))
            {
                m_size.x = static_cast<float>(m_textureBack_L.getSize().x + m_textureBack_M.getSize().x + m_textureBack_R.getSize().x);
                m_size.y = static_cast<float>(m_textureBack_M.getSize().y);

                m_textureBack_M.data->texture.setRepeated(true);
                m_textureFront_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the loading bar. Is the LoadingBar section in " + m_loadedConfigFile + " complete?");
                return false;
            }
        }
        else // The image isn't split
        {
            // Make sure the required textures were loaded
            if ((m_textureBack_M.data != nullptr) && (m_textureFront_M.data != nullptr))
            {
                m_size = sf::Vector2f(m_textureBack_M.getSize());
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the loading bar. Is the LoadingBar section in " + m_loadedConfigFile + " complete?");
                return false;
            }
        }

        m_loaded = true;

        // Calculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();

        // Loading has succeeded
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& LoadingBar::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setSize(float width, float height)
    {
        // Don't do anything when the loading bar wasn't loaded correctly
        if (m_loaded == false)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the loading bar
        m_size.x = width;
        m_size.y = height;

        if (m_splitImage)
        {
            float minimumWidth = (m_textureBack_L.getSize().x + m_textureBack_R.getSize().x) * (m_size.y / m_textureBack_M.getSize().y);
            if (m_size.x < minimumWidth)
                m_size.x = minimumWidth;
        }

        // Recalculate the size of the front image
        recalculateSize();

        // Recalculate the text size
        setText(m_text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            m_value = m_minimum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            m_minimum = m_maximum;

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            m_value = m_maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setValue(unsigned int value)
    {
        // Set the new value
        m_value = value;

        // When the value is below the minimum or above the maximum then adjust it
        if (m_value < m_minimum)
            m_value = m_minimum;
        else if (m_value > m_maximum)
            m_value = m_maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::incrementValue()
    {
        // When the value is still below the maximum then adjust it
        if (m_value < m_maximum)
        {
            ++m_value;

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[ValueChanged].empty() == false)
            {
                m_callback.trigger = ValueChanged;
                m_callback.value   = static_cast<int>(m_value);
                addCallback();
            }

            // Check if the loading bar is now full
            if (m_value == m_maximum)
            {
                // Add the callback (if the user requested it)
                if (m_callbackFunctions[LoadingBarFull].empty() == false)
                {
                    m_callback.trigger = LoadingBarFull;
                    m_callback.value   = static_cast<int>(m_value);
                    addCallback();
                }
            }

            // Recalculate the size of the front image (the size of the part that will be drawn)
            recalculateSize();
        }

        // return the new value
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setText(const sf::String& text)
    {
        // Don't do anything when the loading bar wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Set the new text
        m_text.setString(text);

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            // Calculate a possible text size
            float size = m_size.y * 0.85f;
            m_text.setCharacterSize(static_cast<unsigned int>(size));
            m_text.setCharacterSize(static_cast<unsigned int>(m_text.getCharacterSize() - m_text.getLocalBounds().top));

            // Make sure that the text isn't too width
            if (m_text.getGlobalBounds().width > (m_size.x * 0.8f))
            {
                // The text is too width, so make it smaller
                m_text.setCharacterSize(static_cast<unsigned int>(size / (m_text.getGlobalBounds().width / (m_size.x * 0.8f))));
                m_text.setCharacterSize(static_cast<unsigned int>(m_text.getCharacterSize() - m_text.getLocalBounds().top));
            }
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_text.setCharacterSize(m_textSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String LoadingBar::getText() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTextFont(const sf::Font& font)
    {
        m_text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* LoadingBar::getTextFont() const
    {
        return m_text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTextColor(const sf::Color& color)
    {
        m_text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& LoadingBar::getTextColor() const
    {
        return m_text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::getTextSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        m_textureBack_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureBack_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureBack_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureFront_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureFront_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureFront_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool LoadingBar::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "minimum")
        {
            setMinimum(std::stoi(value));
        }
        else if (property == "maximum")
        {
            setMaximum(std::stoi(value));
        }
        else if (property == "value")
        {
            setValue(std::stoi(value));
        }
        else if (property == "text")
        {
            setText(value);
        }
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "textsize")
        {
            setTextSize(std::stoi(value));
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
                else if ((*it == "LoadingBarFull") || (*it == "loadingbarfull"))
                    bindCallback(LoadingBarFull);
            }
        }
        else // The property didn't match
            return ClickableWidget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool LoadingBar::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "minimum")
            value = std::to_string(getMinimum());
        else if (property == "maximum")
            value = std::to_string(getMaximum());
        else if (property == "value")
            value = std::to_string(getValue());
        else if (property == "text")
            value = getText().toAnsiString();
        else if (property == "textcolor")
            value = "(" + std::to_string(int(getTextColor().r)) + "," + std::to_string(int(getTextColor().g)) + "," + std::to_string(int(getTextColor().b)) + "," + std::to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = std::to_string(getTextSize());
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(ValueChanged) != m_callbackFunctions.end()) && (m_callbackFunctions.at(ValueChanged).size() == 1) && (m_callbackFunctions.at(ValueChanged).front() == nullptr))
                callbacks.push_back("ValueChanged");
            if ((m_callbackFunctions.find(LoadingBarFull) != m_callbackFunctions.end()) && (m_callbackFunctions.at(LoadingBarFull).size() == 1) && (m_callbackFunctions.at(LoadingBarFull).front() == nullptr))
                callbacks.push_back("LoadingBarFull");

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

    std::list< std::pair<std::string, std::string> > LoadingBar::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Minimum", "uint"));
        list.push_back(std::pair<std::string, std::string>("Maximum", "uint"));
        list.push_back(std::pair<std::string, std::string>("Value", "uint"));
        list.push_back(std::pair<std::string, std::string>("Text", "string"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::recalculateSize()
    {
        // Don't calculate anything when the loading bar wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Check if the image is split
        if (m_splitImage)
        {
            float totalWidth = m_size.x / (m_size.y / m_textureBack_M.getSize().y);

            // Get the bounds of the sprites
            sf::IntRect bounds_L = m_textureFront_L.sprite.getTextureRect();
            sf::IntRect bounds_M = m_textureFront_M.sprite.getTextureRect();
            sf::IntRect bounds_R = m_textureFront_R.sprite.getTextureRect();

            // Calculate the size of the part to display
            float frontSize;
            if ((m_maximum - m_minimum) > 0)
                frontSize = totalWidth * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum));
            else
                frontSize = totalWidth;

            // Check if a part of the left piece should be visible
            if (frontSize > 0)
            {
                // Check if a piece of the middle part should be drawn
                if (frontSize > m_textureBack_L.getSize().x)
                {
                    // Check if a piece of the right part should be drawn
                    if (frontSize > totalWidth - m_textureBack_R.getSize().x)
                    {
                        bounds_L.width = m_textureBack_L.getSize().x;
                        bounds_M.width = static_cast<int>(totalWidth - m_textureBack_L.getSize().x - m_textureBack_R.getSize().x);

                        // Check if the bar is not full
                        if (frontSize < totalWidth)
                            bounds_R.width = static_cast<int>(frontSize - (totalWidth - m_textureBack_R.getSize().x));
                        else
                            bounds_R.width = m_textureBack_R.getSize().x;
                    }
                    else // Only a part of the middle image should be drawn
                    {
                        bounds_L.width = m_textureBack_L.getSize().x;
                        bounds_M.width = static_cast<int>(frontSize - (m_textureBack_L.getSize().x));
                        bounds_R.width = 0;
                    }
                }
                else // Only a part of the left piece should be drawn
                {
                    bounds_L.width = static_cast<int>(frontSize);
                    bounds_M.width = 0;
                    bounds_R.width = 0;
                }
            }
            else // Nothing should be drawn
            {
                bounds_L.width = 0;
                bounds_M.width = 0;
                bounds_R.width = 0;
            }

            m_textureFront_L.sprite.setTextureRect(bounds_L);
            m_textureFront_M.sprite.setTextureRect(bounds_M);
            m_textureFront_R.sprite.setTextureRect(bounds_R);

            // Make sure that the back image is displayed correctly
            m_textureBack_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(totalWidth - m_textureBack_L.getSize().x - m_textureBack_R.getSize().x), m_textureBack_M.getSize().y));
        }
        else // The image is not split
        {
            // Calculate the size of the front sprite
            sf::IntRect frontBounds(m_textureFront_M.sprite.getTextureRect());

            // Only change the width when not dividing by zero
            if ((m_maximum - m_minimum) > 0)
                frontBounds.width = static_cast<int>(m_textureBack_M.getSize().x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)));
            else
                frontBounds.width = static_cast<int>(m_textureBack_M.getSize().x);

            // Set the size of the front image
            m_textureFront_M.sprite.setTextureRect(frontBounds);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::initialize(Container *const parent)
    {
        m_parent = parent;
        m_text.setFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the loading bar wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Apply the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Check if the image is split
        if (m_splitImage)
        {
            float scalingY = m_size.y / m_textureBack_M.getSize().y;

            // Scale the image
            states.transform.scale(scalingY, scalingY);

            // Draw the left image of the loading bar
            target.draw(m_textureBack_L, states);
            target.draw(m_textureFront_L, states);

            // Check if the middle image may be drawn
            if ((scalingY * (m_textureBack_L.getSize().x + m_textureBack_R.getSize().x)) < m_size.x)
            {
                // Put the middle image on the correct position
                states.transform.translate(static_cast<float>(m_textureBack_L.getSize().x), 0);

                // Draw the middle image
                target.draw(m_textureBack_M, states);
                target.draw(m_textureFront_M, states);

                // Put the right image on the correct position
                states.transform.translate(m_textureBack_M.sprite.getGlobalBounds().width, 0);

                // Draw the right image
                target.draw(m_textureBack_R, states);
                target.draw(m_textureFront_R, states);
            }
            else // The loading bar isn't width enough, we will draw it at minimum size
            {
                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_textureBack_L.getSize().x), 0);

                // Draw the right image
                target.draw(m_textureBack_R, states);
                target.draw(m_textureFront_R, states);
            }
        }
        else // The image is not split
        {
            // Scale the image
            states.transform.scale(m_size.x / m_textureBack_M.getSize().x, m_size.y / m_textureBack_M.getSize().y);

            // Draw the loading bar
            target.draw(m_textureBack_M, states);
            target.draw(m_textureFront_M, states);
        }

        // Check if there is a text to draw
        if (m_text.getString().isEmpty() == false)
        {
            // Reset the transformations
            states.transform = oldTransform;

            // Get the current size of the text, so that we can recalculate the position
            sf::FloatRect rect = m_text.getGlobalBounds();

            // Calculate the new position for the text
            rect.left = (m_size.x - rect.width) * 0.5f - rect.left;
            rect.top = (m_size.y - rect.height) * 0.5f - rect.top;

            // Set the new position
            states.transform.translate(std::floor(rect.left + 0.5f), std::floor(rect.top + 0.5f));

            // Draw the text
            target.draw(m_text, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
