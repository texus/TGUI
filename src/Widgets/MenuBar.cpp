/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Container.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        bool isSeparator(const MenuBar::Menu& menuItem)
        {
            return menuItem.text.getString() == U"-";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool removeMenuImpl(const std::vector<String>& hierarchy, bool removeParentsWhenEmpty, unsigned int parentIndex, std::vector<MenuBar::Menu>& menus)
        {
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool removeSubMenusImpl(const std::vector<String>& hierarchy, unsigned int parentIndex, std::vector<MenuBar::Menu>& menus)
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void setTextSizeImpl(std::vector<MenuBar::Menu>& menus, unsigned int textSize)
        {
            for (auto& menu : menus)
            {
                menu.text.setCharacterSize(textSize);
                if (!menu.menuItems.empty())
                    setTextSizeImpl(menu.menuItems, textSize);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<MenuBar::GetMenusElement> getMenusImpl(const std::vector<MenuBar::Menu>& menus)
        {
            std::vector<MenuBar::GetMenusElement> menuElements;

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void saveMenus(const std::unique_ptr<DataIO::Node>& parentNode, const std::vector<MenuBar::Menu>& menus)
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
                        [](const MenuBar::Menu& menuItem){ return !menuItem.enabled || !menuItem.menuItems.empty(); });
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char MenuBar::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar(const char* typeName, bool initRenderer) :
        Widget{typeName, false},
        m_menuWidgetPlaceholder(std::make_shared<MenuBarMenuPlaceholder>(this)),
        m_distanceToSideCached(std::round(Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f))
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<MenuBarRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setTextSize(getGlobalTextSize());
        setMinimumSubMenuWidth((Text::getLineHeight(m_fontCached, m_textSizeCached) * 4) + (2 * m_distanceToSideCached));
        setSize({"100%", std::round(Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f)});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar(const MenuBar& other) :
        Widget                         {other},
        m_menus                        {other.m_menus},
        m_menuWidgetPlaceholder        {std::make_shared<MenuBarMenuPlaceholder>(this)},
        m_visibleMenu                  {other.m_visibleMenu},
        m_minimumSubMenuWidth          {other.m_minimumSubMenuWidth},
        m_invertedMenuDirection        {other.m_invertedMenuDirection},
        m_spriteBackground             {other.m_spriteBackground},
        m_spriteItemBackground         {other.m_spriteItemBackground},
        m_spriteSelectedItemBackground {other.m_spriteSelectedItemBackground},
        m_backgroundColorCached        {other.m_backgroundColorCached},
        m_selectedBackgroundColorCached{other.m_selectedBackgroundColorCached},
        m_textColorCached              {other.m_textColorCached},
        m_selectedTextColorCached      {other.m_selectedTextColorCached},
        m_textColorDisabledCached      {other.m_textColorDisabledCached},
        m_distanceToSideCached         {other.m_distanceToSideCached}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar(MenuBar&& other) noexcept :
        Widget                         {std::move(other)},
        m_menus                        {std::move(other.m_menus)},
        m_menuWidgetPlaceholder        {std::make_shared<MenuBarMenuPlaceholder>(this)},
        m_visibleMenu                  {std::move(other.m_visibleMenu)},
        m_minimumSubMenuWidth          {std::move(other.m_minimumSubMenuWidth)},
        m_invertedMenuDirection        {std::move(other.m_invertedMenuDirection)},
        m_spriteBackground             {std::move(other.m_spriteBackground)},
        m_spriteItemBackground         {std::move(other.m_spriteItemBackground)},
        m_spriteSelectedItemBackground {std::move(other.m_spriteSelectedItemBackground)},
        m_backgroundColorCached        {std::move(other.m_backgroundColorCached)},
        m_selectedBackgroundColorCached{std::move(other.m_selectedBackgroundColorCached)},
        m_textColorCached              {std::move(other.m_textColorCached)},
        m_selectedTextColorCached      {std::move(other.m_selectedTextColorCached)},
        m_textColorDisabledCached      {std::move(other.m_textColorDisabledCached)},
        m_distanceToSideCached         {std::move(other.m_distanceToSideCached)}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar& MenuBar::operator=(const MenuBar& other)
    {
        // Make sure it is not the same widget
        if (this != &other)
        {
            Widget::operator=(other);
            m_menus = other.m_menus;
            m_menuWidgetPlaceholder = other.m_menuWidgetPlaceholder;
            m_visibleMenu = other.m_visibleMenu;
            m_minimumSubMenuWidth = other.m_minimumSubMenuWidth;
            m_invertedMenuDirection = other.m_invertedMenuDirection;
            m_spriteBackground = other.m_spriteBackground;
            m_spriteItemBackground = other.m_spriteItemBackground;
            m_spriteSelectedItemBackground = other.m_spriteSelectedItemBackground;
            m_backgroundColorCached = other.m_backgroundColorCached;
            m_selectedBackgroundColorCached = other.m_selectedBackgroundColorCached;
            m_textColorCached = other.m_textColorCached;
            m_selectedTextColorCached = other.m_selectedTextColorCached;
            m_textColorDisabledCached = other.m_textColorDisabledCached;
            m_distanceToSideCached = other.m_distanceToSideCached;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar& MenuBar::operator=(MenuBar&& other) noexcept
    {
        if (this != &other)
        {
            m_menus = std::move(other.m_menus);
            m_menuWidgetPlaceholder = std::move(other.m_menuWidgetPlaceholder);
            m_visibleMenu = std::move(other.m_visibleMenu);
            m_minimumSubMenuWidth = std::move(other.m_minimumSubMenuWidth);
            m_invertedMenuDirection = std::move(other.m_invertedMenuDirection);
            m_spriteBackground = std::move(other.m_spriteBackground);
            m_spriteItemBackground = std::move(other.m_spriteItemBackground);
            m_spriteSelectedItemBackground = std::move(other.m_spriteSelectedItemBackground);
            m_backgroundColorCached = std::move(other.m_backgroundColorCached);
            m_selectedBackgroundColorCached = std::move(other.m_selectedBackgroundColorCached);
            m_textColorCached = std::move(other.m_textColorCached);
            m_selectedTextColorCached = std::move(other.m_selectedTextColorCached);
            m_textColorDisabledCached = std::move(other.m_textColorDisabledCached);
            m_distanceToSideCached = std::move(other.m_distanceToSideCached);
            Widget::operator=(std::move(other));
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::create()
    {
        return std::make_shared<MenuBar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::copy(const MenuBar::ConstPtr& menuBar)
    {
        if (menuBar)
            return std::static_pointer_cast<MenuBar>(menuBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarRenderer* MenuBar::getSharedRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBarRenderer* MenuBar::getSharedRenderer() const
    {
        return aurora::downcast<const MenuBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarRenderer* MenuBar::getRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteBackground.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setEnabled(bool enabled)
    {
        if (m_enabled == enabled)
            return;

        Widget::setEnabled(enabled);

        if (!enabled)
            closeMenu();

        updateTextColors(m_menus, m_visibleMenu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const String& text)
    {
        createMenu(m_menus, text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const String& text)
    {
        if (!m_menus.empty())
            return addMenuItem(m_menus.back().text.getString(), text);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const String& menu, const String& text)
    {
        return addMenuItem({menu, text}, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const std::vector<String>& hierarchy, bool createParents)
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menu = findMenu(hierarchy, 0, m_menus, createParents);
        if (!menu)
            return false;

        createMenu(menu->menuItems, hierarchy.back());
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::changeMenuItem(const std::vector<String>& hierarchy, const String& text)
    {
        if (hierarchy.empty())
            return false;

        auto* menu = findMenuItem(hierarchy);
        if (!menu)
            return false;

        menu->text.setString(text);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::removeAllMenus()
    {
        m_menus.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenu(const String& menu)
    {
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            if (m_menus[i].text.getString() != menu)
                continue;

            closeMenu();
            m_menus.erase(m_menus.begin() + static_cast<std::ptrdiff_t>(i));
            return true;
        }

        // could not find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const String& menu, const String& menuItem)
    {
        return removeMenuItem({menu, menuItem}, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const std::vector<String>& hierarchy, bool removeParentsWhenEmpty)
    {
        if (hierarchy.size() < 2)
            return false;

        return removeMenuImpl(hierarchy, removeParentsWhenEmpty, 0, m_menus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItems(const String& menu)
    {
        return removeSubMenuItems(std::vector<String>{menu});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeSubMenuItems(const std::vector<String>& hierarchy)
    {
        if (hierarchy.empty())
            return false;

        return removeSubMenusImpl(hierarchy, 0, m_menus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuEnabled(const String& menu, bool enabled)
    {
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            if (m_menus[i].text.getString() != menu)
                continue;

            if (!enabled && (m_visibleMenu == static_cast<int>(i)))
                closeMenu();

            m_menus[i].enabled = enabled;
            updateMenuTextColor(m_menus[i], (m_visibleMenu == static_cast<int>(i)));
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuEnabled(const String& menuText) const
    {
        for (auto& menu : m_menus)
        {
            if (menu.text.getString() == menuText)
                return menu.enabled;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuItemEnabled(const String& menuText, const String& menuItemText, bool enabled)
    {
        return setMenuItemEnabled({menuText, menuItemText}, enabled);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuItemEnabled(const std::vector<String>& hierarchy, bool enabled)
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menu = findMenu(hierarchy, 0, m_menus, false);
        if (!menu)
            return false;

        for (unsigned int j = 0; j < menu->menuItems.size(); ++j)
        {
            auto& menuItem = menu->menuItems[j];
            if (menuItem.text.getString() != hierarchy.back())
                continue;

            if (!enabled && (menu->selectedMenuItem == static_cast<int>(j)))
                menu->selectedMenuItem = -1;

            menuItem.enabled = enabled;
            updateMenuTextColor(menuItem, (menu->selectedMenuItem == static_cast<int>(j)));
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuItemEnabled(const String& menuText, const String& menuItemText) const
    {
        return getMenuItemEnabled({menuText, menuItemText});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuItemEnabled(const std::vector<String>& hierarchy) const
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menuItem = findMenuItem(hierarchy);
        if (!menuItem)
            return false;

        return menuItem->enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextSize()
    {
        setTextSizeImpl(m_menus, m_textSizeCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setMinimumSubMenuWidth(float minimumWidth)
    {
        m_minimumSubMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::getMinimumSubMenuWidth() const
    {
        return m_minimumSubMenuWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setInvertedMenuDirection(bool invertDirection)
    {
        m_invertedMenuDirection = invertDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getInvertedMenuDirection() const
    {
        return m_invertedMenuDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<MenuBar::GetMenusElement> MenuBar::getMenus() const
    {
        return getMenusImpl(m_menus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::openMenu(std::size_t menuIndex)
    {
        closeMenu();

        updateMenuTextColor(m_menus[menuIndex], true);
        m_visibleMenu = static_cast<int>(menuIndex);

        if (m_parent)
        {
            // Find the RootContainer that contains the menu bar
            Vector2f scale = getScale();
            Container* container = m_parent;
            while (container->getParent() != nullptr)
            {
                scale.x *= container->getScale().x;
                scale.y *= container->getScale().y;
                container = container->getParent();
            }

            m_menuWidgetPlaceholder->setPosition(getAbsolutePosition());
            m_menuWidgetPlaceholder->setScale(scale);
            container->add(m_menuWidgetPlaceholder, "#TGUI_INTERNAL$MenuBarMenuPlaceholder#");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeMenu()
    {
        if (m_visibleMenu < 0)
            return;

        closeSubMenus(m_menus, m_visibleMenu);

        if (m_menuWidgetPlaceholder->getParent())
            m_menuWidgetPlaceholder->getParent()->remove(m_menuWidgetPlaceholder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::isMouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);

        pos -= getPosition();

        // Loop through the menus to check if the mouse is on top of them
        float menuWidth = 0;
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
            if (pos.x >= menuWidth)
                continue;

            // Close the menu when it was already open
            if (m_visibleMenu == static_cast<int>(i))
                closeMenu();

            // If this menu can be opened then do so
            else if (m_menus[i].enabled && !m_menus[i].menuItems.empty())
                openMenu(i);

            break;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleased(Vector2f pos)
    {
        if (!m_mouseDown)
            return;

        pos -= getPosition();

        // Loop through the menus to check if the mouse is on top of them
        float menuWidth = 0;
        for (const auto& menu : m_menus)
        {
            menuWidth += menu.text.getSize().x + (2 * m_distanceToSideCached);
            if (pos.x >= menuWidth)
                continue;

            // If a menu is clicked that has no menu items then also emit a signal
            if (menu.menuItems.empty())
            {
                onMenuItemClick.emit(this, menu.text.getString(), std::vector<String>(1, menu.text.getString()));
                closeMenu();
            }

            break;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMoved(Vector2f pos)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Don't open a menu without having clicked first
        if (m_visibleMenu < 0)
            return;

        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);
        pos -= getPosition();

        // Loop through the menus to check if the mouse is on top of them
        bool handled = false;
        float menuWidth = 0;
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
            if (pos.x >= menuWidth)
                continue;

            // Check if the menu is already open
            if (visibleMenuIdx == i)
            {
                // If one of the menu items is selected then unselect it
                if (m_menus[visibleMenuIdx].selectedMenuItem >= 0)
                {
                    updateMenuTextColor(m_menus[i].menuItems[static_cast<std::size_t>(m_menus[visibleMenuIdx].selectedMenuItem)], false);
                    m_menus[visibleMenuIdx].selectedMenuItem = -1;
                }
            }
            else // The menu isn't open yet
            {
                // If this menu can be opened then do so
                if (m_menus[i].enabled && !m_menus[i].menuItems.empty())
                    openMenu(i);
                else // The menu is disabled, if there is still another menu open then close it now
                    closeMenu();
            }

            handled = true;
            break;
        }

        // The mouse is to the right of all menus, deselect the selected item of the deepest submenu
        if (!handled)
            deselectBottomItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MenuBar::getSignal(String signalName)
    {
        if (signalName == onMenuItemClick.getName())
            return onMenuItemClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::rendererChanged(const String& property)
    {
        if (property == U"TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == U"SelectedTextColor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == U"TextColorDisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"TextureItemBackground")
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
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);
            updateTextOpacity(m_menus);
            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);
            updateTextFont(m_menus);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> MenuBar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        saveMenus(node, m_menus);

        node->propertyValuePairs[U"MinimumSubMenuWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimumSubMenuWidth));
        if (m_invertedMenuDirection)
            node->propertyValuePairs[U"InvertedMenuDirection"] = std::make_unique<DataIO::ValueNode>("true");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"MinimumSubMenuWidth"])
            setMinimumSubMenuWidth(node->propertyValuePairs[U"MinimumSubMenuWidth"]->value.toFloat());
        if (node->propertyValuePairs[U"InvertedMenuDirection"])
            setInvertedMenuDirection(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"InvertedMenuDirection"]->value).getBool());

        loadMenus(node, m_menus);

        // Remove the 'menu' nodes as they have been processed
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
            [](const std::unique_ptr<DataIO::Node>& child){ return child->name == U"Menu"; }), node->children.end());

        // Update the text colors to properly display disabled menus
        updateTextColors(m_menus, m_visibleMenu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the background
        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else
            target.drawFilledRect(states, getSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached));

        if (m_menus.empty())
            return;

        drawMenusOnBar(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::drawOpenMenu(BackendRenderTarget& target, RenderStates states) const
    {
        TGUI_ASSERT(m_visibleMenu >= 0, "MenuBar::drawOpenMenu can only be called when a menu is open");
        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);

        // Find the position of the menu
        float leftOffset = 0;
        for (std::size_t i = 0; i < visibleMenuIdx; ++i)
            leftOffset += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

        // Move the menu to the left if it otherwise falls off the screen
        bool openSubMenuToRight = true;
        const float menuWidth = calculateMenuWidth(m_menus[visibleMenuIdx]);
        if (getParent() && (getPosition().x + leftOffset + menuWidth > getParent()->getInnerSize().x))
        {
            leftOffset = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
            openSubMenuToRight = false;
        }

        if (m_invertedMenuDirection)
            states.transform.translate({leftOffset, -calculateOpenMenuHeight(m_menus[visibleMenuIdx].menuItems)});
        else
            states.transform.translate({leftOffset, getSize().y});

        drawMenu(target, states, m_menus[visibleMenuIdx], menuWidth, getPosition().x + leftOffset, openSubMenuToRight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::createMenu(std::vector<Menu>& menus, const String& text)
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

    MenuBar::Menu* MenuBar::findMenu(const std::vector<String>& hierarchy, unsigned int parentIndex, std::vector<Menu>& menus, bool createParents)
    {
        assert(hierarchy.size() >= 2);

        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 2 == hierarchy.size())
                return &menu;
            else
                return findMenu(hierarchy, parentIndex + 1, menu.menuItems, createParents);
        }

        if (createParents)
        {
            createMenu(menus, hierarchy[parentIndex]);
            if (parentIndex + 2 == hierarchy.size())
                return &menus.back();
            else
                return findMenu(hierarchy, parentIndex + 1, menus.back().menuItems, createParents);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBar::Menu* MenuBar::findMenu(const std::vector<String>& hierarchy, unsigned int parentIndex, const std::vector<Menu>& menus) const
    {
        assert(hierarchy.size() >= 2);

        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 2 == hierarchy.size())
                return &menu;
            else
                return findMenu(hierarchy, parentIndex + 1, menu.menuItems);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Menu* MenuBar::findMenuItem(const std::vector<String>& hierarchy)
    {
        if (hierarchy.empty())
            return nullptr;

        std::vector<Menu>* menuItems;
        if (hierarchy.size() == 1)
            menuItems = &m_menus;
        else
        {
            auto* menu = findMenu(hierarchy, 0, m_menus, false);
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

    const MenuBar::Menu* MenuBar::findMenuItem(const std::vector<String>& hierarchy) const
    {
        if (hierarchy.empty())
            return nullptr;

        const std::vector<Menu>* menuItems;
        if (hierarchy.size() == 1)
            menuItems = &m_menus;
        else
        {
            const auto* menu = findMenu(hierarchy, 0, m_menus);
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

    void MenuBar::loadMenus(const std::unique_ptr<DataIO::Node>& node, std::vector<Menu>& menus)
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

    void MenuBar::closeSubMenus(std::vector<Menu>& menus, int& selectedMenu)
    {
        if (selectedMenu < 0)
            return;

        const auto selectedMenuIdx = static_cast<std::size_t>(selectedMenu);
        closeSubMenus(menus[selectedMenuIdx].menuItems, menus[selectedMenuIdx].selectedMenuItem);

        updateMenuTextColor(menus[selectedMenuIdx], false);
        selectedMenu = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::deselectBottomItem()
    {
        TGUI_ASSERT(m_visibleMenu >= 0, "MenuBar::deselectBottomItem can only be called when a menu is open");

        auto* menu = &m_menus[static_cast<std::size_t>(m_visibleMenu)];
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

    void MenuBar::updateMenuTextColor(Menu& menu, bool selected)
    {
        if ((!m_enabled || !menu.enabled) && m_textColorDisabledCached.isSet())
            menu.text.setColor(m_textColorDisabledCached);
        else if (selected && m_selectedTextColorCached.isSet())
            menu.text.setColor(m_selectedTextColorCached);
        else
            menu.text.setColor(m_textColorCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextColors(std::vector<Menu>& menus, int selectedMenu)
    {
        for (std::size_t i = 0; i < menus.size(); ++i)
        {
            updateMenuTextColor(menus[i], (selectedMenu == static_cast<int>(i)));
            updateTextColors(menus[i].menuItems, menus[i].selectedMenuItem);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextOpacity(std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            menu.text.setOpacity(m_opacityCached);
            updateTextOpacity(menu.menuItems);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextFont(std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            menu.text.setFont(m_fontCached);
            updateTextFont(menu.menuItems);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::calculateMenuWidth(const Menu& menu) const
    {
        float maxWidth = m_minimumSubMenuWidth;
        for (const auto& item : menu.menuItems)
        {
            float width = item.text.getSize().x + (2.f * m_distanceToSideCached);

            // Reserve space for an arrow if there are submenus
            if (!item.menuItems.empty())
                width += (getSize().y / 4.f) + m_distanceToSideCached;

            if (width > maxWidth)
                maxWidth = width;
        }

        return maxWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::getMenuItemHeight(const Menu& menuItem) const
    {
        if (isSeparator(menuItem))
            return m_separatorThicknessCached + 2*m_separatorVerticalPaddingCached;
        else
            return getSize().y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::calculateOpenMenuHeight(const std::vector<Menu>& menuItems) const
    {
        float height = 0;
        for (const auto& item : menuItems)
            height += getMenuItemHeight(item);

        return height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f MenuBar::calculateSubmenuOffset(const Menu& menu, float globalLeftPos, float menuWidth, float subMenuWidth, bool& openSubMenuToRight) const
    {
        TGUI_ASSERT(menu.selectedMenuItem >= 0, "MenuBar::calculateSubmenuOffset can only be called when the menu has an open submenu");
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
            topOffset -= calculateOpenMenuHeight(menu.menuItems[selectedMenuItemIdx].menuItems) - getSize().y;

        return {leftOffset, topOffset};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::isMouseOnTopOfMenu(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, const Menu& menu, float menuWidth) const
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

    bool MenuBar::isMouseOnOpenMenu(Vector2f pos) const
    {
        // If there is no open menu then the mouse can't be on top of it
        if (m_visibleMenu < 0)
            return false;

        // If the mouse is on top of the menu bar itself then it isn't on one of the menus
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            return false;

        Vector2f menuPos{0, 0};
        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);
        for (std::size_t i = 0; i < visibleMenuIdx; ++i)
            menuPos.x += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

        if (m_invertedMenuDirection)
            menuPos.y -= calculateOpenMenuHeight(m_menus[visibleMenuIdx].menuItems);
        else
            menuPos.y += getSize().y;

        // The menu is moved to the left if it otherwise falls off the screen
        bool openSubMenuToRight = true;
        const float menuWidth = calculateMenuWidth(m_menus[visibleMenuIdx]);
        if (getParent() && (menuPos.x + menuWidth > getParent()->getInnerSize().x))
        {
            menuPos.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
            openSubMenuToRight = false;
        }

        return isMouseOnTopOfMenu(menuPos, pos, openSubMenuToRight, m_menus[visibleMenuIdx], menuWidth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleasedOnMenu()
    {
        // This function may be called while closing the menu (when removing the placeholder widget), in which case m_visibleMenu will be -1
        if (m_visibleMenu < 0)
            return;

        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);

        auto* menu = &m_menus[visibleMenuIdx];
        std::vector<String> hierarchy;
        hierarchy.push_back(m_menus[visibleMenuIdx].text.getString());
        while (menu->selectedMenuItem >= 0)
        {
            auto& menuItem = menu->menuItems[static_cast<std::size_t>(menu->selectedMenuItem)];
            hierarchy.push_back(menuItem.text.getString());
            if (menuItem.menuItems.empty())
            {
                onMenuItemClick.emit(this, menuItem.text.getString(), hierarchy);
                break;
            }

            menu = &menuItem;
        }

        closeMenu();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMovedOnMenu(Vector2f pos)
    {
        TGUI_ASSERT(m_visibleMenu >= 0, "MenuBar::mouseMovedOnMenu can only be called when a menu is open");
        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);

        Vector2f menuPos{0, 0};
        for (std::size_t i = 0; i < visibleMenuIdx; ++i)
            menuPos.x += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

        if (m_invertedMenuDirection)
            menuPos.y -= calculateOpenMenuHeight(m_menus[visibleMenuIdx].menuItems);
        else
            menuPos.y += getSize().y;

        // The menu is moved to the left if it otherwise falls off the screen
        bool openSubMenuToRight = true;
        const float menuWidth = calculateMenuWidth(m_menus[visibleMenuIdx]);
        if (getParent() && (menuPos.x + menuWidth > getParent()->getInnerSize().x))
        {
            menuPos.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
            openSubMenuToRight = false;
        }

        Menu* menuBelowMouse;
        std::size_t menuItemIndexBelowMouse;
        if (findMenuItemBelowMouse(menuPos, pos, openSubMenuToRight, m_menus[visibleMenuIdx], menuWidth, &menuBelowMouse, &menuItemIndexBelowMouse))
        {
            // Check if the mouse is on a different item than before
            auto& menu = *menuBelowMouse;
            if (static_cast<int>(menuItemIndexBelowMouse) != menu.selectedMenuItem)
            {
                // If another of the menu items is selected then unselect it
                closeSubMenus(menu.menuItems, menu.selectedMenuItem);

                // Mark the item below the mouse as selected
                auto& menuItem = menu.menuItems[menuItemIndexBelowMouse];
                if (menuItem.enabled && !isSeparator(menuItem))
                {
                    updateMenuTextColor(menu.menuItems[menuItemIndexBelowMouse], true);
                    menu.selectedMenuItem = static_cast<int>(menuItemIndexBelowMouse);
                }
            }
            else // We already selected this item
            {
                // If the selected item has a submenu then unselect its item
                closeSubMenus(menu.menuItems[menuItemIndexBelowMouse].menuItems, menu.menuItems[menuItemIndexBelowMouse].selectedMenuItem);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::findMenuItemBelowMouse(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, Menu& menu, float menuWidth, Menu** resultMenu, std::size_t* resultSelectedMenuItem)
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

    void MenuBar::drawMenusOnBar(BackendRenderTarget& target, RenderStates states) const
    {
        Transform oldTransform = states.transform;

        // Draw the backgrounds
        Sprite backgroundSprite = m_spriteItemBackground;
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            const bool isMenuOpen = (m_visibleMenu == static_cast<int>(i));
            const float width = m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
            if (backgroundSprite.isSet())
            {
                if (isMenuOpen && m_spriteSelectedItemBackground.isSet())
                {
                    Sprite selectedBackgroundSprite = m_spriteSelectedItemBackground;
                    selectedBackgroundSprite.setSize({width, getSize().y});
                    target.drawSprite(states, selectedBackgroundSprite);
                }
                else // Not selected or no different texture for selected menu
                {
                    backgroundSprite.setSize({width, getSize().y});
                    target.drawSprite(states, backgroundSprite);
                }
            }
            else // No textures where loaded
            {
                if (isMenuOpen && m_selectedBackgroundColorCached.isSet())
                    target.drawFilledRect(states, {width, getSize().y}, Color::applyOpacity(m_selectedBackgroundColorCached, m_opacityCached));
            }

            states.transform.translate({width, 0});
        }

        states.transform = oldTransform;

        // Draw the texts
        const float textHeight = m_menus[0].text.getSize().y;
        states.transform.translate({m_distanceToSideCached, (getSize().y - textHeight) / 2.f});
        for (const auto& menu : m_menus)
        {
            target.drawText(states, menu.text);

            const float width = menu.text.getSize().x + (2 * m_distanceToSideCached);
            states.transform.translate({width, 0});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::drawMenu(BackendRenderTarget& target, RenderStates states, const Menu& menu, float menuWidth, float globalLeftPos, bool openSubMenuToRight) const
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
        states.transform.translate({m_distanceToSideCached, (getSize().y - menu.text.getSize().y) / 2.f});
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
                const float arrowHeight = getSize().y / 2.f;
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

            states.transform.translate({0, getSize().y});
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
                topOffset -= calculateOpenMenuHeight(menu.menuItems[selectedMenuItemIdx].menuItems) - getSize().y;

            states.transform.translate({leftOffset, topOffset});
            drawMenu(target, states, menu.menuItems[selectedMenuItemIdx], subMenuWidth, globalLeftPos + leftOffset, openSubMenuToRight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr MenuBar::clone() const
    {
        return std::make_shared<MenuBar>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarMenuPlaceholder::MenuBarMenuPlaceholder(MenuBar* menuBar) :
        Widget{"MenuBarMenuPlaceholder", true},
        m_menuBar{menuBar}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f MenuBarMenuPlaceholder::getFullSize() const
    {
        if (m_parent)
            return m_parent->getInnerSize() - getPosition();
        else
            return {0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f MenuBarMenuPlaceholder::getWidgetOffset() const
    {
        return -getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBarMenuPlaceholder::isMouseOnWidget(Vector2f) const
    {
        return true; // Absorb all mouse events until the menu is closed
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarMenuPlaceholder::leftMouseButtonNoLongerDown()
    {
        m_menuBar->leftMouseReleasedOnMenu();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarMenuPlaceholder::mouseMoved(Vector2f pos)
    {
        bool mouseOnMenuBar = false;
        if (m_menuBar->isMouseOnOpenMenu(pos - getPosition()))
        {
            m_mouseHover = true;
            m_menuBar->mouseMovedOnMenu(pos - getPosition());
        }
        else
        {
            if (m_mouseHover)
            {
                // Deselect the selected item of the deepest submenu
                m_menuBar->deselectBottomItem();
                m_mouseHover = false;
            }

            if (m_menuBar->getParent())
            {
                const Vector2f menuBarMousePos = pos - m_menuBar->getAbsolutePosition() + m_menuBar->getPosition();
                if (m_menuBar->isMouseOnWidget(menuBarMousePos))
                {
                    mouseOnMenuBar = true;
                    m_mouseWasOnMenuBar = true;
                    m_menuBar->mouseMoved(menuBarMousePos);
                }
            }
        }

        if (!mouseOnMenuBar && m_mouseWasOnMenuBar)
        {
            m_mouseWasOnMenuBar = false;
            m_menuBar->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarMenuPlaceholder::draw(BackendRenderTarget& target, RenderStates states) const
    {
        m_menuBar->drawOpenMenu(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr MenuBarMenuPlaceholder::clone() const
    {
        // This function should never be called
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
