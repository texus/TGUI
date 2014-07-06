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
#include <TGUI/MenuBar.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar()
    {
        m_callback.widgetType = Type_MenuBar;

        setSize({0, 20});
        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::create(const std::string& configFileFilename)
    {
        auto menuBar = std::make_shared<MenuBar>();

        menuBar->m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Open the config file
        ConfigFile configFile{menuBar->m_loadedConfigFile, "MenuBar"};

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "backgroundcolor")
                menuBar->setBackgroundColor(extractColor(it->second));
            else if (it->first == "textcolor")
                menuBar->setTextColor(extractColor(it->second));
            else if (it->first == "selectedbackgroundcolor")
                menuBar->setSelectedBackgroundColor(extractColor(it->second));
            else if (it->first == "selectedtextcolor")
                menuBar->setSelectedTextColor(extractColor(it->second));
            else if (it->first == "distancetoside")
                menuBar->setDistanceToSide(tgui::stoul(it->second));
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section MenuBar in " + menuBar->m_loadedConfigFile + "."};
        }

        return menuBar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        if (!m_menus.empty())
        {
            Label tempText(m_menus[0].text);
            tempText.setText("kg");

            // Position the menus
            sf::Vector2f pos = {getPosition().x, getPosition().y + (getSize().y - tempText.getSize().y) / 2.0f};
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                m_menus[i].text.setPosition({pos.x + m_distanceToSide, pos.y});

                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    m_menus[i].menuItems[j].setPosition(pos.x + 2*m_distanceToSide, pos.y + (j+1)*getSize().y);

                pos.x += m_menus[i].text.getSize().x + 2*m_distanceToSide;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout& size)
    {
        Widget::setSize(size);

        setTextSize(static_cast<unsigned int>(getSize().y * 0.75f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        Menu newMenu;

        newMenu.selectedMenuItem = -1;

        newMenu.text.setTextFont(*m_textFont);
        newMenu.text.setText(text);
        newMenu.text.setTextColor(m_textColor);
        newMenu.text.setTextSize(m_textSize);

        m_menus.push_back(std::move(newMenu));

        // Update the position of the menus
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& menu, const sf::String& text)
    {
        Label tempText(m_menus[0].text);
        tempText.setText("kg");

        sf::Vector2f pos = {getPosition().x, getPosition().y + (getSize().y - tempText.getSize().y) / 2.0f};

        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then add the menu item to it
            if (m_menus[i].text.getText() == menu)
            {
                Label menuItem;
                menuItem.setTextFont(*m_textFont);
                menuItem.setText(text);
                menuItem.setTextColor(m_textColor);
                menuItem.setTextSize(m_textSize);

                m_menus[i].menuItems.push_back(std::move(menuItem));

                // Position the new menu item
                m_menus[i].menuItems.back().setPosition({pos.x + 2*m_distanceToSide, pos.y + m_menus[i].menuItems.size() * getSize().y});

                return true;
            }

            pos.x += m_menus[i].text.getSize().x + 2*m_distanceToSide;
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
            if (m_menus[i].text.getText() == menu)
            {
                m_menus.erase(m_menus.begin() + i);

                // The menu was removed, so it can't remain open
                if (m_visibleMenu == static_cast<int>(i))
                    m_visibleMenu = -1;

                // Update the position of the menus
                updatePosition();
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
            if (m_menus[i].text.getText() == menu)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    // If this is the menu item then remove it
                    if (m_menus[i].menuItems[j].getText() == menuItem)
                    {
                        m_menus[i].menuItems.erase(m_menus[i].menuItems.begin() + j);

                        // The item can't still be selected
                        if (m_menus[i].selectedMenuItem == static_cast<int>(j))
                            m_menus[i].selectedMenuItem = -1;

                        // Update the position of the menus
                        updatePosition();
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
                    m_menus[i].menuItems[j].setTextColor(selectedTextColor);
                else
                    m_menus[i].menuItems[j].setTextColor(textColor);
            }

            m_menus[i].text.setTextColor(textColor);
        }
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
                    m_menus[i].menuItems[j].setTextColor(textColor);
            }

            m_menus[i].text.setTextColor(textColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_selectedTextColor = selectedTextColor;

        if (m_visibleMenu != -1)
        {
            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(selectedTextColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextFont(const sf::Font& font)
    {
        m_textFont = &font;

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
            {
                m_menus[i].menuItems[j].setTextFont(font);
            }

            m_menus[i].text.setTextFont(font);
        }

        setTextSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextSize(unsigned int size)
    {
        m_textSize = size;

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                m_menus[i].menuItems[j].setTextSize(m_textSize);

            m_menus[i].text.setTextSize(m_textSize);
        }

        // Update the position of the items
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the menu bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
            return true;
        else
        {
            // Check if there is a menu open
            if (m_visibleMenu != -1)
            {
                // Search the left position of the open menu
                float left = 0;
                for (int i = 0; i < m_visibleMenu; ++i)
                    left += m_menus[i].text.getSize().x + (2 * m_distanceToSide);

                // Find out what the width of the menu should be
                float width = m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menus[m_visibleMenu].menuItems.size(); ++j)
                {
                    if (width < m_menus[m_visibleMenu].menuItems[j].getSize().x + (3 * m_distanceToSide))
                        width = m_menus[m_visibleMenu].menuItems[j].getSize().x + (3 * m_distanceToSide);
                }

                // Check if the mouse is on top of the open menu
                if (getTransform().transformRect(sf::FloatRect(left, getSize().y, width, getSize().y * m_menus[m_visibleMenu].menuItems.size())).contains(x, y))
                    return true;
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
        if (y <= getSize().y + getPosition().y)
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSide);
                if (x < menuWidth)
                {
                    // Close the menu when it was already open
                    if (m_visibleMenu == static_cast<int>(i))
                    {
                        if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                        {
                            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(m_textColor);
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
            if (y > getSize().y + getPosition().y)
            {
                unsigned int selectedMenuItem = static_cast<unsigned int>((y - getSize().y - getPosition().y) / getSize().y);

                if (selectedMenuItem < m_menus[m_visibleMenu].menuItems.size())
                {
                    if (m_callbackFunctions[MenuItemClicked].empty() == false)
                    {
                        m_callback.trigger = MenuItemClicked;
                        m_callback.text = m_menus[m_visibleMenu].menuItems[selectedMenuItem].getText();
                        m_callback.index = m_visibleMenu;
                        addCallback();
                    }

                    if (m_visibleMenu != -1)
                    {
                        if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                        {
                            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(m_textColor);
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
        if (y <= getSize().y + getPosition().y)
        {
            // Don't open a menu without having clicked first
            if (m_visibleMenu != -1)
            {
                // Loop through the menus to check if the mouse is on top of them
                float menuWidth = 0;
                for (unsigned int i = 0; i < m_menus.size(); ++i)
                {
                    menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSide);
                    if (x < menuWidth)
                    {
                        // Check if the menu is already open
                        if (m_visibleMenu == static_cast<int>(i))
                        {
                            // If one of the menu items is selected then unselect it
                            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                            {
                                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(m_textColor);
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
                                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(m_textColor);
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
            int selectedMenuItem = static_cast<int>((y - getSize().y - getPosition().y) / getSize().y);

            // Check if the mouse is on a different item than before
            if (selectedMenuItem != m_menus[m_visibleMenu].selectedMenuItem)
            {
                // If another of the menu items is selected then unselect it
                if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(m_textColor);

                // Mark the item below the mouse as selected
                m_menus[m_visibleMenu].selectedMenuItem = selectedMenuItem;
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(m_selectedTextColor);
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
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(m_textColor);
                m_menus[m_visibleMenu].selectedMenuItem = -1;
            }

            m_visibleMenu = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());

        setSize(m_parent->getSize().x, m_size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Transform oldTransform = states.transform;
        states.transform.translate(getPosition());

        // Draw the background
        sf::RectangleShape background(getSize());
        background.setFillColor(m_backgroundColor);
        target.draw(background, states);

        if (m_menus.empty())
            return;

        // Draw the menus
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // Is the menu open?
            if (m_visibleMenu == static_cast<int>(i))
            {
                states.transform.translate(0, getSize().y);

                // Find out what the width of the menu should be
                float menuWidth = m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    menuWidth = TGUI_MAXIMUM(menuWidth, m_menus[i].menuItems[j].getSize().x + (3 * m_distanceToSide));

                // Draw the background of the menu
                background = sf::RectangleShape({menuWidth, getSize().y * m_menus[i].menuItems.size()});
                background.setFillColor(m_backgroundColor);
                target.draw(background, states);

                // If there is a selected menu item then draw its background
                if (m_menus[i].selectedMenuItem != -1)
                {
                    states.transform.translate(0, m_menus[i].selectedMenuItem * getSize().y);
                    background = sf::RectangleShape({menuWidth, getSize().y});
                    background.setFillColor(m_selectedBackgroundColor);
                    target.draw(background, states);
                    states.transform.translate(0, m_menus[i].selectedMenuItem * -getSize().y);
                }

                states.transform.translate(m_menus[i].text.getSize().x + 2*m_distanceToSide, -getSize().y);
            }
            else // The menu isn't open
                states.transform.translate(m_menus[i].text.getSize().x + m_distanceToSide, 0);
        }

        states.transform = oldTransform;

        // Draw the menus
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            target.draw(m_menus[i].text, states);

            // Draw the menu items when the menu is open
            if (m_visibleMenu == static_cast<int>(i))
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    target.draw(m_menus[i].menuItems[j], states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
