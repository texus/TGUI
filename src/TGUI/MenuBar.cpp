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
#include <TGUI/MenuBar.hpp>

#include <cmath>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar() :
    m_visibleMenu        (-1),
    m_textFont           (nullptr),
    m_textSize           (0),
    m_distanceToSide     (4),
    m_minimumSubMenuWidth(125)
    {
        m_callback.widgetType = Type_MenuBar;

        setSize(0, 20);
        changeColors();

        m_loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar* MenuBar::clone()
    {
        return new MenuBar(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

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
        if (!configFile.read("MenuBar", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_loadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "backgroundcolor")
            {
                setBackgroundColor(extractColor(value));
            }
            else if (property == "textcolor")
            {
                setTextColor(extractColor(value));
            }
            else if (property == "selectedbackgroundcolor")
            {
                setSelectedBackgroundColor(extractColor(value));
            }
            else if (property == "selectedtextcolor")
            {
                setSelectedTextColor(extractColor(value));
            }
            else if (property == "distancetoside")
            {
                setDistanceToSide(tgui::stoul(value));
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section MenuBar in " + m_loadedConfigFile + ".");
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& MenuBar::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(float width, float height)
    {
        m_size.x = width;
        m_size.y = height;

        // There is a minimum height
        if (m_size.y < 10)
            m_size.y = 10;

        setTextSize(static_cast<unsigned int>(height * 0.75f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f MenuBar::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        Menu menu;

        menu.selectedMenuItem = -1;

        menu.text.setFont(*m_textFont);
        menu.text.setString(text);
        menu.text.setColor(m_textColor);
        menu.text.setCharacterSize(m_textSize);

        m_menus.push_back(menu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& menu, const sf::String& text)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then add the menu item to it
            if (m_menus[i].text.getString() == menu)
            {
                sf::Text menuItem;
                menuItem.setFont(*m_textFont);
                menuItem.setString(text);
                menuItem.setColor(m_textColor);
                menuItem.setCharacterSize(m_textSize);

                m_menus[i].menuItems.push_back(menuItem);
                return true;
            }
        }

        // Couldn't find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenu(const sf::String& menu)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then remove it
            if (m_menus[i].text.getString() == menu)
            {
                m_menus.erase(m_menus.begin() + i);

                // The menu was removed, so it can't remain open
                if (m_visibleMenu == static_cast<int>(i))
                    m_visibleMenu = -1;

                return true;
            }
        }

        // Couldn't find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const sf::String& menu, const sf::String& menuItem)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then search for the menu item
            if (m_menus[i].text.getString() == menu)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    // If this is the menu item then remove it
                    if (m_menus[i].menuItems[j].getString() == menuItem)
                    {
                        m_menus[i].menuItems.erase(m_menus[i].menuItems.begin() + j);

                        // The item can't still be selected
                        if (m_menus[i].selectedMenuItem == static_cast<int>(j))
                            m_menus[i].selectedMenuItem = -1;

                        return true;
                    }
                }
            }
        }

        // Couldn't find menu item
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::removeAllMenus()
    {
        m_menus.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::changeColors(const sf::Color& backgroundColor, const sf::Color& textColor,
                               const sf::Color& selectedBackgroundColor, const sf::Color& selectedTextColor)
    {
        m_backgroundColor = backgroundColor;
        m_textColor = textColor;
        m_selectedBackgroundColor = selectedBackgroundColor;
        m_selectedTextColor = selectedTextColor;

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
            {
                if (m_menus[i].selectedMenuItem == static_cast<int>(j))
                    m_menus[i].menuItems[j].setColor(selectedTextColor);
                else
                    m_menus[i].menuItems[j].setColor(textColor);
            }

            m_menus[i].text.setColor(textColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextColor(const sf::Color& textColor)
    {
        m_textColor = textColor;

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
            {
                if (m_menus[i].selectedMenuItem != static_cast<int>(j))
                    m_menus[i].menuItems[j].setColor(textColor);
            }

            m_menus[i].text.setColor(textColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor)
    {
        m_selectedBackgroundColor = selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_selectedTextColor = selectedTextColor;

        if (m_visibleMenu != -1)
        {
            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(selectedTextColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MenuBar::getBackgroundColor() const
    {
        return m_backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MenuBar::getTextColor() const
    {
        return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MenuBar::getSelectedBackgroundColor() const
    {
        return m_selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MenuBar::getSelectedTextColor() const
    {
        return m_selectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextFont(const sf::Font& font)
    {
        m_textFont = &font;

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
            {
                m_menus[i].menuItems[j].setFont(font);
            }

            m_menus[i].text.setFont(font);
        }

        setTextSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* MenuBar::getTextFont() const
    {
        return m_textFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextSize(unsigned int size)
    {
        m_textSize = size;

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                m_menus[i].menuItems[j].setCharacterSize(m_textSize);

            m_menus[i].text.setCharacterSize(m_textSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setDistanceToSide(unsigned int distanceToSide)
    {
        m_distanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getDistanceToSide() const
    {
        return m_distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setMinimumSubMenuWidth(unsigned int minimumWidth)
    {
        m_minimumSubMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getMinimumSubMenuWidth() const
    {
        return m_minimumSubMenuWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::mouseOnWidget(float x, float y)
    {
        if (m_loaded)
        {
            // Check if the mouse is on top of the menu bar
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x, m_size.y)).contains(x, y))
                return true;
            else
            {
                // Check if there is a menu open
                if (m_visibleMenu != -1)
                {
                    // Search the left position of the open menu
                    float left = 0;
                    for (int i = 0; i < m_visibleMenu; ++i)
                        left += m_menus[i].text.getLocalBounds().width + (2 * m_distanceToSide);

                    // Find out what the width of the menu should be
                    float width = 0;
                    for (unsigned int j = 0; j < m_menus[m_visibleMenu].menuItems.size(); ++j)
                    {
                        if (width < m_menus[m_visibleMenu].menuItems[j].getLocalBounds().width + (3 * m_distanceToSide))
                            width = m_menus[m_visibleMenu].menuItems[j].getLocalBounds().width + (3 * m_distanceToSide);
                    }

                    // There is a minimum width
                    if (width < m_minimumSubMenuWidth)
                        width = static_cast<float>(m_minimumSubMenuWidth);

                    // Check if the mouse is on top of the open menu
                    if (getTransform().transformRect(sf::FloatRect(left, m_size.y, width, m_size.y * m_menus[m_visibleMenu].menuItems.size())).contains(x, y))
                        return true;
                }
            }
        }

        if (m_mouseHover)
            mouseLeftWidget();

        m_mouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMousePressed(float x, float y)
    {
        // Check if a menu should be opened or closed
        if (y <= m_size.y + getPosition().y)
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getLocalBounds().width + (2 * m_distanceToSide);
                if (x < menuWidth)
                {
                    // Close the menu when it was already open
                    if (m_visibleMenu == static_cast<int>(i))
                    {
                        if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                        {
                            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColor);
                            m_menus[m_visibleMenu].selectedMenuItem = -1;
                        }

                        m_visibleMenu = -1;
                    }

                    // If this menu can be opened then do so
                    else if (!m_menus[i].menuItems.empty())
                        m_visibleMenu = static_cast<int>(i);

                    break;
                }
            }
        }

        m_mouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleased(float, float y)
    {
        if (m_mouseDown)
        {
            // Check if the mouse is on top of one of the menus
            if (y > m_size.y + getPosition().y)
            {
                unsigned int selectedMenuItem = static_cast<unsigned int>((y - m_size.y - getPosition().y) / m_size.y);

                if (selectedMenuItem < m_menus[m_visibleMenu].menuItems.size())
                {
                    if (m_callbackFunctions[MenuItemClicked].empty() == false)
                    {
                        m_callback.trigger = MenuItemClicked;
                        m_callback.text = m_menus[m_visibleMenu].menuItems[selectedMenuItem].getString();
                        m_callback.index = m_visibleMenu;
                        addCallback();
                    }

                    if (m_visibleMenu != -1)
                    {
                        if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                        {
                            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColor);
                            m_menus[m_visibleMenu].selectedMenuItem = -1;
                        }

                        m_visibleMenu = -1;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMoved(float x, float y)
    {
        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;

        // Check if the mouse is on top of the menu bar (not on an open menus)
        if (y <= m_size.y + getPosition().y)
        {
            // Don't open a menu without having clicked first
            if (m_visibleMenu != -1)
            {
                // Loop through the menus to check if the mouse is on top of them
                float menuWidth = 0;
                for (unsigned int i = 0; i < m_menus.size(); ++i)
                {
                    menuWidth += m_menus[i].text.getLocalBounds().width + (2 * m_distanceToSide);
                    if (x < menuWidth)
                    {
                        // Check if the menu is already open
                        if (m_visibleMenu == static_cast<int>(i))
                        {
                            // If one of the menu items is selected then unselect it
                            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                            {
                                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColor);
                                m_menus[m_visibleMenu].selectedMenuItem = -1;
                            }
                        }
                        else // The menu isn't open yet
                        {
                            // If there is another menu open then close it first
                            if (m_visibleMenu != -1)
                            {
                                // If an item in that other menu was selected then unselect it first
                                if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                                {
                                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColor);
                                    m_menus[m_visibleMenu].selectedMenuItem = -1;
                                }

                                m_visibleMenu = -1;
                            }

                            // If this menu can be opened then do so
                            if (!m_menus[i].menuItems.empty())
                                m_visibleMenu = static_cast<int>(i);
                        }
                        break;
                    }
                }
            }
        }
        else // The mouse is on top of one of the menus
        {
            // Calculate on what menu item the mouse is located
            int selectedMenuItem = static_cast<int>((y - m_size.y - getPosition().y) / m_size.y);

            // Check if the mouse is on a different item than before
            if (selectedMenuItem != m_menus[m_visibleMenu].selectedMenuItem)
            {
                // If another of the menu items is selected then unselect it
                if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColor);

                // Mark the item below the mouse as selected
                m_menus[m_visibleMenu].selectedMenuItem = selectedMenuItem;
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_selectedTextColor);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseNoLongerDown()
    {
        // Check if there is still a menu open
        if (m_visibleMenu != -1)
        {
            // If an item in that menu was selected then unselect it first
            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
            {
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColor);
                m_menus[m_visibleMenu].selectedMenuItem = -1;
            }

            m_visibleMenu = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "backgroundcolor")
        {
            setBackgroundColor(extractColor(value));
        }
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "selectedbackgroundcolor")
        {
            setSelectedBackgroundColor(extractColor(value));
        }
        else if (property == "selectedtextcolor")
        {
            setSelectedTextColor(extractColor(value));
        }
        else if (property == "textsize")
        {
            setTextSize(tgui::stoi(value));
        }
        else if (property == "distancetoside")
        {
            setDistanceToSide(tgui::stoi(value));
        }
        else if (property == "minimumsubmenuwidth")
        {
            setMinimumSubMenuWidth(tgui::stoi(value));
        }
        else if (property == "menus")
        {
            removeAllMenus();

            std::vector<sf::String> menus;
            decodeList(value, menus);

            for (auto menuIt = menus.begin(); menuIt != menus.end(); ++menuIt)
            {
                std::string::size_type commaPos = menuIt->find(',');
                if (commaPos != std::string::npos)
                {
                    std::vector<sf::String> menuItems;
                    std::string menu = menuIt->toAnsiString().substr(0, commaPos);
                    menuIt->erase(0, commaPos + 1);

                   addMenu(menu);
                   decodeList(*menuIt, menuItems);

                   for (auto menuItemIt = menuItems.cbegin(); menuItemIt != menuItems.cend(); ++menuItemIt)
                        addMenuItem(menu, *menuItemIt);
                }
                else // No comma found
                    addMenu(*menuIt);
            }
        }
        else if (property == "callback")
        {
            Widget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "MenuItemClicked") || (*it == "menuitemclicked"))
                    bindCallback(MenuItemClicked);
            }
        }
        else // The property didn't match
            return Widget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "backgroundcolor")
            value = "(" + tgui::to_string(int(getBackgroundColor().r)) + "," + tgui::to_string(int(getBackgroundColor().g)) + "," + tgui::to_string(int(getBackgroundColor().b)) + "," + tgui::to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "selectedbackgroundcolor")
            value = "(" + tgui::to_string(int(getSelectedBackgroundColor().r)) + "," + tgui::to_string(int(getSelectedBackgroundColor().g))
                    + "," + tgui::to_string(int(getSelectedBackgroundColor().b)) + "," + tgui::to_string(int(getSelectedBackgroundColor().a)) + ")";
        else if (property == "selectedtextcolor")
            value = "(" + tgui::to_string(int(getSelectedTextColor().r)) + "," + tgui::to_string(int(getSelectedTextColor().g))
                    + "," + tgui::to_string(int(getSelectedTextColor().b)) + "," + tgui::to_string(int(getSelectedTextColor().a)) + ")";
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "distancetoside")
            value = tgui::to_string(getDistanceToSide());
        else if (property == "minimumsubmenuwidth")
            value = tgui::to_string(getMinimumSubMenuWidth());
        else if (property == "menus")
        {
            std::vector<sf::String> menusList;

            for (auto menuIt = m_menus.cbegin(); menuIt != m_menus.cend(); ++menuIt)
            {
                std::vector<sf::String> menuItemsList;
                menuItemsList.push_back(menuIt->text.getString());

                for (auto menuItemIt = menuIt->menuItems.cbegin(); menuItemIt != menuIt->menuItems.cend(); ++menuItemIt)
                    menuItemsList.push_back(menuItemIt->getString());

                std::string menuItemsString;
                encodeList(menuItemsList, menuItemsString);

                menusList.push_back(menuItemsString);
            }

            encodeList(menusList, value);
        }
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(MenuItemClicked) != m_callbackFunctions.end()) && (m_callbackFunctions.at(MenuItemClicked).size() == 1) && (m_callbackFunctions.at(MenuItemClicked).front() == nullptr))
                callbacks.push_back("MenuItemClicked");

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

    std::list< std::pair<std::string, std::string> > MenuBar::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("BackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedBackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedTextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("DistanceToSide", "uint"));
        list.push_back(std::pair<std::string, std::string>("MinimumSubMenuWidth", "uint"));
        list.push_back(std::pair<std::string, std::string>("menus", "string"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());

        m_size.x = m_parent->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        // Draw the background
        sf::RectangleShape background(m_size);
        background.setFillColor(m_backgroundColor);
        target.draw(background, states);

        if (m_menus.empty())
            return;

        sf::Text tempText(m_menus[0].text);
        tempText.setString("kg");

        sf::Vector2f textShift;
        textShift.x = -tempText.getLocalBounds().left;
        textShift.y = ((m_size.y - tempText.getLocalBounds().height) / 2.0f) - tempText.getLocalBounds().top;

        // Draw the menus
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            states.transform.translate(static_cast<float>(m_distanceToSide), 0);
            states.transform.translate(textShift.x, textShift.y);
            target.draw(m_menus[i].text, states);
            states.transform.translate(-textShift.x, -textShift.y);

            // Is the menu open?
            if (m_visibleMenu == static_cast<int>(i))
            {
                states.transform.translate(-static_cast<float>(m_distanceToSide), m_size.y);

                // Find out what the width of the menu should be
                float menuWidth = 0;
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    if (menuWidth < m_menus[i].menuItems[j].getLocalBounds().width + (3 * m_distanceToSide))
                        menuWidth = m_menus[i].menuItems[j].getLocalBounds().width + (3 * m_distanceToSide);
                }

                // There is a minimum width
                if (menuWidth < m_minimumSubMenuWidth)
                    menuWidth = static_cast<float>(m_minimumSubMenuWidth);

                // Draw the background of the menu
                background = sf::RectangleShape(sf::Vector2f(menuWidth, m_size.y * m_menus[i].menuItems.size()));
                background.setFillColor(m_backgroundColor);
                target.draw(background, states);

                // If there is a selected menu item then draw its background
                if (m_menus[i].selectedMenuItem != -1)
                {
                    states.transform.translate(0, m_menus[i].selectedMenuItem * m_size.y);
                    background = sf::RectangleShape(sf::Vector2f(menuWidth, m_size.y));
                    background.setFillColor(m_selectedBackgroundColor);
                    target.draw(background, states);
                    states.transform.translate(0, m_menus[i].selectedMenuItem * -m_size.y);
                }

                states.transform.translate(2.0f * m_distanceToSide, 0);
                states.transform.translate(textShift.x, textShift.y);

                // Draw the menu items
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    target.draw(m_menus[i].menuItems[j], states);

                    states.transform.translate(0, m_size.y);
                }

                states.transform.translate(-textShift.x, -textShift.y);
                states.transform.translate(m_menus[i].text.getLocalBounds().width, -m_size.y * (m_menus[i].menuItems.size()+1));
            }
            else // The menu isn't open
            {
                states.transform.translate(m_menus[i].text.getLocalBounds().width + m_distanceToSide, 0);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
