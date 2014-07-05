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
#include <TGUI/Tab.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Tab()
    {
        m_callback.widgetType = Type_Tab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Ptr Tab::create(const std::string& configFileFilename)
    {
        auto tab = std::make_shared<Tab>();

        tab->m_loadedConfigFile = getResourcePath() + configFileFilename;

        tab->m_texturesNormal.clear();
        tab->m_texturesSelected.clear();

        // Open the config file
        ConfigFile configFile{tab->m_loadedConfigFile, "Tab"};

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = tab->m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = tab->m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "separateselectedimage")
                tab->m_separateSelectedImage = configFile.readBool(it);
            else if (it->first == "textcolor")
                tab->m_textColor = configFile.readColor(it);
            else if (it->first == "selectedtextcolor")
                tab->m_selectedTextColor = configFile.readColor(it);
            else if (it->first == "distancetoside")
                tab->setDistanceToSide(tgui::stoul(it->second));
            else if (it->first == "normalimage")
                configFile.readTexture(it, configFileFolder, tab->m_textureNormal);
            else if (it->first == "selectedimage")
                configFile.readTexture(it, configFileFolder, tab->m_textureSelected);
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section Tab in " + tab->m_loadedConfigFile + "."};
        }

        // Clear the vectors
        tab->m_tabNames.clear();

        // Make sure the required texture was loaded
        if (tab->m_textureNormal.getData() == nullptr)
            throw Exception{"NormalImage wasn't loaded. Is the Tab section in " + tab->m_loadedConfigFile + " complete?"};

        // Recalculate the text size when auto sizing
        if (tab->m_textSize == 0)
            tab->setTextSize(0);

        return tab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        float positionX = getPosition().x;
        float textHeight = sf::Text{"kg", *getTextFont(), getTextSize()}.getLocalBounds().height;

        auto it2 = m_texturesSelected.begin();
        auto tabIt = m_tabNames.begin();
        for (auto it = m_texturesNormal.begin(); it != m_texturesNormal.end(); ++it, ++it2, ++tabIt)
        {
            it->setPosition({positionX, getPosition().y});
            it2->setPosition({positionX, getPosition().y});

            tabIt->setPosition({positionX + m_distanceToSide + ((it->getSize().x - (2*m_distanceToSide) - tabIt->getSize().x) / 2.0f),
                                getPosition().y + ((getSize().y - textHeight) / 2.0f)});

            positionX += it->getSize().x;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setSize(const Layout&)
    {
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::add(const sf::String& name, bool selectTab)
    {
        // Create the new tab
        Label newTab;
        newTab.setTextFont(*getTextFont());
        newTab.setTextColor(getTextColor());
        newTab.setTextSize(getTextSize());
        newTab.setText(name);

        // Calculate the width of the tab
        float width;
        if (m_maximumTabWidth)
            width = TGUI_MAXIMUM(TGUI_MINIMUM(newTab.getSize().x + (2 * m_distanceToSide), m_maximumTabWidth), m_textureNormal.getSize().y);
        else
            width = TGUI_MAXIMUM(newTab.getSize().x + (2 * m_distanceToSide), m_textureNormal.getSize().y);

        // Add the new tab sprite
        m_texturesNormal.push_back(m_textureNormal);
        m_texturesSelected.push_back(m_textureSelected);

        m_texturesNormal.back().setSize({width, m_texturesNormal.back().getSize().y});
        m_texturesSelected.back().setSize({width, m_texturesSelected.back().getSize().y});

        m_texturesNormal.back().setPosition({getPosition().x + getSize().x, getPosition().y});
        m_texturesSelected.back().setPosition({getPosition().x + getSize().x, getPosition().y});

        // Set the correct size of the tab text
        float textHeight = sf::Text{"kg", *getTextFont(), getTextSize()}.getLocalBounds().height;
        newTab.setPosition({getPosition().x + getSize().x + m_distanceToSide + ((width - (2*m_distanceToSide) - newTab.getSize().x) / 2.0f),
                            getPosition().y + ((getSize().y - textHeight) / 2.0f)});

        // If the tab has to be selected then do so
        if (selectTab)
        {
            if (m_selectedTab >= 0)
                m_tabNames[m_selectedTab].setTextColor(m_textColor);

            m_selectedTab = m_tabNames.size();
            newTab.setTextColor(m_selectedTextColor);
        }

        // Add the tab
        m_tabNames.push_back(std::move(newTab));

        m_width += width;

        // Return the index of the new tab
        return m_tabNames.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(const sf::String& name)
    {
        if (m_selectedTab >= 0)
            m_tabNames[m_selectedTab].setTextColor(m_textColor);

        // Loop through all tabs
        for (unsigned int i = 0; i < m_tabNames.size(); ++i)
        {
            // Find the tab that should be selected
            if (m_tabNames[i].getText() == name)
            {
                // Select the tab
                m_selectedTab = i;
                m_tabNames[i].setTextColor(m_selectedTextColor);
                return;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(unsigned int index)
    {
        // If the index is too big then do nothing
        if (index > m_tabNames.size() - 1)
            return;

        if (m_selectedTab >= 0)
            m_tabNames[m_selectedTab].setTextColor(m_textColor);

        // Select the tab
        m_selectedTab = index;
        m_tabNames[m_selectedTab].setTextColor(m_selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::deselect()
    {
        if (m_selectedTab >= 0)
            m_tabNames[m_selectedTab].setTextColor(m_textColor);

        m_selectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(const sf::String& name)
    {
        // Loop through all tabs
        auto texturesNormalIt = m_texturesNormal.begin();
        auto texturesSelectedIt = m_texturesSelected.begin();
        for (unsigned int i = 0; i < m_tabNames.size(); ++i)
        {
            // Check if you found the tab to remove
            if (m_tabNames[i].getText() == name)
            {
                m_width -= texturesNormalIt->getSize().x;

                // Remove the tab
                m_tabNames.erase(m_tabNames.begin() + i);

                m_texturesNormal.erase(texturesNormalIt);
                m_texturesSelected.erase(texturesSelectedIt);

                // Check if the selected tab should change
                if (m_selectedTab == static_cast<int>(i))
                    m_selectedTab = -1;
                else if (m_selectedTab > static_cast<int>(i))
                    --m_selectedTab;

                // Recalculate the positions of the tabs
                updatePosition();
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(unsigned int index)
    {
        // The index can't be too high
        if (index > m_tabNames.size() - 1)
            return;

        // Remove the tab
        m_tabNames.erase(m_tabNames.begin() + index);

        auto texturesNormalIt = m_texturesNormal.begin();
        std::advance(texturesNormalIt, index);
        m_width -= texturesNormalIt->getSize().x;

        auto texturesSelectedIt = m_texturesSelected.begin();
        std::advance(texturesSelectedIt, index);

        m_texturesNormal.erase(texturesNormalIt);
        m_texturesSelected.erase(texturesSelectedIt);

        // Check if the selected tab should change
        if (m_selectedTab == static_cast<int>(index))
            m_selectedTab = -1;
        else if (m_selectedTab > static_cast<int>(index))
            --m_selectedTab;

        // Recalculate the positions of the tabs
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::removeAll()
    {
        m_tabNames.clear();
        m_selectedTab = -1;

        m_texturesNormal.clear();
        m_texturesSelected.clear();

        m_width = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextFont(const sf::Font& font)
    {
        m_textFont = &font;
        for (auto& tab : m_tabNames)
            tab.setTextFont(font);

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextSize(unsigned int size)
    {
        if (m_textSize != size)
        {
            m_textSize = size;
            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTextSize() const
    {
        if (m_textSize == 0)
            return static_cast<unsigned int>(m_textureNormal.getSize().y * 0.75f);
        else
            return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTabHeight(float height)
    {
        m_textureNormal.setSize({m_textureNormal.getSize().x, height});
        m_textureSelected.setSize({m_textureSelected.getSize().x, height});

        for (auto it = m_texturesNormal.begin(); it != m_texturesNormal.end(); ++it)
            it->setSize({it->getSize().x, height});

        for (auto it = m_texturesSelected.begin(); it != m_texturesSelected.end(); ++it)
            it->setSize({it->getSize().x, height});

        // Recalculate the size when auto sizing
        if (m_textSize == 0)
            setTextSize(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setMaximumTabWidth(unsigned int maximumWidth)
    {
        m_maximumTabWidth = maximumWidth;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setDistanceToSide(unsigned int distanceToSide)
    {
        m_distanceToSide = distanceToSide;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_textureNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureSelected.setColor(sf::Color(255, 255, 255, m_opacity));

        for (auto it = m_texturesNormal.begin(); it != m_texturesNormal.end(); ++it)
            it->setColor(sf::Color(255, 255, 255, m_opacity));

        for (auto it = m_texturesSelected.begin(); it != m_texturesSelected.end(); ++it)
            it->setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::mouseOnWidget(float x, float y)
    {
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
            return true;

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
        auto it = m_texturesNormal.cbegin();
        for (unsigned int i = 0; i < m_texturesNormal.size(); ++i, ++it)
        {
            // Append the width of the tab
            width += it->getSize().x;

            // Check if the mouse went down on the tab
            if (x < width)
            {
                if (m_selectedTab >= 0)
                    m_tabNames[m_selectedTab].setTextColor(m_textColor);

                // Select this tab
                m_selectedTab = i;
                m_tabNames[m_selectedTab].setTextColor(m_selectedTextColor);

                // Add the callback (if the user requested it)
                if (m_callbackFunctions[TabChanged].empty() == false)
                {
                    m_callback.trigger = TabChanged;
                    m_callback.value   = m_selectedTab;
                    m_callback.text    = m_tabNames[i].getText();
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
/**
    void Tab::setProperty(std::string property, const std::string& value)
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
            setTextSize(tgui::stoi(value));
        }
        else if (property == "tabheight")
        {
            setTabHeight(tgui::stoi(value));
        }
        else if (property == "maximumtabwidth")
        {
            setMaximumTabWidth(tgui::stoi(value));
        }
        else if (property == "distancetoside")
        {
            setDistanceToSide(tgui::stoi(value));
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
            select(tgui::stoi(value));
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
            Widget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "tabheight")
            value = tgui::to_string(getTabHeight());
        else if (property == "maximumtabwidth")
            value = tgui::to_string(getMaximumTabWidth());
        else if (property == "distancetoside")
            value = tgui::to_string(getDistanceToSide());
        else if (property == "tabs")
            encodeList(m_tabNames, value);
        else if (property == "selectedtab")
            value = tgui::to_string(getSelectedIndex());
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
            Widget::getProperty(property, value);
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
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::recalculateTabsWidth()
    {
        m_width = 0;

        // Recalculate the name widths
        auto textureNormalIt = m_texturesNormal.begin();
        auto textureSelectedIt = m_texturesSelected.begin();
        for (unsigned int i = 0; i < m_texturesNormal.size(); ++i, ++textureNormalIt, ++textureSelectedIt)
        {
            float width;
            if (m_maximumTabWidth)
                width = TGUI_MAXIMUM(TGUI_MINIMUM(m_tabNames[i].getSize().x + (2 * m_distanceToSide), m_maximumTabWidth), m_textureNormal.getSize().y);
            else
                width = TGUI_MAXIMUM(m_tabNames[i].getSize().x + (2 * m_distanceToSide), m_textureNormal.getSize().y);

            textureNormalIt->setSize({width, textureNormalIt->getSize().y});
            textureSelectedIt->setSize({width, textureSelectedIt->getSize().y});

            m_width += width;
        }

        // Recalculate the positions of the tabs
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        GLint scissor[4];
        unsigned int accumulatedTabWidth = 0;

        // Loop through all tabs
        auto textureNormalIt = m_texturesNormal.cbegin();
        auto textureSelectedIt = m_texturesSelected.cbegin();
        for (unsigned int i = 0; i < m_tabNames.size(); ++i, ++textureNormalIt, ++textureSelectedIt)
        {
            // Draw the tab image
            if (m_separateSelectedImage)
            {
                if ((m_selectedTab == static_cast<int>(i)) && textureSelectedIt->getData())
                    target.draw(*textureSelectedIt, states);
                else
                    target.draw(*textureNormalIt, states);
            }
            else // There is no separate selected image
            {
                target.draw(*textureNormalIt, states);

                if ((m_selectedTab == static_cast<int>(i)) && textureSelectedIt->getData())
                    target.draw(*textureSelectedIt, states);
            }

            // Draw the text
            {
                // Check if clipping is required for this tab
                bool clippingRequired = false;
                if (m_tabNames[i].getSize().x > textureNormalIt->getSize().x - 2 * m_distanceToSide)
                    clippingRequired = true;

                // Check if clipping is required for this text
                if (clippingRequired)
                {
                    // Get the old clipping area
                    glGetIntegerv(GL_SCISSOR_BOX, scissor);

                    const sf::View& view = target.getView();

                    // Calculate the scale factor of the view
                    float scaleViewX = target.getSize().x / view.getSize().x;
                    float scaleViewY = target.getSize().y / view.getSize().y;

                    // Get the global position
                    sf::Vector2f topLeftPosition = {((getAbsolutePosition().x + accumulatedTabWidth + m_distanceToSide + (view.getSize().x / 2.f) - view.getCenter().x) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y + (view.getSize().y / 2.f) - view.getCenter().y) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
                    sf::Vector2f bottomRightPosition = {((getAbsolutePosition().x + accumulatedTabWidth + textureNormalIt->getSize().x - m_distanceToSide - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                        ((getAbsolutePosition().y + ((m_textureNormal.getSize().y + m_tabNames[i].getSize().y) / 2.f) - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};

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
                target.draw(m_tabNames[i], states);

                // Reset the old clipping area when needed
                if (clippingRequired)
                {
                    clippingRequired = false;
                    glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
                }
            }

            accumulatedTabWidth += textureNormalIt->getSize().x;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
