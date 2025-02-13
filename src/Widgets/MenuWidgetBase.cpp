/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Widgets/MenuWidgetBase.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBase::MenuWidgetBase(const char* typeName, bool initRenderer) :
        Widget{typeName, initRenderer},
        m_openMenuPlaceholder(std::make_shared<OpenMenuPlaceholder>(this)),
        m_distanceToSideCached(std::round(Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f))
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBase::MenuWidgetBase(const MenuWidgetBase& other) :
        Widget                         {other},
        m_openMenuPlaceholder          {std::make_shared<OpenMenuPlaceholder>(this)},
        m_minimumMenuWidth             {other.m_minimumMenuWidth},
        m_spriteItemBackground         {other.m_spriteItemBackground},
        m_spriteSelectedItemBackground {other.m_spriteSelectedItemBackground},
        m_backgroundColorCached        {other.m_backgroundColorCached},
        m_selectedBackgroundColorCached{other.m_selectedBackgroundColorCached},
        m_textColorCached              {other.m_textColorCached},
        m_selectedTextColorCached      {other.m_selectedTextColorCached},
        m_textColorDisabledCached      {other.m_textColorDisabledCached},
        m_distanceToSideCached         {other.m_distanceToSideCached},
        m_invertedMenuDirection        {other.m_invertedMenuDirection}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBase::MenuWidgetBase(MenuWidgetBase&& other) noexcept :
        Widget                         {std::move(other)},
        m_openMenuPlaceholder          {std::make_shared<OpenMenuPlaceholder>(this)},
        m_minimumMenuWidth             {std::move(other.m_minimumMenuWidth)},
        m_spriteItemBackground         {std::move(other.m_spriteItemBackground)},
        m_spriteSelectedItemBackground {std::move(other.m_spriteSelectedItemBackground)},
        m_backgroundColorCached        {std::move(other.m_backgroundColorCached)},
        m_selectedBackgroundColorCached{std::move(other.m_selectedBackgroundColorCached)},
        m_textColorCached              {std::move(other.m_textColorCached)},
        m_selectedTextColorCached      {std::move(other.m_selectedTextColorCached)},
        m_textColorDisabledCached      {std::move(other.m_textColorDisabledCached)},
        m_distanceToSideCached         {std::move(other.m_distanceToSideCached)},
        m_invertedMenuDirection        {std::move(other.m_invertedMenuDirection)}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBase& MenuWidgetBase::operator=(const MenuWidgetBase& other)
    {
        // Make sure it is not the same widget
        if (this != &other)
        {
            Widget::operator=(other);
            m_openMenuPlaceholder = other.m_openMenuPlaceholder;
            m_minimumMenuWidth = other.m_minimumMenuWidth;
            m_spriteItemBackground = other.m_spriteItemBackground;
            m_spriteSelectedItemBackground = other.m_spriteSelectedItemBackground;
            m_backgroundColorCached = other.m_backgroundColorCached;
            m_selectedBackgroundColorCached = other.m_selectedBackgroundColorCached;
            m_textColorCached = other.m_textColorCached;
            m_selectedTextColorCached = other.m_selectedTextColorCached;
            m_textColorDisabledCached = other.m_textColorDisabledCached;
            m_distanceToSideCached = other.m_distanceToSideCached;
            m_invertedMenuDirection = other.m_invertedMenuDirection;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBase& MenuWidgetBase::operator=(MenuWidgetBase&& other) noexcept
    {
        if (this != &other)
        {
            m_openMenuPlaceholder = std::move(other.m_openMenuPlaceholder);
            m_minimumMenuWidth = std::move(other.m_minimumMenuWidth);
            m_spriteItemBackground = std::move(other.m_spriteItemBackground);
            m_spriteSelectedItemBackground = std::move(other.m_spriteSelectedItemBackground);
            m_backgroundColorCached = std::move(other.m_backgroundColorCached);
            m_selectedBackgroundColorCached = std::move(other.m_selectedBackgroundColorCached);
            m_textColorCached = std::move(other.m_textColorCached);
            m_selectedTextColorCached = std::move(other.m_selectedTextColorCached);
            m_textColorDisabledCached = std::move(other.m_textColorDisabledCached);
            m_distanceToSideCached = std::move(other.m_distanceToSideCached);
            m_invertedMenuDirection = std::move(other.m_invertedMenuDirection);
            Widget::operator=(std::move(other));
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBaseRenderer* MenuWidgetBase::getSharedRenderer()
    {
        return aurora::downcast<MenuWidgetBaseRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuWidgetBaseRenderer* MenuWidgetBase::getSharedRenderer() const
    {
        return aurora::downcast<const MenuWidgetBaseRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBaseRenderer* MenuWidgetBase::getRenderer()
    {
        return aurora::downcast<MenuWidgetBaseRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MenuWidgetBase::getSignal(String signalName)
    {
        if (signalName == onMenuItemClick.getName())
            return onMenuItemClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::rendererChanged(const String& property)
    {
        if (property == U"TextureItemBackground")
        {
            m_spriteItemBackground.setTexture(getSharedRenderer()->getTextureItemBackground());
        }
        else if (property == U"TextureSelectedItemBackground")
        {
            m_spriteSelectedItemBackground.setTexture(getSharedRenderer()->getTextureSelectedItemBackground());
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"SelectedBackgroundColor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == U"DistanceToSide")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
        }
        else if (property == U"SeparatorColor")
        {
            m_separatorColorCached = getSharedRenderer()->getSeparatorColor();
        }
        else if (property == U"SeparatorThickness")
        {
            m_separatorThicknessCached = getSharedRenderer()->getSeparatorThickness();
        }
        else if (property == U"SeparatorVerticalPadding")
        {
            m_separatorVerticalPaddingCached = getSharedRenderer()->getSeparatorVerticalPadding();
        }
        else if (property == U"SeparatorSidePadding")
        {
            m_separatorSidePaddingCached = getSharedRenderer()->getSeparatorSidePadding();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::drawOpenMenu(BackendRenderTarget& target, RenderStates states, const Menu& menu, Vector2f menuOffset) const
    {
        // Move the menu to the left if it otherwise falls off the screen
        bool openSubMenuToRight = true;
        const float menuWidth = calculateMenuWidth(menu);
        if (getParent() && (getPosition().x + menuOffset.x + menuWidth > getParent()->getInnerSize().x))
        {
            menuOffset.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
            openSubMenuToRight = false;
        }

        states.transform.translate(menuOffset);
        drawMenu(target, states, menu, menuWidth, getPosition().x + menuOffset.x, openSubMenuToRight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::createMenu(std::vector<Menu>& menus, const String& text)
    {
        Menu newMenu;
        newMenu.text.setFont(m_fontCached);
        newMenu.text.setColor(m_textColorCached);
        newMenu.text.setOpacity(m_opacityCached);
        newMenu.text.setCharacterSize(m_textSizeCached);
        newMenu.text.setString(text);
        menus.push_back(std::move(newMenu));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBase::Menu* MenuWidgetBase::findMenuItemParent(const std::vector<String>& hierarchy, unsigned int parentIndex, std::vector<Menu>& menus, bool createParents)
    {
        TGUI_ASSERT(hierarchy.size() >= 2, "Hierarchy needs at least 2 elements in MenuWidgetBase::findMenuItemParent!");

        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 2 == hierarchy.size())
                return &menu;
            else
                return findMenuItemParent(hierarchy, parentIndex + 1, menu.menuItems, createParents);
        }

        if (createParents)
        {
            createMenu(menus, hierarchy[parentIndex]);
            if (parentIndex + 2 == hierarchy.size())
                return &menus.back();
            else
                return findMenuItemParent(hierarchy, parentIndex + 1, menus.back().menuItems, createParents);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuWidgetBase::Menu* MenuWidgetBase::findMenuItemParent(const std::vector<String>& hierarchy, unsigned int parentIndex, const std::vector<Menu>& menus) const
    {
        TGUI_ASSERT(hierarchy.size() >= 2, "Hierarchy needs at least 2 elements in MenuWidgetBase::findMenuItemParent!");

        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 2 == hierarchy.size())
                return &menu;
            else
                return findMenuItemParent(hierarchy, parentIndex + 1, menu.menuItems);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuWidgetBase::Menu* MenuWidgetBase::findMenuItem(const std::vector<String>& hierarchy, std::vector<Menu>& menus)
    {
        if (hierarchy.empty())
            return nullptr;

        std::vector<Menu>* menuItems;
        if (hierarchy.size() == 1)
            menuItems = &menus;
        else
        {
            auto* menu = findMenuItemParent(hierarchy, 0, menus, false);
            if (!menu)
                return nullptr;

            menuItems = &menu->menuItems;
        }

        for (auto& menuItem : *menuItems)
        {
            if (menuItem.text.getString() != hierarchy.back())
                continue;

            return &menuItem;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuWidgetBase::Menu* MenuWidgetBase::findMenuItem(const std::vector<String>& hierarchy, const std::vector<Menu>& menus) const
    {
        if (hierarchy.empty())
            return nullptr;

        const std::vector<Menu>* menuItems;
        if (hierarchy.size() == 1)
            menuItems = &menus;
        else
        {
            const auto* menu = findMenuItemParent(hierarchy, 0, menus);
            if (!menu)
                return nullptr;

            menuItems = &menu->menuItems;
        }

        for (auto& menuItem : *menuItems)
        {
            if (menuItem.text.getString() != hierarchy.back())
                continue;

            return &menuItem;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::loadMenus(const std::unique_ptr<DataIO::Node>& node, std::vector<Menu>& menus)
    {
        for (const auto& childNode : node->children)
        {
            if (childNode->name != U"Menu")
                continue;

            if (!childNode->propertyValuePairs[U"Text"])
                throw Exception{U"Failed to parse 'Menu' property, expected a nested 'Text' propery"};

            const String menuText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs[U"Text"]->value).getString();
            createMenu(menus, menuText);

            if (childNode->propertyValuePairs[U"Enabled"])
                menus.back().enabled = Deserializer::deserialize(ObjectConverter::Type::Bool, childNode->propertyValuePairs[U"Enabled"]->value).getBool();

            // Recursively handle the menu nodes
            if (!childNode->children.empty())
                loadMenus(childNode, menus.back().menuItems);

            // Menu items can also be stored in an string array in the 'Items' property instead of as a nested Menu section
            if (childNode->propertyValuePairs[U"Items"])
            {
                if (!childNode->propertyValuePairs[U"Items"]->listNode)
                    throw Exception{U"Failed to parse 'Items' property inside 'Menu' property, expected a list as value"};

                for (std::size_t i = 0; i < childNode->propertyValuePairs[U"Items"]->valueList.size(); ++i)
                {
                    const String menuItemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs[U"Items"]->valueList[i]).getString();
                    createMenu(menus.back().menuItems, menuItemText);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::closeSubMenus(std::vector<Menu>& menus, int& selectedMenu)
    {
        if (selectedMenu < 0)
            return;

        const auto selectedMenuIdx = static_cast<std::size_t>(selectedMenu);
        closeSubMenus(menus[selectedMenuIdx].menuItems, menus[selectedMenuIdx].selectedMenuItem);

        updateMenuTextColor(menus[selectedMenuIdx], false);
        selectedMenu = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::deselectDeepestItem(Menu* menu)
    {
        while (menu->selectedMenuItem >= 0)
        {
            auto& menuItem = menu->menuItems[static_cast<std::size_t>(menu->selectedMenuItem)];
            if (menuItem.menuItems.empty())
            {
                closeSubMenus(menu->menuItems, menu->selectedMenuItem);
                break;
            }

            menu = &menuItem;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::updateMenuTextColor(Menu& menu, bool selected)
    {
        if ((!m_enabled || !menu.enabled) && m_textColorDisabledCached.isSet())
            menu.text.setColor(m_textColorDisabledCached);
        else if (selected && m_selectedTextColorCached.isSet())
            menu.text.setColor(m_selectedTextColorCached);
        else
            menu.text.setColor(m_textColorCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::updateTextColors(std::vector<Menu>& menus, int selectedMenu)
    {
        for (std::size_t i = 0; i < menus.size(); ++i)
        {
            updateMenuTextColor(menus[i], (selectedMenu == static_cast<int>(i)));
            updateTextColors(menus[i].menuItems, menus[i].selectedMenuItem);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::updateTextOpacity(std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            menu.text.setOpacity(m_opacityCached);
            updateTextOpacity(menu.menuItems);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::updateTextFont(std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            menu.text.setFont(m_fontCached);
            updateTextFont(menu.menuItems);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuWidgetBase::calculateMenuWidth(const Menu& menu) const
    {
        float maxWidth = m_minimumMenuWidth;
        const float arrowSpace = (getDefaultMenuItemHeight() / 4.f) + m_distanceToSideCached;
        for (const auto& item : menu.menuItems)
        {
            float width = item.text.getSize().x + (2.f * m_distanceToSideCached);

            // Reserve space for an arrow if there are submenus
            if (!item.menuItems.empty())
                width += arrowSpace;

            if (width > maxWidth)
                maxWidth = width;
        }

        return maxWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuWidgetBase::getMenuItemHeight(const Menu& menuItem) const
    {
        if (isSeparator(menuItem))
            return m_separatorThicknessCached + 2*m_separatorVerticalPaddingCached;
        else
            return getDefaultMenuItemHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuWidgetBase::calculateOpenMenuHeight(const std::vector<Menu>& menuItems) const
    {
        float height = 0;
        for (const auto& item : menuItems)
            height += getMenuItemHeight(item);

        return height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f MenuWidgetBase::calculateSubmenuOffset(const Menu& menu, float globalLeftPos, float menuWidth, float subMenuWidth, bool& openSubMenuToRight) const
    {
        TGUI_ASSERT(menu.selectedMenuItem >= 0, "MenuWidgetBase::calculateSubmenuOffset can only be called when the menu has an open submenu");
        const auto selectedMenuItemIdx = static_cast<std::size_t>(menu.selectedMenuItem);

        float leftOffset;
        if (openSubMenuToRight)
        {
            leftOffset = menuWidth;
            if (getParent() && (globalLeftPos + leftOffset + subMenuWidth > getParent()->getInnerSize().x))
            {
                if (globalLeftPos + leftOffset + subMenuWidth - getParent()->getInnerSize().x < globalLeftPos)
                {
                    leftOffset = -subMenuWidth;
                    openSubMenuToRight = false;
                }
            }
        }
        else // Submenu opens to the left side
        {
            leftOffset = -subMenuWidth;
            if (getParent() && (globalLeftPos < subMenuWidth))
            {
                if (getParent()->getInnerSize().x - menuWidth - globalLeftPos > globalLeftPos)
                {
                    leftOffset = menuWidth;
                    openSubMenuToRight = true;
                }
            }
        }

        float topOffset = 0;
        for (std::size_t i = 0; i < selectedMenuItemIdx; ++i)
            topOffset += getMenuItemHeight(menu.menuItems[i]);

        if (m_invertedMenuDirection)
            topOffset -= calculateOpenMenuHeight(menu.menuItems[selectedMenuItemIdx].menuItems) - getDefaultMenuItemHeight();

        return {leftOffset, topOffset};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuWidgetBase::isSeparator(const Menu& menuItem)
    {
        return menuItem.text.getString() == U"-";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuWidgetBase::removeMenuImpl(const std::vector<String>& hierarchy, bool removeParentsWhenEmpty, unsigned int parentIndex, std::vector<Menu>& menus)
    {
        TGUI_ASSERT(!hierarchy.empty(), "Hierarchy can't be empty in MenuWidgetBase::removeMenuImpl!");

        for (auto it = menus.begin(); it != menus.end(); ++it)
        {
            if (it->text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 1 == hierarchy.size())
            {
                menus.erase(it);
                return true;
            }
            else
            {
                // Return false if some menu in the hierarchy couldn't be found
                if (!removeMenuImpl(hierarchy, removeParentsWhenEmpty, parentIndex + 1, it->menuItems))
                    return false;

                // If parents don't have to be removed as well then we are done
                if (!removeParentsWhenEmpty)
                    return true;

                // Also delete the parent if empty
                if (it->menuItems.empty())
                    menus.erase(it);

                return true;
            }
        }

        // The hierarchy doesn't exist
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuWidgetBase::removeSubMenusImpl(const std::vector<String>& hierarchy, unsigned int parentIndex, std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 1 == hierarchy.size())
            {
                menu.menuItems.clear();
                return true;
            }
            else
                return removeSubMenusImpl(hierarchy, parentIndex + 1, menu.menuItems);
        }

        // The hierarchy doesn't exist
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::setTextSizeImpl(std::vector<Menu>& menus, unsigned int textSize)
    {
        for (auto& menu : menus)
        {
            menu.text.setCharacterSize(textSize);
            if (!menu.menuItems.empty())
                setTextSizeImpl(menu.menuItems, textSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<MenuWidgetBase::GetMenusElement> MenuWidgetBase::getMenusImpl(const std::vector<Menu>& menus)
    {
        std::vector<GetMenusElement> menuElements;

        for (const auto& menu : menus)
        {
            TGUI_EMPLACE_BACK(element, menuElements)
            element.text = menu.text.getString();
            element.enabled = menu.enabled;
            if (!menu.menuItems.empty())
                element.menuItems = getMenusImpl(menu.menuItems);
        }

        return menuElements;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::saveMenus(const std::unique_ptr<DataIO::Node>& parentNode, const std::vector<Menu>& menus)
    {
        for (const auto& menu : menus)
        {
            auto menuNode = std::make_unique<DataIO::Node>();
            menuNode->name = "Menu";

            menuNode->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(menu.text.getString()));
            if (!menu.enabled)
                menuNode->propertyValuePairs[U"Enabled"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(menu.enabled));

            if (!menu.menuItems.empty())
            {
                // Save as nested 'Menu' sections only when needed, use the more compact string list when just storing the menu items
                const bool recursionNeeded = std::any_of(menu.menuItems.begin(), menu.menuItems.end(),
                    [](const Menu& menuItem){ return !menuItem.enabled || !menuItem.menuItems.empty(); });
                if (recursionNeeded)
                    saveMenus(menuNode, menu.menuItems);
                else
                {
                    String itemList = "[" + Serializer::serialize(menu.menuItems[0].text.getString());
                    for (std::size_t i = 1; i < menu.menuItems.size(); ++i)
                        itemList += ", " + Serializer::serialize(menu.menuItems[i].text.getString());
                    itemList += "]";

                    menuNode->propertyValuePairs[U"Items"] = std::make_unique<DataIO::ValueNode>(itemList);
                }
            }

            parentNode->children.push_back(std::move(menuNode));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuWidgetBase::isMouseOnTopOfMenu(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, const Menu& menu, float menuWidth) const
    {
        // Check if the mouse is on top of the menu
        if (FloatRect{menuPos.x, menuPos.y, menuWidth, calculateOpenMenuHeight(menu.menuItems)}.contains(mousePos))
            return true;

        // Check if the mouse is on one of the submenus
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[static_cast<std::size_t>(menu.selectedMenuItem)].menuItems.empty())
        {
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[static_cast<std::size_t>(menu.selectedMenuItem)]);
            const Vector2f offset = calculateSubmenuOffset(menu, menuPos.x, menuWidth, subMenuWidth, openSubMenuToRight);
            if (isMouseOnTopOfMenu(menuPos + offset, mousePos, openSubMenuToRight, menu.menuItems[static_cast<std::size_t>(menu.selectedMenuItem)], subMenuWidth))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuWidgetBase::isMouseOnOpenMenu(Vector2f pos, const Menu& menu, Vector2f menuOffset) const
    {
        // The menu is moved to the left if it otherwise falls off the screen
        bool openSubMenuToRight = true;
        const float menuWidth = calculateMenuWidth(menu);
        if (getParent() && (menuOffset.x + menuWidth > getParent()->getInnerSize().x))
        {
            menuOffset.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
            openSubMenuToRight = false;
        }

        return isMouseOnTopOfMenu(menuOffset, pos, openSubMenuToRight, menu, menuWidth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::leftMouseReleasedOnMenu(const Menu* menu)
    {
        bool emit = false;
        std::vector<String> hierarchy;
        hierarchy.push_back(menu->text.getString());
        while (menu->selectedMenuItem >= 0)
        {
            menu = &menu->menuItems[static_cast<std::size_t>(menu->selectedMenuItem)];
            hierarchy.push_back(menu->text.getString());
            if (menu->menuItems.empty())
            {
                emit = true;
                break;
            }
        }

        // Close the menu if the mouse went down on a leaf menu item.
        // If the item below the mouse is disabled or we clicked on an item that has a submenu then menuItems won't be empty.
        if (menu->menuItems.empty())
            closeMenu();

        if (emit)
            emitMenuItemClick(hierarchy);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::mouseMovedOnMenu(Vector2f pos, Menu& menu, Vector2f menuOffset)
    {
        // The menu is moved to the left if it otherwise falls off the screen
        bool openSubMenuToRight = true;
        const float menuWidth = calculateMenuWidth(menu);
        if (getParent() && (menuOffset.x + menuWidth > getParent()->getInnerSize().x))
        {
            menuOffset.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
            openSubMenuToRight = false;
        }

        Menu* menuBelowMouse;
        std::size_t menuItemIndexBelowMouse;
        if (findMenuItemBelowMouse(menuOffset, pos, openSubMenuToRight, menu, menuWidth, &menuBelowMouse, &menuItemIndexBelowMouse))
        {
            // Check if the mouse is on a different item than before
            if (static_cast<int>(menuItemIndexBelowMouse) != menuBelowMouse->selectedMenuItem)
            {
                // If another of the menu items is selected then unselect it
                closeSubMenus(menuBelowMouse->menuItems, menuBelowMouse->selectedMenuItem);

                // Mark the item below the mouse as selected
                auto& menuItem = menuBelowMouse->menuItems[menuItemIndexBelowMouse];
                if (menuItem.enabled && !isSeparator(menuItem))
                {
                    updateMenuTextColor(menuBelowMouse->menuItems[menuItemIndexBelowMouse], true);
                    menuBelowMouse->selectedMenuItem = static_cast<int>(menuItemIndexBelowMouse);
                }
            }
            else // We already selected this item
            {
                // If the selected item has a submenu then unselect its item
                closeSubMenus(menuBelowMouse->menuItems[menuItemIndexBelowMouse].menuItems, menuBelowMouse->menuItems[menuItemIndexBelowMouse].selectedMenuItem);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuWidgetBase::findMenuItemBelowMouse(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, Menu& menu, float menuWidth, Menu** resultMenu, std::size_t* resultSelectedMenuItem)
    {
        // Loop over the open submenus and make sure to handle them first as menus can overlap
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[static_cast<std::size_t>(menu.selectedMenuItem)].menuItems.empty())
        {
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[static_cast<std::size_t>(menu.selectedMenuItem)]);
            const Vector2f offset = calculateSubmenuOffset(menu, menuPos.x, menuWidth, subMenuWidth, openSubMenuToRight);
            if (findMenuItemBelowMouse(menuPos + offset, mousePos, openSubMenuToRight, menu.menuItems[static_cast<std::size_t>(menu.selectedMenuItem)], subMenuWidth, resultMenu, resultSelectedMenuItem))
                return true;
        }

        // Check if the mouse is on top of the menu
        if (!menu.menuItems.empty() && FloatRect{menuPos.x, menuPos.y, menuWidth, calculateOpenMenuHeight(menu.menuItems)}.contains(mousePos))
        {
            std::size_t selectedItem = menu.menuItems.size() - 1;
            float topPos = menuPos.y;
            for (std::size_t i = 0; i < menu.menuItems.size(); ++i)
            {
                topPos += getMenuItemHeight(menu.menuItems[i]);
                if (topPos > mousePos.y)
                {
                    selectedItem = i;
                    break;
                }
            }

            *resultMenu = &menu;
            *resultSelectedMenuItem = selectedItem;
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuWidgetBase::drawMenu(BackendRenderTarget& target, RenderStates states, const Menu& menu, float menuWidth, float globalLeftPos, bool openSubMenuToRight) const
    {
        if (menu.menuItems.empty())
            return;

        Transform oldTransform = states.transform;

        // Draw the backgrounds
        Sprite backgroundSprite = m_spriteItemBackground;
        if ((menu.selectedMenuItem < 0) && !backgroundSprite.isSet() && !m_selectedBackgroundColorCached.isSet())
        {
            target.drawFilledRect(states, {menuWidth, calculateOpenMenuHeight(menu.menuItems)}, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
        }
        else // We can't draw the entire menu with a singe draw call
        {
            for (std::size_t j = 0; j < menu.menuItems.size(); ++j)
            {
                const float menuItemHeight = getMenuItemHeight(menu.menuItems[j]);
                const bool isMenuItemSelected = (menu.selectedMenuItem == static_cast<int>(j));
                if (backgroundSprite.isSet())
                {
                    if (isMenuItemSelected && m_spriteSelectedItemBackground.isSet())
                    {
                        Sprite selectedBackgroundSprite = m_spriteSelectedItemBackground;
                        selectedBackgroundSprite.setSize({menuWidth, menuItemHeight});
                        target.drawSprite(states, selectedBackgroundSprite);
                    }
                    else // Not selected or no different texture for selected menu
                    {
                        backgroundSprite.setSize({menuWidth, menuItemHeight});
                        target.drawSprite(states, backgroundSprite);
                    }
                }
                else // No textures where loaded
                {
                    if (isMenuItemSelected && m_selectedBackgroundColorCached.isSet())
                        target.drawFilledRect(states, {menuWidth, menuItemHeight}, Color::applyOpacity(m_selectedBackgroundColorCached, m_opacityCached));
                    else
                        target.drawFilledRect(states, {menuWidth, menuItemHeight}, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
                }

                states.transform.translate({0, menuItemHeight});
            }

            states.transform = oldTransform;
        }

        // Draw the texts (and arrows when there are submenus)
        bool menuContainsSeparators = false;
        const float itemHeight = getDefaultMenuItemHeight();
        states.transform.translate({m_distanceToSideCached, (itemHeight - menu.menuItems[0].text.getSize().y) / 2.f});
        for (std::size_t j = 0; j < menu.menuItems.size(); ++j)
        {
            if (isSeparator(menu.menuItems[j]))
            {
                menuContainsSeparators = true;
                states.transform.translate({0, getMenuItemHeight(menu.menuItems[j])});
                continue;
            }

            target.drawText(states, menu.menuItems[j].text);

            // Draw an arrow next to the text if there is a submenu
            if (!menu.menuItems[j].menuItems.empty())
            {
                Transform textTransform = states.transform;
                const float arrowHeight = itemHeight / 2.f;
                const float arrowWidth = arrowHeight / 2.f;
                states.transform.translate({menuWidth - 2*m_distanceToSideCached - arrowWidth, // 2x m_distanceToSideCached because we already translated once
                                            (menu.menuItems[j].text.getSize().y - arrowHeight) / 2.f});

                Vertex::Color arrowVertexColor;
                if ((!m_enabled || !menu.menuItems[j].enabled) && m_textColorDisabledCached.isSet())
                    arrowVertexColor = Vertex::Color(Color::applyOpacity(m_textColorDisabledCached, m_opacityCached));
                else if ((menu.selectedMenuItem == static_cast<int>(j)) && m_selectedTextColorCached.isSet())
                    arrowVertexColor = Vertex::Color(Color::applyOpacity(m_selectedTextColorCached, m_opacityCached));
                else
                    arrowVertexColor = Vertex::Color(Color::applyOpacity(m_textColorCached, m_opacityCached));

                target.drawTriangle(states,
                    {{0, 0}, arrowVertexColor},
                    {{arrowWidth, arrowHeight / 2.f}, arrowVertexColor},
                    {{0, arrowHeight}, arrowVertexColor}
                );

                states.transform = textTransform;
            }

            states.transform.translate({0, itemHeight});
        }

        if (menuContainsSeparators)
        {
            states.transform = oldTransform;
            states.transform.translate({m_separatorSidePaddingCached, m_separatorVerticalPaddingCached});
            for (const auto& menuItem : menu.menuItems)
            {
                if (isSeparator(menuItem))
                    target.drawFilledRect(states, {menuWidth - 2*m_separatorSidePaddingCached, m_separatorThicknessCached}, m_separatorColorCached);

                states.transform.translate({0, getMenuItemHeight(menuItem)});
            }
        }

        // Draw the submenu if one is opened
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[static_cast<std::size_t>(menu.selectedMenuItem)].menuItems.empty())
        {
            const auto selectedMenuItemIdx = static_cast<std::size_t>(menu.selectedMenuItem);
            states.transform = oldTransform;

            // If there isn't enough room on the right side then open the menu to the left if it has more room
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[selectedMenuItemIdx]);

            float leftOffset;
            if (openSubMenuToRight)
            {
                leftOffset = menuWidth;
                if (getParent() && (globalLeftPos + leftOffset + subMenuWidth > getParent()->getInnerSize().x))
                {
                    if (globalLeftPos + leftOffset + subMenuWidth - getParent()->getInnerSize().x < globalLeftPos)
                    {
                        leftOffset = -subMenuWidth;
                        openSubMenuToRight = false;
                    }
                }
            }
            else // Submenu opens to the left side
            {
                leftOffset = -subMenuWidth;
                if (getParent() && (globalLeftPos < subMenuWidth))
                {
                    if (getParent()->getInnerSize().x - menuWidth - globalLeftPos > globalLeftPos)
                    {
                        leftOffset = menuWidth;
                        openSubMenuToRight = true;
                    }
                }
            }

            float topOffset = 0;
            for (std::size_t i = 0; i < selectedMenuItemIdx; ++i)
                topOffset += getMenuItemHeight(menu.menuItems[i]);

            if (m_invertedMenuDirection)
                topOffset -= calculateOpenMenuHeight(menu.menuItems[selectedMenuItemIdx].menuItems) - itemHeight;

            states.transform.translate({leftOffset, topOffset});
            drawMenu(target, states, menu.menuItems[selectedMenuItemIdx], subMenuWidth, globalLeftPos + leftOffset, openSubMenuToRight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    OpenMenuPlaceholder::OpenMenuPlaceholder(MenuWidgetBase* menuWidget) :
        Widget{"OpenMenuPlaceholder", true},
        m_menuWidget{menuWidget}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f OpenMenuPlaceholder::getFullSize() const
    {
        if (m_parent)
            return m_parent->getInnerSize() + getWidgetOffset();
        else
            return {0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f OpenMenuPlaceholder::getWidgetOffset() const
    {
        if (m_parentGui)
            return m_parentGui->getView().getPosition() - getPosition();
        else
            return -getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool OpenMenuPlaceholder::isMouseOnWidget(Vector2f) const
    {
        return true; // Absorb all mouse events until the menu is closed
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void OpenMenuPlaceholder::leftMouseButtonNoLongerDown()
    {
        if (m_mouseHover)
            m_menuWidget->leftMouseReleasedOnMenu();
        else
            m_menuWidget->closeMenu();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void OpenMenuPlaceholder::rightMousePressed(Vector2f pos)
    {
        if (!m_menuWidget->isMouseOnOpenMenu(pos - getPosition()))
            m_menuWidget->closeMenu();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void OpenMenuPlaceholder::mouseMoved(Vector2f pos)
    {
        bool mouseOnMenuWidget = false;
        if (m_menuWidget->isMouseOnOpenMenu(pos - getPosition()))
        {
            m_mouseHover = true;
            m_menuWidget->mouseMovedOnMenu(pos - getPosition());
        }
        else
        {
            if (m_mouseHover)
            {
                // Deselect the selected item of the deepest submenu
                m_menuWidget->deselectDeepestItem();
                m_mouseHover = false;
            }

            if (m_menuWidget->getParent())
            {
                const Vector2f menuWidgetMousePos = pos - m_menuWidget->getAbsolutePosition() + m_menuWidget->getPosition();
                if (m_menuWidget->isMouseOnWidget(menuWidgetMousePos))
                {
                    mouseOnMenuWidget = true;
                    m_mouseWasOnMenuWidget = true;
                    m_menuWidget->mouseMoved(menuWidgetMousePos);
                }
            }
        }

        if (!mouseOnMenuWidget && m_mouseWasOnMenuWidget)
        {
            m_mouseWasOnMenuWidget = false;
            m_menuWidget->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void OpenMenuPlaceholder::keyPressed(const Event::KeyEvent& event)
    {
        if (event.code == Event::KeyboardKey::Escape)
            m_menuWidget->closeMenu();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void OpenMenuPlaceholder::draw(BackendRenderTarget& target, RenderStates states) const
    {
        m_menuWidget->drawOpenMenu(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr OpenMenuPlaceholder::clone() const
    {
        // This function should never be called
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
