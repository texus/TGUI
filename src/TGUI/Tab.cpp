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

#include <SFML/OpenGL.hpp>

#include <TGUI/Container.hpp>
#include <TGUI/Tab.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Tab() :
    m_SplitImage           (false),
    m_SeparateSelectedImage(true),
    m_TabHeight            (0),
    m_TextSize             (0),
    m_MaximumTabWidth      (0),
    m_DistanceToSide       (5),
    m_SelectedTab          (0)
    {
        m_Callback.widgetType = Type_Tab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Tab(const Tab& copy) :
    Widget                 (copy),
    m_LoadedConfigFile     (copy.m_LoadedConfigFile),
    m_SplitImage           (copy.m_SplitImage),
    m_SeparateSelectedImage(copy.m_SeparateSelectedImage),
    m_TabHeight            (copy.m_TabHeight),
    m_TextSize             (copy.m_TextSize),
    m_TextColor            (copy.m_TextColor),
    m_SelectedTextColor    (copy.m_SelectedTextColor),
    m_MaximumTabWidth      (copy.m_MaximumTabWidth),
    m_DistanceToSide       (copy.m_DistanceToSide),
    m_SelectedTab          (copy.m_SelectedTab),
    m_TabNames             (copy.m_TabNames),
    m_NameWidth            (copy.m_NameWidth),
    m_Text                 (copy.m_Text)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_L, m_TextureNormal_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_M, m_TextureNormal_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_R, m_TextureNormal_R);

        TGUI_TextureManager.copyTexture(copy.m_TextureSelected_L, m_TextureSelected_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureSelected_M, m_TextureSelected_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureSelected_R, m_TextureSelected_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::~Tab()
    {
        if (m_TextureNormal_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureSelected_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureSelected_L);
        if (m_TextureSelected_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureSelected_M);
        if (m_TextureSelected_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureSelected_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab& Tab::operator= (const Tab& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Tab temp(right);
            this->Widget::operator=(right);

            std::swap(m_LoadedConfigFile,      temp.m_LoadedConfigFile);
            std::swap(m_SplitImage,            temp.m_SplitImage);
            std::swap(m_SeparateSelectedImage, temp.m_SeparateSelectedImage);
            std::swap(m_TabHeight,             temp.m_TabHeight);
            std::swap(m_TextSize,              temp.m_TextSize);
            std::swap(m_TextColor,             temp.m_TextColor);
            std::swap(m_SelectedTextColor,     temp.m_SelectedTextColor);
            std::swap(m_MaximumTabWidth,       temp.m_MaximumTabWidth);
            std::swap(m_DistanceToSide,        temp.m_DistanceToSide);
            std::swap(m_SelectedTab,           temp.m_SelectedTab);
            std::swap(m_TabNames,              temp.m_TabNames);
            std::swap(m_NameWidth,             temp.m_NameWidth);
            std::swap(m_TextureNormal_L,       temp.m_TextureNormal_L);
            std::swap(m_TextureNormal_M,       temp.m_TextureNormal_M);
            std::swap(m_TextureNormal_R,       temp.m_TextureNormal_R);
            std::swap(m_TextureSelected_L,     temp.m_TextureSelected_L);
            std::swap(m_TextureSelected_M,     temp.m_TextureSelected_M);
            std::swap(m_TextureSelected_R,     temp.m_TextureSelected_R);
            std::swap(m_Text,                  temp.m_Text);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab* Tab::clone()
    {
        return new Tab(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::load(const std::string& configFileFilename, const std::string& sectionName)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If the button was loaded before then remove the old textures first
        if (m_TextureNormal_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureSelected_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureSelected_L);
        if (m_TextureSelected_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureSelected_M);
        if (m_TextureSelected_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureSelected_R);

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
        if (!configFile.read(sectionName, properties, values))
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

            if (property == "separateselectedimage")
            {
                m_SeparateSelectedImage = configFile.readBool(value, false);
            }
            else if (property == "textcolor")
            {
                m_TextColor = configFile.readColor(value);
            }
            else if (property == "selectedtextcolor")
            {
                m_SelectedTextColor = configFile.readColor(value);
            }
            else if (property == "distancetoside")
            {
                setDistanceToSide(static_cast<unsigned int>(atoi(value.c_str())));
            }
            else if (property == "normalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section Tab in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "selectedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureSelected_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for SelectedImage in section Tab in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_L in section Tab in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_M in section Tab in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "normalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_R in section Tab in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "selectedimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureSelected_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for SelectedImage_L in section Tab in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "selectedimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureSelected_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for SelectedImage_M in section Tab in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "selectedimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureSelected_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for SelectedImage_R in section Tab in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Tab in " + m_LoadedConfigFile + ".");
        }

        // Clear the vectors
        m_TabNames.clear();
        m_NameWidth.clear();

        // Check if the image is split
        if (m_SplitImage)
        {
            // Make sure the required textures were loaded
            if ((m_TextureNormal_L.data != nullptr) && (m_TextureNormal_M.data != nullptr) && (m_TextureNormal_R.data != nullptr))
            {
                m_TabHeight = m_TextureNormal_M.getSize().y;

                m_TextureNormal_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the tab. Is the Tab section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureSelected_L.data != nullptr) && (m_TextureSelected_M.data != nullptr) && (m_TextureSelected_R.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Selected;

                m_TextureSelected_M.data->texture.setRepeated(true);
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_TextureNormal_M.data != nullptr)
            {
                m_TabHeight = m_TextureNormal_M.getSize().y;
            }
            else
            {
                TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the Tab section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if (m_TextureSelected_M.data != nullptr)
            {
                m_WidgetPhase |= WidgetPhase_Selected;
            }
        }

        // Recalculate the text size when auto sizing
        if (m_TextSize == 0)
            setTextSize(0);

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Tab::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setSize(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Tab::getSize() const
    {
        // Make sure the tab has been loaded
        if (m_Loaded == false)
            return sf::Vector2f(0, 0);

        // Add the width of all the tabs together
        float width = 0;
        for (unsigned int i=0; i<m_NameWidth.size(); ++i)
        {
            if (m_SplitImage)
                width += TGUI_MAXIMUM(m_MaximumTabWidth ? TGUI_MINIMUM(m_NameWidth[i] + (2 * m_DistanceToSide), m_MaximumTabWidth) : m_NameWidth[i] + (2 * m_DistanceToSide),
                                      (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * (m_TabHeight / static_cast<float>(m_TextureNormal_M.getSize().y)));
            else
                width += m_MaximumTabWidth ? TGUI_MINIMUM(m_NameWidth[i] + (2 * m_DistanceToSide), m_MaximumTabWidth) : m_NameWidth[i] + (2 * m_DistanceToSide);
        }

        return sf::Vector2f(width, static_cast<float>(m_TabHeight));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::add(const sf::String& name, bool selectTab)
    {
        // Add the tab
        m_TabNames.push_back(name);

        // Calculate the width of the tab
        m_Text.setString(name);
        m_NameWidth.push_back(m_Text.getLocalBounds().width);

        // If the tab has to be selected then do so
        if (selectTab)
            m_SelectedTab = m_TabNames.size()-1;

        // Return the index of the new tab
        return m_TabNames.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(const sf::String& name)
    {
        // Loop through all tabs
        for (unsigned int i=0; i<m_TabNames.size(); ++i)
        {
            // Find the tab that should be selected
            if (m_TabNames[i] == name)
            {
                // Select the tab
                m_SelectedTab = i;
                return;
            }
        }

        TGUI_OUTPUT("TGUI warning: Failed to select the tab. The name didn't match any tab.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(unsigned int index)
    {
        // If the index is too big then do nothing
        if (index > m_TabNames.size()-1)
        {
            TGUI_OUTPUT("TGUI warning: Failed to select the tab. The index was too high.");
            return;
        }

        // Select the tab
        m_SelectedTab = index;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::deselect()
    {
        m_SelectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(const sf::String& name)
    {
        // Loop through all tabs
        for (unsigned int i=0; i<m_TabNames.size(); ++i)
        {
            // Check if you found the tab to remove
            if (m_TabNames[i] == name)
            {
                // Remove the tab
                m_TabNames.erase(m_TabNames.begin() + i);
                m_NameWidth.erase(m_NameWidth.begin() + i);

                // Check if the selected tab should change
                if (m_SelectedTab == static_cast<int>(i))
                    m_SelectedTab = -1;
                else if (m_SelectedTab > static_cast<int>(i))
                    --m_SelectedTab;

                return;
            }
        }

        TGUI_OUTPUT("TGUI warning: Failed to remove the tab. The name didn't match any tab.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(unsigned int index)
    {
        // The index can't be too high
        if (index > m_TabNames.size()-1)
        {
            TGUI_OUTPUT("TGUI warning: Failed to remove the tab. The index was too high.");
            return;
        }

        // Remove the tab
        m_TabNames.erase(m_TabNames.begin() + index);
        m_NameWidth.erase(m_NameWidth.begin() + index);

        // Check if the selected tab should change
        if (m_SelectedTab == static_cast<int>(index))
            m_SelectedTab = -1;
        else if (m_SelectedTab > static_cast<int>(index))
            --m_SelectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::removeAll()
    {
        m_TabNames.clear();
        m_NameWidth.clear();
        m_SelectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tab::getSelected() const
    {
        if (m_SelectedTab == -1)
            return "";
        else
            return m_TabNames[m_SelectedTab];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Tab::getSelectedIndex() const
    {
        return m_SelectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);

        // Recalculate the name widths
        for (unsigned int i = 0; i < m_NameWidth.size(); ++i)
        {
            m_Text.setString(m_TabNames[i]);
            m_NameWidth[i] = m_Text.getLocalBounds().width;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Tab::getTextFont() const
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextColor(const sf::Color& color)
    {
        m_TextColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Tab::getTextColor() const
    {
        return m_TextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setSelectedTextColor(const sf::Color& color)
    {
        m_SelectedTextColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Tab::getSelectedTextColor() const
    {
        return m_SelectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextSize(unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Check if the text is auto sized
        if (m_TextSize == 0)
        {
            // Calculate the text size
            m_Text.setString("kg");
            m_Text.setCharacterSize(static_cast<unsigned int>(m_TabHeight * 0.75f));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_Text.setCharacterSize(m_TextSize);
        }

        // Recalculate the name widths
        for (unsigned int i=0; i<m_NameWidth.size(); ++i)
        {
            m_Text.setString(m_TabNames[i]);
            m_NameWidth[i] = m_Text.getLocalBounds().width;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTextSize() const
    {
        return m_Text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTabHeight(unsigned int height)
    {
        // Make sure that the height changed
        if (m_TabHeight != height)
        {
            m_TabHeight = height;

            // Recalculate the size when auto sizing
            if (m_TextSize == 0)
                setTextSize(0);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTabHeight() const
    {
        return m_TabHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setMaximumTabWidth(unsigned int maximumWidth)
    {
        m_MaximumTabWidth = maximumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getMaximumTabWidth() const
    {
        return m_MaximumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setDistanceToSide(unsigned int distanceToSide)
    {
        m_DistanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getDistanceToSide() const
    {
        return m_DistanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_TextureNormal_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureNormal_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureNormal_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureSelected_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureSelected_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureSelected_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the tab
        if (m_Loaded)
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
                return true;
        }

        if (m_MouseHover)
            mouseLeftWidget();

        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::leftMousePressed(float x, float y)
    {
        float width = getPosition().x;

        // Loop through all tabs
        for (unsigned int i=0; i<m_NameWidth.size(); ++i)
        {
            // Append the width of the tab
            if (m_SplitImage)
                width += TGUI_MAXIMUM(m_MaximumTabWidth ? TGUI_MINIMUM(m_NameWidth[i] + (2 * m_DistanceToSide), m_MaximumTabWidth) : m_NameWidth[i] + (2 * m_DistanceToSide),
                                      (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * (m_TabHeight / static_cast<float>(m_TextureNormal_M.getSize().y)));
            else
                width += m_MaximumTabWidth ? TGUI_MINIMUM(m_NameWidth[i] + (2 * m_DistanceToSide), m_MaximumTabWidth) : m_NameWidth[i] + (2 * m_DistanceToSide);

            // Check if the mouse went down on the tab
            if (x < width)
            {
                // Select this tab
                m_SelectedTab = i;

                // Add the callback (if the user requested it)
                if (m_CallbackFunctions[TabChanged].empty() == false)
                {
                    m_Callback.trigger = TabChanged;
                    m_Callback.value   = m_SelectedTab;
                    m_Callback.text    = m_TabNames[i];
                    m_Callback.mouse.x = static_cast<int>(x - getPosition().x);
                    m_Callback.mouse.y = static_cast<int>(y - getPosition().y);
                    addCallback();
                }

                // The tab was found
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "textsize")
        {
            setTextSize(atoi(value.c_str()));
        }
        else if (property == "tabheight")
        {
            setTabHeight(atoi(value.c_str()));
        }
        else if (property == "maximumtabwidth")
        {
            setMaximumTabWidth(atoi(value.c_str()));
        }
        else if (property == "distancetoside")
        {
            setDistanceToSide(atoi(value.c_str()));
        }
        else if (property == "tabs")
        {
            removeAll();

            std::vector<sf::String> tabs;
            decodeList(value, tabs);

            for (auto it = tabs.cbegin(); it != tabs.cend(); ++it)
                add(*it);
        }
        else if (property == "selectedtab")
        {
            select(atoi(value.c_str()));
        }
        else if (property == "callback")
        {
            Widget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "TabChanged") || (*it == "tabchanged"))
                    bindCallback(TabChanged);
            }
        }
        else // The property didn't match
            return Widget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "textcolor")
            value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = to_string(getTextSize());
        else if (property == "tabheight")
            value = to_string(getTabHeight());
        else if (property == "maximumtabwidth")
            value = to_string(getMaximumTabWidth());
        else if (property == "distancetoside")
            value = to_string(getDistanceToSide());
        else if (property == "tabs")
            encodeList(m_TabNames, value);
        else if (property == "selectedtab")
            value = to_string(getSelectedIndex());
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(TabChanged) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(TabChanged).size() == 1) && (m_CallbackFunctions.at(TabChanged).front() == nullptr))
                callbacks.push_back("TabChanged");

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

    std::list< std::pair<std::string, std::string> > Tab::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("TabHeight", "uint"));
        list.push_back(std::pair<std::string, std::string>("MaximumTabWidth", "uint"));
        list.push_back(std::pair<std::string, std::string>("DistanceToSide", "uint"));
        list.push_back(std::pair<std::string, std::string>("Tabs", "string"));
        list.push_back(std::pair<std::string, std::string>("SelectedTab", "int"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::initialize(Container *const parent)
    {
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the child window wasn't created
        if (m_Loaded == false)
            return;

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the scale factor of the view
        const sf::View& view = target.getView();
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Apply the transformations
        states.transform *= getTransform();

        float scalingY = m_TabHeight / static_cast<float>(m_TextureNormal_M.getSize().y);
        bool clippingRequired = false;
        unsigned int accumulatedTabWidth = 0;
        unsigned int tabWidth;
        sf::FloatRect realRect;
        sf::FloatRect defaultRect;
        sf::Text tempText(m_Text);

        // Calculate the height and top of all strings
        tempText.setString("kg");
        defaultRect = tempText.getLocalBounds();

        // Loop through all tabs
        for (unsigned int i = 0; i < m_TabNames.size(); ++i)
        {
            // Find the tab height
            if (m_MaximumTabWidth)
            {
                if (m_MaximumTabWidth < m_NameWidth[i] + (2 * m_DistanceToSide))
                {
                    tabWidth = m_MaximumTabWidth;
                    clippingRequired = true;
                }
                else
                    tabWidth = static_cast<unsigned int>(m_NameWidth[i] + (2 * m_DistanceToSide));
            }
            else
                tabWidth = static_cast<unsigned int>(m_NameWidth[i] + (2 * m_DistanceToSide));

            // There is a minimum tab width
            if (tabWidth < 2 * m_DistanceToSide)
                tabWidth = 2 * m_DistanceToSide;

            // Check if the image is split
            if (m_SplitImage)
            {
                // There is another minimum when using SplitImage
                float minimumWidth = (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * scalingY;
                if (tabWidth < minimumWidth)
                    tabWidth = static_cast<unsigned int>(minimumWidth);

                // Set the scaling
                states.transform.scale(scalingY, scalingY);

                // Draw the left tab image
                if (m_SeparateSelectedImage)
                {
                    if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                        target.draw(m_TextureSelected_L, states);
                    else
                        target.draw(m_TextureNormal_L, states);
                }
                else // There is no separate selected image
                {
                    target.draw(m_TextureNormal_L, states);

                    if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                        target.draw(m_TextureSelected_L, states);
                }

                // Check if the middle image may be drawn
                if ((scalingY * (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x)) < tabWidth)
                {
                    // Calculate the scale for our middle image
                    float scaleX = (tabWidth / static_cast<float>(m_TextureNormal_M.getSize().x)) - (((m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * scalingY) / m_TextureNormal_M.getSize().x);

                    // Put the middle image on the correct position
                    states.transform.translate(static_cast<float>(m_TextureNormal_L.getSize().x), 0);

                    // Set the scale for the middle image
                    states.transform.scale(scaleX / scalingY, 1);

                    // Draw the middle tab image
                    if (m_SeparateSelectedImage)
                    {
                        if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                            target.draw(m_TextureSelected_M, states);
                        else
                            target.draw(m_TextureNormal_M, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_TextureNormal_M, states);

                        if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                            target.draw(m_TextureSelected_M, states);
                    }

                    // Put the right image on the correct position
                    states.transform.translate(static_cast<float>(m_TextureNormal_M.getSize().x), 0);

                    // Set the scale for the right image
                    states.transform.scale(scalingY / scaleX, 1);

                    // Draw the right tab image
                    if (m_SeparateSelectedImage)
                    {
                        if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                            target.draw(m_TextureSelected_R, states);
                        else
                            target.draw(m_TextureNormal_R, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_TextureNormal_R, states);

                        if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                            target.draw(m_TextureSelected_R, states);
                    }

                    // Undo the translation
                    states.transform.translate(-(m_TextureNormal_L.getSize().x + (m_TextureNormal_M.getSize().x * scaleX / scalingY)), 0);
                }
                else // The edit box isn't width enough, we will draw it at minimum size
                {
                    // Put the right image on the correct position
                    states.transform.translate(static_cast<float>(m_TextureNormal_L.getSize().x), 0);

                    // Draw the right tab image
                    if (m_SeparateSelectedImage)
                    {
                        if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                            target.draw(m_TextureSelected_R, states);
                        else
                            target.draw(m_TextureNormal_R, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_TextureNormal_R, states);

                        if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                            target.draw(m_TextureSelected_R, states);
                    }

                    // Undo the translation
                    states.transform.translate(-static_cast<float>(m_TextureNormal_L.getSize().x), 0);
                }

                // Undo the scaling of the split images
                states.transform.scale(1.f/scalingY, 1.f/scalingY);
            }
            else // The image isn't split
            {
                // Set the scaling
                states.transform.scale(tabWidth / static_cast<float>(m_TextureNormal_M.getSize().x), scalingY);

                // Draw the tab image
                if (m_SeparateSelectedImage)
                {
                    if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                        target.draw(m_TextureSelected_M, states);
                    else
                        target.draw(m_TextureNormal_M, states);
                }
                else // There is no separate selected image
                {
                    target.draw(m_TextureNormal_M, states);

                    if ((m_SelectedTab == static_cast<int>(i)) && (m_WidgetPhase & WidgetPhase_Selected))
                        target.draw(m_TextureSelected_M, states);
                }

                // Undo the scaling
                states.transform.scale(static_cast<float>(m_TextureNormal_M.getSize().x) / tabWidth, 1.f / scalingY);
            }

            // Draw the text
            {
                // Give the text the correct color
                if (m_SelectedTab == static_cast<int>(i))
                    tempText.setColor(m_SelectedTextColor);
                else
                    tempText.setColor(m_TextColor);

                // Get the current size of the text, so that we can recalculate the position
                tempText.setString(m_TabNames[i]);
                realRect = tempText.getLocalBounds();

                // Calculate the new position for the text
                if ((m_SplitImage) && (tabWidth == (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * scalingY))
                    realRect.left = ((tabWidth - realRect.width) / 2.f) - realRect.left;
                else
                    realRect.left = m_DistanceToSide - realRect.left;
                realRect.top = ((m_TabHeight - defaultRect.height) / 2.f) - defaultRect.top;

                // Move the text to the correct position
                states.transform.translate(std::floor(realRect.left + 0.5f), std::floor(realRect.top + 0.5f));

                // Check if clipping is required for this text
                if (clippingRequired)
                {
                    // Get the global position
                    sf::Vector2f topLeftPosition = sf::Vector2f(((getAbsolutePosition().x + accumulatedTabWidth + m_DistanceToSide + (view.getSize().x / 2.f) - view.getCenter().x) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                                ((getAbsolutePosition().y + (view.getSize().y / 2.f) - view.getCenter().y) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
                    sf::Vector2f bottomRightPosition = sf::Vector2f(((getAbsolutePosition().x + accumulatedTabWidth + tabWidth - m_DistanceToSide - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                                    ((getAbsolutePosition().y + ((m_TabHeight + defaultRect.height) / 2.f) - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));

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
                }

                // Draw the text
                target.draw(tempText, states);

                // Undo the translation of the text
                states.transform.translate(-std::floor(realRect.left + 0.5f), -std::floor(realRect.top + 0.5f));

                // Reset the old clipping area when needed
                if (clippingRequired)
                {
                    clippingRequired = false;
                    glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
                }
            }

            // Set the next tab on the correct position
            states.transform.translate(static_cast<float>(tabWidth), 0);
            accumulatedTabWidth += tabWidth;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
