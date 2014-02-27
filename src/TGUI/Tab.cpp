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

#include <SFML/OpenGL.hpp>

#include <TGUI/Container.hpp>
#include <TGUI/Tab.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Tab() :
    m_splitImage           (false),
    m_separateSelectedImage(true),
    m_tabHeight            (0),
    m_textSize             (0),
    m_maximumTabWidth      (0),
    m_distanceToSide       (5),
    m_selectedTab          (0)
    {
        m_callback.widgetType = Type_Tab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Tab(const Tab& copy) :
    Widget                 (copy),
    m_loadedConfigFile     (copy.m_loadedConfigFile),
    m_splitImage           (copy.m_splitImage),
    m_separateSelectedImage(copy.m_separateSelectedImage),
    m_tabHeight            (copy.m_tabHeight),
    m_textSize             (copy.m_textSize),
    m_textColor            (copy.m_textColor),
    m_selectedTextColor    (copy.m_selectedTextColor),
    m_maximumTabWidth      (copy.m_maximumTabWidth),
    m_distanceToSide       (copy.m_distanceToSide),
    m_selectedTab          (copy.m_selectedTab),
    m_tabNames             (copy.m_tabNames),
    m_nameWidth            (copy.m_nameWidth),
    m_text                 (copy.m_text)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureNormal_L, m_textureNormal_L);
        TGUI_TextureManager.copyTexture(copy.m_textureNormal_M, m_textureNormal_M);
        TGUI_TextureManager.copyTexture(copy.m_textureNormal_R, m_textureNormal_R);

        TGUI_TextureManager.copyTexture(copy.m_textureSelected_L, m_textureSelected_L);
        TGUI_TextureManager.copyTexture(copy.m_textureSelected_M, m_textureSelected_M);
        TGUI_TextureManager.copyTexture(copy.m_textureSelected_R, m_textureSelected_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::~Tab()
    {
        if (m_textureNormal_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureNormal_L);
        if (m_textureNormal_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureNormal_M);
        if (m_textureNormal_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureNormal_R);

        if (m_textureSelected_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureSelected_L);
        if (m_textureSelected_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureSelected_M);
        if (m_textureSelected_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureSelected_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab& Tab::operator= (const Tab& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Tab temp(right);
            this->Widget::operator=(right);

            std::swap(m_loadedConfigFile,      temp.m_loadedConfigFile);
            std::swap(m_splitImage,            temp.m_splitImage);
            std::swap(m_separateSelectedImage, temp.m_separateSelectedImage);
            std::swap(m_tabHeight,             temp.m_tabHeight);
            std::swap(m_textSize,              temp.m_textSize);
            std::swap(m_textColor,             temp.m_textColor);
            std::swap(m_selectedTextColor,     temp.m_selectedTextColor);
            std::swap(m_maximumTabWidth,       temp.m_maximumTabWidth);
            std::swap(m_distanceToSide,        temp.m_distanceToSide);
            std::swap(m_selectedTab,           temp.m_selectedTab);
            std::swap(m_tabNames,              temp.m_tabNames);
            std::swap(m_nameWidth,             temp.m_nameWidth);
            std::swap(m_textureNormal_L,       temp.m_textureNormal_L);
            std::swap(m_textureNormal_M,       temp.m_textureNormal_M);
            std::swap(m_textureNormal_R,       temp.m_textureNormal_R);
            std::swap(m_textureSelected_L,     temp.m_textureSelected_L);
            std::swap(m_textureSelected_M,     temp.m_textureSelected_M);
            std::swap(m_textureSelected_R,     temp.m_textureSelected_R);
            std::swap(m_text,                  temp.m_text);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab* Tab::clone()
    {
        return new Tab(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_loaded = false;

        // If the button was loaded before then remove the old textures first
        if (m_textureNormal_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureNormal_L);
        if (m_textureNormal_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureNormal_M);
        if (m_textureNormal_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_textureNormal_R);
        if (m_textureSelected_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureSelected_L);
        if (m_textureSelected_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureSelected_M);
        if (m_textureSelected_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_textureSelected_R);

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
        if (!configFile.read("Tab", properties, values))
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

            if (property == "separateselectedimage")
            {
                m_separateSelectedImage = configFile.readBool(value, false);
            }
            else if (property == "textcolor")
            {
                m_textColor = configFile.readColor(value);
            }
            else if (property == "selectedtextcolor")
            {
                m_selectedTextColor = configFile.readColor(value);
            }
            else if (property == "distancetoside")
            {
                setDistanceToSide(std::stoul(value));
            }
            else if (property == "normalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section Tab in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = false;
            }
            else if (property == "selectedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureSelected_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for SelectedImage in section Tab in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_L in section Tab in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_M in section Tab in " + m_loadedConfigFile + ".");
                    return false;
                }

                m_splitImage = true;
            }
            else if (property == "normalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_R in section Tab in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "selectedimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureSelected_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for SelectedImage_L in section Tab in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "selectedimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureSelected_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for SelectedImage_M in section Tab in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "selectedimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureSelected_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for SelectedImage_R in section Tab in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Tab in " + m_loadedConfigFile + ".");
        }

        // Clear the vectors
        m_tabNames.clear();
        m_nameWidth.clear();

        // Check if the image is split
        if (m_splitImage)
        {
            // Make sure the required textures were loaded
            if ((m_textureNormal_L.data != nullptr) && (m_textureNormal_M.data != nullptr) && (m_textureNormal_R.data != nullptr))
            {
                m_tabHeight = m_textureNormal_M.getSize().y;

                m_textureNormal_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the tab. Is the Tab section in " + m_loadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_textureSelected_L.data != nullptr) && (m_textureSelected_M.data != nullptr) && (m_textureSelected_R.data != nullptr))
            {
                m_widgetPhase |= WidgetPhase_Selected;

                m_textureSelected_M.data->texture.setRepeated(true);
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_textureNormal_M.data != nullptr)
            {
                m_tabHeight = m_textureNormal_M.getSize().y;
            }
            else
            {
                TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the Tab section in " + m_loadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if (m_textureSelected_M.data != nullptr)
            {
                m_widgetPhase |= WidgetPhase_Selected;
            }
        }

        // Recalculate the text size when auto sizing
        if (m_textSize == 0)
            setTextSize(0);

        return m_loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Tab::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setSize(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Tab::getSize() const
    {
        // Make sure the tab has been loaded
        if (m_loaded == false)
            return sf::Vector2f(0, 0);

        // Add the width of all the tabs together
        float width = 0;
        for (unsigned int i=0; i<m_nameWidth.size(); ++i)
        {
            if (m_splitImage)
                width += TGUI_MAXIMUM(m_maximumTabWidth ? TGUI_MINIMUM(m_nameWidth[i] + (2 * m_distanceToSide), m_maximumTabWidth) : m_nameWidth[i] + (2 * m_distanceToSide),
                                      (m_textureNormal_L.getSize().x + m_textureNormal_R.getSize().x) * (m_tabHeight / static_cast<float>(m_textureNormal_M.getSize().y)));
            else
                width += m_maximumTabWidth ? TGUI_MINIMUM(m_nameWidth[i] + (2 * m_distanceToSide), m_maximumTabWidth) : m_nameWidth[i] + (2 * m_distanceToSide);
        }

        return sf::Vector2f(width, static_cast<float>(m_tabHeight));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::add(const sf::String& name, bool selectTab)
    {
        // Add the tab
        m_tabNames.push_back(name);

        // Calculate the width of the tab
        m_text.setString(name);
        m_nameWidth.push_back(m_text.getLocalBounds().width);

        // If the tab has to be selected then do so
        if (selectTab)
            m_selectedTab = m_tabNames.size()-1;

        // Return the index of the new tab
        return m_tabNames.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(const sf::String& name)
    {
        // Loop through all tabs
        for (unsigned int i=0; i<m_tabNames.size(); ++i)
        {
            // Find the tab that should be selected
            if (m_tabNames[i] == name)
            {
                // Select the tab
                m_selectedTab = i;
                return;
            }
        }

        TGUI_OUTPUT("TGUI warning: Failed to select the tab. The name didn't match any tab.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(unsigned int index)
    {
        // If the index is too big then do nothing
        if (index > m_tabNames.size()-1)
        {
            TGUI_OUTPUT("TGUI warning: Failed to select the tab. The index was too high.");
            return;
        }

        // Select the tab
        m_selectedTab = index;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::deselect()
    {
        m_selectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(const sf::String& name)
    {
        // Loop through all tabs
        for (unsigned int i=0; i<m_tabNames.size(); ++i)
        {
            // Check if you found the tab to remove
            if (m_tabNames[i] == name)
            {
                // Remove the tab
                m_tabNames.erase(m_tabNames.begin() + i);
                m_nameWidth.erase(m_nameWidth.begin() + i);

                // Check if the selected tab should change
                if (m_selectedTab == static_cast<int>(i))
                    m_selectedTab = -1;
                else if (m_selectedTab > static_cast<int>(i))
                    --m_selectedTab;

                return;
            }
        }

        TGUI_OUTPUT("TGUI warning: Failed to remove the tab. The name didn't match any tab.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(unsigned int index)
    {
        // The index can't be too high
        if (index > m_tabNames.size()-1)
        {
            TGUI_OUTPUT("TGUI warning: Failed to remove the tab. The index was too high.");
            return;
        }

        // Remove the tab
        m_tabNames.erase(m_tabNames.begin() + index);
        m_nameWidth.erase(m_nameWidth.begin() + index);

        // Check if the selected tab should change
        if (m_selectedTab == static_cast<int>(index))
            m_selectedTab = -1;
        else if (m_selectedTab > static_cast<int>(index))
            --m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::removeAll()
    {
        m_tabNames.clear();
        m_nameWidth.clear();
        m_selectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tab::getSelected() const
    {
        if (m_selectedTab == -1)
            return "";
        else
            return m_tabNames[m_selectedTab];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Tab::getSelectedIndex() const
    {
        return m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextFont(const sf::Font& font)
    {
        m_text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Tab::getTextFont() const
    {
        return m_text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextColor(const sf::Color& color)
    {
        m_textColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Tab::getTextColor() const
    {
        return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setSelectedTextColor(const sf::Color& color)
    {
        m_selectedTextColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Tab::getSelectedTextColor() const
    {
        return m_selectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            // Calculate the text size
            m_text.setString("kg");
            m_text.setCharacterSize(static_cast<unsigned int>(m_tabHeight * 0.85f));
            m_text.setCharacterSize(static_cast<unsigned int>(m_text.getCharacterSize() - m_text.getLocalBounds().top));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_text.setCharacterSize(m_textSize);
        }

        // Recalculate the name widths
        for (unsigned int i=0; i<m_nameWidth.size(); ++i)
        {
            m_text.setString(m_tabNames[i]);
            m_nameWidth[i] = m_text.getLocalBounds().width;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTextSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTabHeight(unsigned int height)
    {
        // Make sure that the height changed
        if (m_tabHeight != height)
        {
            m_tabHeight = height;

            // Recalculate the size when auto sizing
            if (m_textSize == 0)
                setTextSize(0);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTabHeight() const
    {
        return m_tabHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setMaximumTabWidth(unsigned int maximumWidth)
    {
        m_maximumTabWidth = maximumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getMaximumTabWidth() const
    {
        return m_maximumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setDistanceToSide(unsigned int distanceToSide)
    {
        m_distanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getDistanceToSide() const
    {
        return m_distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_textureNormal_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureNormal_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureNormal_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureSelected_L.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureSelected_M.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureSelected_R.sprite.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the tab
        if (m_loaded)
        {
            if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
                return true;
        }

        if (m_mouseHover)
            mouseLeftWidget();

        m_mouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::leftMousePressed(float x, float y)
    {
        float width = getPosition().x;

        // Loop through all tabs
        for (unsigned int i=0; i<m_nameWidth.size(); ++i)
        {
            // Append the width of the tab
            if (m_splitImage)
                width += TGUI_MAXIMUM(m_maximumTabWidth ? TGUI_MINIMUM(m_nameWidth[i] + (2 * m_distanceToSide), m_maximumTabWidth) : m_nameWidth[i] + (2 * m_distanceToSide),
                                      (m_textureNormal_L.getSize().x + m_textureNormal_R.getSize().x) * (m_tabHeight / static_cast<float>(m_textureNormal_M.getSize().y)));
            else
                width += m_maximumTabWidth ? TGUI_MINIMUM(m_nameWidth[i] + (2 * m_distanceToSide), m_maximumTabWidth) : m_nameWidth[i] + (2 * m_distanceToSide);

            // Check if the mouse went down on the tab
            if (x < width)
            {
                // Select this tab
                m_selectedTab = i;

                // Add the callback (if the user requested it)
                if (m_callbackFunctions[TabChanged].empty() == false)
                {
                    m_callback.trigger = TabChanged;
                    m_callback.value   = m_selectedTab;
                    m_callback.text    = m_tabNames[i];
                    m_callback.mouse.x = static_cast<int>(x - getPosition().x);
                    m_callback.mouse.y = static_cast<int>(y - getPosition().y);
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
            setTextSize(std::stoi(value));
        }
        else if (property == "tabheight")
        {
            setTabHeight(std::stoi(value));
        }
        else if (property == "maximumtabwidth")
        {
            setMaximumTabWidth(std::stoi(value));
        }
        else if (property == "distancetoside")
        {
            setDistanceToSide(std::stoi(value));
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
            select(std::stoi(value));
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
            value = "(" + std::to_string(int(getTextColor().r)) + "," + std::to_string(int(getTextColor().g)) + "," + std::to_string(int(getTextColor().b)) + "," + std::to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = std::to_string(getTextSize());
        else if (property == "tabheight")
            value = std::to_string(getTabHeight());
        else if (property == "maximumtabwidth")
            value = std::to_string(getMaximumTabWidth());
        else if (property == "distancetoside")
            value = std::to_string(getDistanceToSide());
        else if (property == "tabs")
            encodeList(m_tabNames, value);
        else if (property == "selectedtab")
            value = std::to_string(getSelectedIndex());
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(TabChanged) != m_callbackFunctions.end()) && (m_callbackFunctions.at(TabChanged).size() == 1) && (m_callbackFunctions.at(TabChanged).front() == nullptr))
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
        m_parent = parent;
        m_text.setFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the child window wasn't created
        if (m_loaded == false)
            return;

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Apply the transformations
        states.transform *= getTransform();

        float scalingY = m_tabHeight / static_cast<float>(m_textureNormal_M.getSize().y);
        bool clippingRequired = false;
        unsigned int tabWidth;
        sf::FloatRect realRect;
        sf::FloatRect defaultRect;
        sf::Text tempText(m_text);

        // Calculate the height and top of all strings
        tempText.setString("kg");
        defaultRect = tempText.getLocalBounds();

        // Loop through all tabs
        for (unsigned int i=0; i<m_tabNames.size(); ++i)
        {
            // Find the tab height
            if (m_maximumTabWidth)
            {
                if (m_maximumTabWidth < m_nameWidth[i] + (2 * m_distanceToSide))
                {
                    tabWidth = m_maximumTabWidth;
                    clippingRequired = true;
                }
                else
                    tabWidth = static_cast<unsigned int>(m_nameWidth[i] + (2 * m_distanceToSide));
            }
            else
                tabWidth = static_cast<unsigned int>(m_nameWidth[i] + (2 * m_distanceToSide));

            // There is a minimum tab width
            if (tabWidth < 2 * m_distanceToSide)
                tabWidth = 2 * m_distanceToSide;

            // Check if the image is split
            if (m_splitImage)
            {
                // There is another minimum when using SplitImage
                float minimumWidth = (m_textureNormal_L.getSize().x + m_textureNormal_R.getSize().x) * scalingY;
                if (tabWidth < minimumWidth)
                    tabWidth = static_cast<unsigned int>(minimumWidth);

                // Set the scaling
                states.transform.scale(scalingY, scalingY);

                // Draw the left tab image
                if (m_separateSelectedImage)
                {
                    if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                        target.draw(m_textureSelected_L, states);
                    else
                        target.draw(m_textureNormal_L, states);
                }
                else // There is no separate selected image
                {
                    target.draw(m_textureNormal_L, states);

                    if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                        target.draw(m_textureSelected_L, states);
                }

                // Check if the middle image may be drawn
                if ((scalingY * (m_textureNormal_L.getSize().x + m_textureNormal_R.getSize().x)) < tabWidth)
                {
                    // Calculate the scale for our middle image
                    float scaleX = (tabWidth / static_cast<float>(m_textureNormal_M.getSize().x)) - (((m_textureNormal_L.getSize().x + m_textureNormal_R.getSize().x) * scalingY) / m_textureNormal_M.getSize().x);

                    // Put the middle image on the correct position
                    states.transform.translate(static_cast<float>(m_textureNormal_L.getSize().x), 0);

                    // Set the scale for the middle image
                    states.transform.scale(scaleX / scalingY, 1);

                    // Draw the middle tab image
                    if (m_separateSelectedImage)
                    {
                        if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                            target.draw(m_textureSelected_M, states);
                        else
                            target.draw(m_textureNormal_M, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_textureNormal_M, states);

                        if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                            target.draw(m_textureSelected_M, states);
                    }

                    // Put the right image on the correct position
                    states.transform.translate(static_cast<float>(m_textureNormal_M.getSize().x), 0);

                    // Set the scale for the right image
                    states.transform.scale(scalingY / scaleX, 1);

                    // Draw the right tab image
                    if (m_separateSelectedImage)
                    {
                        if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                            target.draw(m_textureSelected_R, states);
                        else
                            target.draw(m_textureNormal_R, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_textureNormal_R, states);

                        if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                            target.draw(m_textureSelected_R, states);
                    }

                    // Undo the translation
                    states.transform.translate(-(m_textureNormal_L.getSize().x + (m_textureNormal_M.getSize().x * scaleX / scalingY)), 0);
                }
                else // The edit box isn't width enough, we will draw it at minimum size
                {
                    // Put the right image on the correct position
                    states.transform.translate(static_cast<float>(m_textureNormal_L.getSize().x), 0);

                    // Draw the right tab image
                    if (m_separateSelectedImage)
                    {
                        if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                            target.draw(m_textureSelected_R, states);
                        else
                            target.draw(m_textureNormal_R, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_textureNormal_R, states);

                        if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                            target.draw(m_textureSelected_R, states);
                    }

                    // Undo the translation
                    states.transform.translate(-static_cast<float>(m_textureNormal_L.getSize().x), 0);
                }

                // Undo the scaling of the split images
                states.transform.scale(1.f/scalingY, 1.f/scalingY);
            }
            else // The image isn't split
            {
                // Set the scaling
                states.transform.scale(tabWidth / static_cast<float>(m_textureNormal_M.getSize().x), scalingY);

                // Draw the tab image
                if (m_separateSelectedImage)
                {
                    if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                        target.draw(m_textureSelected_M, states);
                    else
                        target.draw(m_textureNormal_M, states);
                }
                else // There is no separate selected image
                {
                    target.draw(m_textureNormal_M, states);

                    if ((m_selectedTab == static_cast<int>(i)) && (m_widgetPhase & WidgetPhase_Selected))
                        target.draw(m_textureSelected_M, states);
                }

                // Undo the scaling
                states.transform.scale(static_cast<float>(m_textureNormal_M.getSize().x) / tabWidth, 1.f / scalingY);
            }

            // Draw the text
            {
                // Give the text the correct color
                if (m_selectedTab == static_cast<int>(i))
                    tempText.setColor(m_selectedTextColor);
                else
                    tempText.setColor(m_textColor);

                // Get the current size of the text, so that we can recalculate the position
                tempText.setString(m_tabNames[i]);
                realRect = tempText.getLocalBounds();

                // Calculate the new position for the text
                if ((m_splitImage) && (tabWidth == (m_textureNormal_L.getSize().x + m_textureNormal_R.getSize().x) * scalingY))
                    realRect.left = ((tabWidth - realRect.width) / 2.f) - realRect.left;
                else
                    realRect.left = m_distanceToSide - realRect.left;
                realRect.top = ((m_tabHeight - defaultRect.height) / 2.f) - defaultRect.top;

                // Move the text to the correct position
                states.transform.translate(std::floor(realRect.left + 0.5f), std::floor(realRect.top + 0.5f));

                // Check if clipping is required for this text
                if (clippingRequired)
                {
                    // Get the global position
                    sf::Vector2f topLeftPosition = states.transform.transformPoint((target.getView().getSize() / 2.f) - target.getView().getCenter());
                    sf::Vector2f bottomRightPosition = states.transform.transformPoint(sf::Vector2f(tabWidth - (2.0f * m_distanceToSide), (m_tabHeight + defaultRect.height) / 2.f) - target.getView().getCenter() + (target.getView().getSize() / 2.f));

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
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
