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
#include <TGUI/Tab.hpp>

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

        float positionX = 0;
        auto it2 = m_texturesSelected.begin();
        for (auto it = m_texturesNormal.begin(); it != m_texturesNormal.end(); ++it, ++it2)
        {
            it->setPosition(positionX, 0);
            it2->setPosition(positionX, 0);

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
        // Add the tab
        m_tabNames.push_back(name);
        m_text.setText(name);

        sf::Vector2f currentSize = getSize();

        // Calculate the width of the tab
        float width = 0;
        if (m_maximumTabWidth)
            width += TGUI_MAXIMUM(TGUI_MINIMUM(m_text.getSize().x + (2 * m_distanceToSide), m_maximumTabWidth), m_textureNormal.getSize().y);
        else
            width += TGUI_MAXIMUM(m_text.getSize().x + (2 * m_distanceToSide), m_textureNormal.getSize().y);

        // Add the new tab sprite
        m_texturesNormal.push_back(m_textureNormal);
        m_texturesSelected.push_back(m_textureSelected);

        m_texturesNormal.back().setSize({width, m_texturesNormal.back().getSize().y});
        m_texturesSelected.back().setSize({width, m_texturesSelected.back().getSize().y});

        m_texturesNormal.back().setPosition({currentSize.x, 0});
        m_texturesSelected.back().setPosition({currentSize.x, 0});

        m_width += width;

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
        for (unsigned int i = 0; i < m_tabNames.size(); ++i)
        {
            // Find the tab that should be selected
            if (m_tabNames[i] == name)
            {
                // Select the tab
                m_selectedTab = i;
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
        auto texturesNormalIt = m_texturesNormal.begin();
        auto texturesSelectedIt = m_texturesSelected.begin();
        for (unsigned int i = 0; i < m_tabNames.size(); ++i)
        {
            // Check if you found the tab to remove
            if (m_tabNames[i] == name)
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
        m_text.setTextFont(font);

        setTextSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Set the text size
        if (m_textSize == 0)
            m_text.setTextSize(static_cast<unsigned int>(m_textureNormal.getSize().y * 0.75f));
        else
            m_text.setTextSize(m_textSize);

        recalculateTabsWidth();
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
            m_text.setText(m_tabNames[i]);

            float width;
            if (m_maximumTabWidth)
                width = TGUI_MAXIMUM(TGUI_MINIMUM(m_text.getSize().x + (2 * m_distanceToSide), m_maximumTabWidth), m_textureNormal.getSize().y);
            else
                width = TGUI_MAXIMUM(m_text.getSize().x + (2 * m_distanceToSide), m_textureNormal.getSize().y);

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
        bool clippingRequired = false;
        unsigned int accumulatedTabWidth = 0;
        Label tempText(m_text);

        // Calculate the height of all strings
        tempText.setText("kg");
        float textHeight = tempText.getSize().y;

        tempText.setPosition(0, (m_textureNormal.getSize().y - textHeight) / 2.0f);

        states.transform *= getTransform();

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
                // Give the text the correct color
                if (m_selectedTab == static_cast<int>(i))
                    tempText.setTextColor(m_selectedTextColor);
                else
                    tempText.setTextColor(m_textColor);

                // Get the current size of the text, so that we can recalculate the position
                tempText.setText(m_tabNames[i]);
                tempText.setPosition({m_distanceToSide + accumulatedTabWidth + ((textureNormalIt->getSize().x - (2*m_distanceToSide) - tempText.getSize().x) / 2.0f), tempText.getPosition().y});

                // Check if clipping is required for this tab
                if (tempText.getSize().x > textureNormalIt->getSize().x - 2 * m_distanceToSide)
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
                                                        ((getAbsolutePosition().y + ((m_textureNormal.getSize().y + textHeight) / 2.f) - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};

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
