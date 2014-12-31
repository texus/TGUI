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
#include <TGUI/MenuBar.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar() :
    m_VisibleMenu        (-1),
    m_TextFont           (nullptr),
    m_TextSize           (0),
    m_DistanceToSide     (4),
    m_MinimumSubMenuWidth(125)
    {
        m_Callback.widgetType = Type_MenuBar;

        setSize(0, 20);
        changeColors();

        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar* MenuBar::clone()
    {
        return new MenuBar(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::load(const std::string& configFileFilename)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

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
        if (!configFile.read("MenuBar", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_LoadedConfigFile + ".");
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
                setDistanceToSide(static_cast<unsigned int>(atoi(value.c_str())));
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section MenuBar in " + m_LoadedConfigFile + ".");
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& MenuBar::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(float width, float height)
    {
        m_Size.x = width;
        m_Size.y = height;

        // There is a minimum height
        if (m_Size.y < 10)
            m_Size.y = 10;

        setTextSize(static_cast<unsigned int>(height * 0.75f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f MenuBar::getSize() const
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        Menu menu;

        menu.selectedMenuItem = -1;

        menu.text.setFont(*m_TextFont);
        menu.text.setString(text);
        menu.text.setColor(m_TextColor);
        menu.text.setCharacterSize(m_TextSize);

        m_Menus.push_back(menu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& menu, const sf::String& text)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_Menus.size(); ++i)
        {
            // If this is the menu then add the menu item to it
            if (m_Menus[i].text.getString() == menu)
            {
                sf::Text menuItem;
                menuItem.setFont(*m_TextFont);
                menuItem.setString(text);
                menuItem.setColor(m_TextColor);
                menuItem.setCharacterSize(m_TextSize);

                m_Menus[i].menuItems.push_back(menuItem);
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
        for (unsigned int i = 0; i < m_Menus.size(); ++i)
        {
            // If this is the menu then remove it
            if (m_Menus[i].text.getString() == menu)
            {
                m_Menus.erase(m_Menus.begin() + i);

                // The menu was removed, so it can't remain open
                if (m_VisibleMenu == static_cast<int>(i))
                    m_VisibleMenu = -1;

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
        for (unsigned int i = 0; i < m_Menus.size(); ++i)
        {
            // If this is the menu then search for the menu item
            if (m_Menus[i].text.getString() == menu)
            {
                for (unsigned int j = 0; j < m_Menus[i].menuItems.size(); ++j)
                {
                    // If this is the menu item then remove it
                    if (m_Menus[i].menuItems[j].getString() == menuItem)
                    {
                        m_Menus[i].menuItems.erase(m_Menus[i].menuItems.begin() + j);

                        // The item can't still be selected
                        if (m_Menus[i].selectedMenuItem == static_cast<int>(j))
                            m_Menus[i].selectedMenuItem = -1;

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
        m_Menus.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::changeColors(const sf::Color& backgroundColor, const sf::Color& textColor,
                               const sf::Color& selectedBackgroundColor, const sf::Color& selectedTextColor)
    {
        m_BackgroundColor = backgroundColor;
        m_TextColor = textColor;
        m_SelectedBackgroundColor = selectedBackgroundColor;
        m_SelectedTextColor = selectedTextColor;

        for (unsigned int i = 0; i < m_Menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_Menus[i].menuItems.size(); ++j)
            {
                if (m_Menus[i].selectedMenuItem == static_cast<int>(j))
                    m_Menus[i].menuItems[j].setColor(selectedTextColor);
                else
                    m_Menus[i].menuItems[j].setColor(textColor);
            }

            m_Menus[i].text.setColor(textColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_BackgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextColor(const sf::Color& textColor)
    {
        m_TextColor = textColor;

        for (unsigned int i = 0; i < m_Menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_Menus[i].menuItems.size(); ++j)
            {
                if (m_Menus[i].selectedMenuItem != static_cast<int>(j))
                    m_Menus[i].menuItems[j].setColor(textColor);
            }

            m_Menus[i].text.setColor(textColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor)
    {
        m_SelectedBackgroundColor = selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_SelectedTextColor = selectedTextColor;

        if (m_VisibleMenu != -1)
        {
            if (m_Menus[m_VisibleMenu].selectedMenuItem != -1)
                m_Menus[m_VisibleMenu].menuItems[m_Menus[m_VisibleMenu].selectedMenuItem].setColor(selectedTextColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MenuBar::getBackgroundColor() const
    {
        return m_BackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MenuBar::getTextColor() const
    {
        return m_TextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MenuBar::getSelectedBackgroundColor() const
    {
        return m_SelectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& MenuBar::getSelectedTextColor() const
    {
        return m_SelectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextFont(const sf::Font& font)
    {
        m_TextFont = &font;

        for (unsigned int i = 0; i < m_Menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_Menus[i].menuItems.size(); ++j)
            {
                m_Menus[i].menuItems[j].setFont(font);
            }

            m_Menus[i].text.setFont(font);
        }

        setTextSize(m_TextSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* MenuBar::getTextFont() const
    {
        return m_TextFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextSize(unsigned int size)
    {
        m_TextSize = size;

        for (unsigned int i = 0; i < m_Menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_Menus[i].menuItems.size(); ++j)
                m_Menus[i].menuItems[j].setCharacterSize(m_TextSize);

            m_Menus[i].text.setCharacterSize(m_TextSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getTextSize() const
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setDistanceToSide(unsigned int distanceToSide)
    {
        m_DistanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getDistanceToSide() const
    {
        return m_DistanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setMinimumSubMenuWidth(unsigned int minimumWidth)
    {
        m_MinimumSubMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getMinimumSubMenuWidth() const
    {
        return m_MinimumSubMenuWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::mouseOnWidget(float x, float y)
    {
        if (m_Loaded)
        {
            // Check if the mouse is on top of the menu bar
            if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
                return true;
            else
            {
                // Check if there is a menu open
                if (m_VisibleMenu != -1)
                {
                    // Search the left position of the open menu
                    float left = 0;
                    for (int i = 0; i < m_VisibleMenu; ++i)
                        left += m_Menus[i].text.getLocalBounds().width + (2 * m_DistanceToSide);

                    // Find out what the width of the menu should be
                    float width = 0;
                    for (unsigned int j = 0; j < m_Menus[m_VisibleMenu].menuItems.size(); ++j)
                    {
                        if (width < m_Menus[m_VisibleMenu].menuItems[j].getLocalBounds().width + (3 * m_DistanceToSide))
                            width = m_Menus[m_VisibleMenu].menuItems[j].getLocalBounds().width + (3 * m_DistanceToSide);
                    }

                    // There is a minimum width
                    if (width < m_MinimumSubMenuWidth)
                        width = static_cast<float>(m_MinimumSubMenuWidth);

                    // Check if the mouse is on top of the open menu
                    if (getTransform().transformRect(sf::FloatRect(left, m_Size.y, width, m_Size.y * m_Menus[m_VisibleMenu].menuItems.size())).contains(x, y))
                        return true;
                }
            }
        }

        if (m_MouseHover)
            mouseLeftWidget();

        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMousePressed(float x, float y)
    {
        // Check if a menu should be opened or closed
        if (y <= m_Size.y + getPosition().y)
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (unsigned int i = 0; i < m_Menus.size(); ++i)
            {
                menuWidth += m_Menus[i].text.getLocalBounds().width + (2 * m_DistanceToSide);
                if (x < menuWidth)
                {
                    // Close the menu when it was already open
                    if (m_VisibleMenu == static_cast<int>(i))
                    {
                        if (m_Menus[m_VisibleMenu].selectedMenuItem != -1)
                        {
                            m_Menus[m_VisibleMenu].menuItems[m_Menus[m_VisibleMenu].selectedMenuItem].setColor(m_TextColor);
                            m_Menus[m_VisibleMenu].selectedMenuItem = -1;
                        }

                        m_VisibleMenu = -1;
                    }

                    // If this menu can be opened then do so
                    else if (!m_Menus[i].menuItems.empty())
                        m_VisibleMenu = static_cast<int>(i);

                    break;
                }
            }
        }

        m_MouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleased(float, float y)
    {
        if (m_MouseDown)
        {
            // Check if the mouse is on top of one of the menus
            if (y > m_Size.y + getPosition().y)
            {
                unsigned int selectedMenuItem = static_cast<unsigned int>((y - m_Size.y - getPosition().y) / m_Size.y);

                if (selectedMenuItem < m_Menus[m_VisibleMenu].menuItems.size())
                {
                    if (m_CallbackFunctions[MenuItemClicked].empty() == false)
                    {
                        m_Callback.trigger = MenuItemClicked;
                        m_Callback.text = m_Menus[m_VisibleMenu].menuItems[selectedMenuItem].getString();
                        m_Callback.index = m_VisibleMenu;
                        addCallback();
                    }

                    if (m_VisibleMenu != -1)
                    {
                        if (m_Menus[m_VisibleMenu].selectedMenuItem != -1)
                        {
                            m_Menus[m_VisibleMenu].menuItems[m_Menus[m_VisibleMenu].selectedMenuItem].setColor(m_TextColor);
                            m_Menus[m_VisibleMenu].selectedMenuItem = -1;
                        }

                        m_VisibleMenu = -1;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMoved(float x, float y)
    {
        if (m_MouseHover == false)
            mouseEnteredWidget();

        m_MouseHover = true;

        // Check if the mouse is on top of the menu bar (not on an open menus)
        if (y <= m_Size.y + getPosition().y)
        {
            // Don't open a menu without having clicked first
            if (m_VisibleMenu != -1)
            {
                // Loop through the menus to check if the mouse is on top of them
                float menuWidth = 0;
                for (unsigned int i = 0; i < m_Menus.size(); ++i)
                {
                    menuWidth += m_Menus[i].text.getLocalBounds().width + (2 * m_DistanceToSide);
                    if (x < menuWidth)
                    {
                        // Check if the menu is already open
                        if (m_VisibleMenu == static_cast<int>(i))
                        {
                            // If one of the menu items is selected then unselect it
                            if (m_Menus[m_VisibleMenu].selectedMenuItem != -1)
                            {
                                m_Menus[m_VisibleMenu].menuItems[m_Menus[m_VisibleMenu].selectedMenuItem].setColor(m_TextColor);
                                m_Menus[m_VisibleMenu].selectedMenuItem = -1;
                            }
                        }
                        else // The menu isn't open yet
                        {
                            // If there is another menu open then close it first
                            if (m_VisibleMenu != -1)
                            {
                                // If an item in that other menu was selected then unselect it first
                                if (m_Menus[m_VisibleMenu].selectedMenuItem != -1)
                                {
                                    m_Menus[m_VisibleMenu].menuItems[m_Menus[m_VisibleMenu].selectedMenuItem].setColor(m_TextColor);
                                    m_Menus[m_VisibleMenu].selectedMenuItem = -1;
                                }

                                m_VisibleMenu = -1;
                            }

                            // If this menu can be opened then do so
                            if (!m_Menus[i].menuItems.empty())
                                m_VisibleMenu = static_cast<int>(i);
                        }
                        break;
                    }
                }
            }
        }
        else // The mouse is on top of one of the menus
        {
            // Calculate on what menu item the mouse is located
            int selectedMenuItem = static_cast<int>((y - m_Size.y - getPosition().y) / m_Size.y);

            // Check if the mouse is on a different item than before
            if (selectedMenuItem != m_Menus[m_VisibleMenu].selectedMenuItem)
            {
                // If another of the menu items is selected then unselect it
                if (m_Menus[m_VisibleMenu].selectedMenuItem != -1)
                    m_Menus[m_VisibleMenu].menuItems[m_Menus[m_VisibleMenu].selectedMenuItem].setColor(m_TextColor);

                // Mark the item below the mouse as selected
                m_Menus[m_VisibleMenu].selectedMenuItem = selectedMenuItem;
                m_Menus[m_VisibleMenu].menuItems[m_Menus[m_VisibleMenu].selectedMenuItem].setColor(m_SelectedTextColor);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseNoLongerDown()
    {
        // Check if there is still a menu open
        if (m_VisibleMenu != -1)
        {
            // If an item in that menu was selected then unselect it first
            if (m_Menus[m_VisibleMenu].selectedMenuItem != -1)
            {
                m_Menus[m_VisibleMenu].menuItems[m_Menus[m_VisibleMenu].selectedMenuItem].setColor(m_TextColor);
                m_Menus[m_VisibleMenu].selectedMenuItem = -1;
            }

            m_VisibleMenu = -1;
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
            setTextSize(atoi(value.c_str()));
        }
        else if (property == "distancetoside")
        {
            setDistanceToSide(atoi(value.c_str()));
        }
        else if (property == "minimumsubmenuwidth")
        {
            setMinimumSubMenuWidth(atoi(value.c_str()));
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
            value = "(" + to_string(int(getBackgroundColor().r)) + "," + to_string(int(getBackgroundColor().g)) + "," + to_string(int(getBackgroundColor().b)) + "," + to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "textcolor")
            value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
        else if (property == "selectedbackgroundcolor")
            value = "(" + to_string(int(getSelectedBackgroundColor().r)) + "," + to_string(int(getSelectedBackgroundColor().g))
                    + "," + to_string(int(getSelectedBackgroundColor().b)) + "," + to_string(int(getSelectedBackgroundColor().a)) + ")";
        else if (property == "selectedtextcolor")
            value = "(" + to_string(int(getSelectedTextColor().r)) + "," + to_string(int(getSelectedTextColor().g))
                    + "," + to_string(int(getSelectedTextColor().b)) + "," + to_string(int(getSelectedTextColor().a)) + ")";
        else if (property == "textsize")
            value = to_string(getTextSize());
        else if (property == "distancetoside")
            value = to_string(getDistanceToSide());
        else if (property == "minimumsubmenuwidth")
            value = to_string(getMinimumSubMenuWidth());
        else if (property == "menus")
        {
            std::vector<sf::String> menusList;

            for (auto menuIt = m_Menus.cbegin(); menuIt != m_Menus.cend(); ++menuIt)
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

            if ((m_CallbackFunctions.find(MenuItemClicked) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(MenuItemClicked).size() == 1) && (m_CallbackFunctions.at(MenuItemClicked).front() == nullptr))
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
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
        m_Size.x = m_Parent->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        // Draw the background
        sf::RectangleShape background(m_Size);
        background.setFillColor(m_BackgroundColor);
        target.draw(background, states);

        if (m_Menus.empty())
            return;

        sf::Text tempText(m_Menus[0].text);
        tempText.setString("kg");

        sf::Vector2f textShift;
        textShift.x = -tempText.getLocalBounds().left;
        textShift.y = ((m_Size.y - tempText.getLocalBounds().height) / 2.0f) - tempText.getLocalBounds().top;

        // Draw the menus
        for (unsigned int i = 0; i < m_Menus.size(); ++i)
        {
            states.transform.translate(static_cast<float>(m_DistanceToSide), 0);
            states.transform.translate(textShift.x, textShift.y);
            target.draw(m_Menus[i].text, states);
            states.transform.translate(-textShift.x, -textShift.y);

            // Is the menu open?
            if (m_VisibleMenu == static_cast<int>(i))
            {
                states.transform.translate(-static_cast<float>(m_DistanceToSide), m_Size.y);

                // Find out what the width of the menu should be
                float menuWidth = 0;
                for (unsigned int j = 0; j < m_Menus[i].menuItems.size(); ++j)
                {
                    if (menuWidth < m_Menus[i].menuItems[j].getLocalBounds().width + (3 * m_DistanceToSide))
                        menuWidth = m_Menus[i].menuItems[j].getLocalBounds().width + (3 * m_DistanceToSide);
                }

                // There is a minimum width
                if (menuWidth < m_MinimumSubMenuWidth)
                    menuWidth = static_cast<float>(m_MinimumSubMenuWidth);

                // Draw the background of the menu
                background = sf::RectangleShape(sf::Vector2f(menuWidth, m_Size.y * m_Menus[i].menuItems.size()));
                background.setFillColor(m_BackgroundColor);
                target.draw(background, states);

                // If there is a selected menu item then draw its background
                if (m_Menus[i].selectedMenuItem != -1)
                {
                    states.transform.translate(0, m_Menus[i].selectedMenuItem * m_Size.y);
                    background = sf::RectangleShape(sf::Vector2f(menuWidth, m_Size.y));
                    background.setFillColor(m_SelectedBackgroundColor);
                    target.draw(background, states);
                    states.transform.translate(0, m_Menus[i].selectedMenuItem * -m_Size.y);
                }

                states.transform.translate(2.0f * m_DistanceToSide, 0);
                states.transform.translate(textShift.x, textShift.y);

                // Draw the menu items
                for (unsigned int j = 0; j < m_Menus[i].menuItems.size(); ++j)
                {
                    target.draw(m_Menus[i].menuItems[j], states);

                    states.transform.translate(0, m_Size.y);
                }

                states.transform.translate(-textShift.x, -textShift.y);
                states.transform.translate(m_Menus[i].text.getLocalBounds().width, -m_Size.y * (m_Menus[i].menuItems.size()+1));
            }
            else // The menu isn't open
            {
                states.transform.translate(m_Menus[i].text.getLocalBounds().width + m_DistanceToSide, 0);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
