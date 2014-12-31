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


#include "../../../include/TGUI/FormBuilder/MenuBar.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MenuBar::MenuBar(tgui::Gui& gui) :
    m_Menu(gui, "MenuBar")
{
    m_Menu->setBackgroundColor(sf::Color(245, 245, 245));
    m_Menu->setSize(m_Menu->getSize().x, 20);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MenuBar::addMenuItem(const std::string& menu, const std::string& item)
{
    // Find the menu
    for (auto menuIt = m_Menus.begin(); menuIt != m_Menus.end(); ++menuIt)
    {
        if (menu == menuIt->text)
        {
            // Find the item
            for (auto itemIt = menuIt->menuItems.cbegin(); itemIt != menuIt->menuItems.cend(); ++itemIt)
            {
                if (item == *itemIt)
                {
                    // The item already existed
                    return;
                }
            }

            // The item didn't exit yet, place it on the correct spot
            for (auto menuOrderIt = m_MenuOrder.cbegin(); menuOrderIt != m_MenuOrder.cend(); ++menuOrderIt)
            {
                if (menuOrderIt->text == menu)
                {
                    auto menuItemIt = menuIt->menuItems.begin();
                    for (auto menuItemOrderIt = menuOrderIt->menuItems.cbegin(); menuItemOrderIt != menuOrderIt->menuItems.cend(); ++menuItemOrderIt)
                    {
                        if (*menuItemOrderIt == item)
                        {
                            menuIt->menuItems.insert(menuItemIt, item);

                            // Correct the menu bar widget
                            {
                                m_Menu->removeMenu(menu);
                                m_Menu->addMenu(menu);

                                for (auto itemIt = menuIt->menuItems.begin(); itemIt != menuIt->menuItems.end(); ++itemIt)
                                    m_Menu->addMenuItem(menuIt->text, *itemIt);
                            }

                            return;
                        }

                        if ((menuItemIt != menuIt->menuItems.end()) && (*menuItemIt == *menuItemOrderIt))
                            ++menuItemIt;
                    }
                }
            }


            menuIt->menuItems.push_back(item);
            m_Menu->addMenuItem(menu, item);
            return;
        }
    }

    // The menu didn't exits yet, find the place where it should be
    auto menuIt = m_Menus.begin();
    for (auto orderIt = m_MenuOrder.cbegin(); orderIt != m_MenuOrder.cend(); ++orderIt)
    {
        if (orderIt->text == menu)
        {
            // Add the menu to the correct place
            m_Menus.insert(menuIt, Menu(menu, item));

            // Correct the menu bar widget
            m_Menu->removeAllMenus();
            for (menuIt = m_Menus.begin(); menuIt != m_Menus.end(); ++menuIt)
            {
                m_Menu->addMenu(menuIt->text);

                for (auto itemIt = menuIt->menuItems.begin(); itemIt != menuIt->menuItems.end(); ++itemIt)
                    m_Menu->addMenuItem(menuIt->text, *itemIt);
            }

            return;
        }

        if ((menuIt != m_Menus.end()) && (menuIt->text == orderIt->text))
            ++menuIt;
    }

    // The menu isn't named in the order, so just add it on the last place
    m_Menus.push_back(Menu(menu, item));
    m_Menu->addMenu(menu);
    m_Menu->addMenuItem(menu, item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MenuBar::removeMenuItem(const std::string& menu, const std::string& item)
{
    // Find the menu
    for (auto menuIt = m_Menus.begin(); menuIt != m_Menus.end(); ++menuIt)
    {
        if (menu == menuIt->text)
        {
            // Find the item
            for (auto itemIt = menuIt->menuItems.begin(); itemIt != menuIt->menuItems.end(); ++itemIt)
            {
                if (item == *itemIt)
                {
                    // Remove the item
                    menuIt->menuItems.erase(itemIt);
                    m_Menu->removeMenuItem(menu, item);

                    // If the menu is empty, also remove the menu
                    if (menuIt->menuItems.empty())
                    {
                        m_Menus.erase(menuIt);
                        m_Menu->removeMenu(menu);
                    }

                    return;
                }
            }

            // The item didn't exit
            return;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MenuBar::addMenuOrder(const std::string& menuName)
{
    m_MenuOrder.push_back(Menu(menuName));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MenuBar::addMenuItemOrder(const std::string& menuName, const std::string& itemName)
{
    for (auto orderIt = m_MenuOrder.begin(); orderIt != m_MenuOrder.end(); ++orderIt)
    {
        if (orderIt->text == menuName)
        {
            orderIt->menuItems.push_back(itemName);
            return;
        }
    }

    m_MenuOrder.push_back(Menu(menuName, itemName));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
