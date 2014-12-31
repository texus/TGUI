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


#ifndef TGUI_FORM_BUILDER_MENU_BAR_HPP
#define TGUI_FORM_BUILDER_MENU_BAR_HPP

#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MenuBar
{
public:

    MenuBar(tgui::Gui& gui);

    void addMenuItem(const std::string& menu, const std::string& item);
    void removeMenuItem(const std::string& menu, const std::string& item);

    void addMenuOrder(const std::string& menuName);
    void addMenuItemOrder(const std::string& menuName, const std::string& itemName);


private:

    struct Menu
    {
        Menu(const std::string& menuName) : text(menuName) {}
        Menu(const std::string& menuName, const std::string& itemName) : text(menuName), menuItems(1, itemName) {}

        std::string text;
        std::vector<std::string> menuItems;
    };

    tgui::MenuBar::Ptr m_Menu;

    std::vector<Menu> m_Menus;
    std::vector<Menu> m_MenuOrder;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_FORM_BUILDER_FORM_HPP
