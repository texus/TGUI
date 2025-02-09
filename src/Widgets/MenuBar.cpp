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

#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Container.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char MenuBar::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar(const char* typeName, bool initRenderer) :
        MenuWidgetBase{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<MenuBarRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
        }

        setMinimumSubMenuWidth((Text::getLineHeight(m_fontCached, m_textSizeCached) * 4) + (2 * m_distanceToSideCached));
        setSize({"100%", std::round(Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f)});
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
        return aurora::downcast<MenuBarRenderer*>(MenuWidgetBase::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBarRenderer* MenuBar::getSharedRenderer() const
    {
        return aurora::downcast<const MenuBarRenderer*>(MenuWidgetBase::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarRenderer* MenuBar::getRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(MenuWidgetBase::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout2d& size)
    {
        MenuWidgetBase::setSize(size);

        m_spriteBackground.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setEnabled(bool enabled)
    {
        if (m_enabled == enabled)
            return;

        MenuWidgetBase::setEnabled(enabled);

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

        auto* menu = findMenuItemParent(hierarchy, 0, m_menus, createParents);
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

        auto* menu = findMenuItem(hierarchy, m_menus);
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

        auto* menu = findMenuItemParent(hierarchy, 0, m_menus, false);
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

        auto* menuItem = findMenuItem(hierarchy, m_menus);
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
        m_minimumMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::getMinimumSubMenuWidth() const
    {
        return m_minimumMenuWidth;
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

    std::vector<MenuWidgetBase::GetMenusElement> MenuBar::getMenus() const
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

            m_openMenuPlaceholder->setPosition(getAbsolutePosition());
            m_openMenuPlaceholder->setScale(scale);
            container->add(m_openMenuPlaceholder, "#TGUI_INTERNAL$OpenMenuPlaceholder#");

            // Focus the menu itself to handle key events (e.g. escape to close the menu)
            m_openMenuPlaceholder->setFocused(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeMenu()
    {
        if (m_visibleMenu < 0)
            return;

        closeSubMenus(m_menus, m_visibleMenu);

        if (m_openMenuPlaceholder->getParent())
            m_openMenuPlaceholder->getParent()->remove(m_openMenuPlaceholder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::isMouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::leftMousePressed(Vector2f pos)
    {
        MenuWidgetBase::leftMousePressed(pos);

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
            deselectDeepestItem();
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
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            MenuWidgetBase::rendererChanged(property);
            updateTextOpacity(m_menus);
            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            MenuWidgetBase::rendererChanged(property);
            updateTextFont(m_menus);
        }
        else
            MenuWidgetBase::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> MenuBar::save(SavingRenderersMap& renderers) const
    {
        auto node = MenuWidgetBase::save(renderers);

        saveMenus(node, m_menus);

        node->propertyValuePairs[U"MinimumSubMenuWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimumMenuWidth));
        if (m_invertedMenuDirection)
            node->propertyValuePairs[U"InvertedMenuDirection"] = std::make_unique<DataIO::ValueNode>("true");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        MenuWidgetBase::load(node, renderers);

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

    Vector2f MenuBar::calculateMenuOffset(std::size_t visibleMenuIdx) const
    {
        Vector2f menuOffset;
        for (std::size_t i = 0; i < visibleMenuIdx; ++i)
            menuOffset.x += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

        if (m_invertedMenuDirection)
            menuOffset.y = -calculateOpenMenuHeight(m_menus[visibleMenuIdx].menuItems);
        else
            menuOffset.y = getSize().y;

        return menuOffset;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::drawOpenMenu(BackendRenderTarget& target, RenderStates states) const
    {
        TGUI_ASSERT(m_visibleMenu >= 0, "MenuBar::drawOpenMenu can only be called when a menu is open");
        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);
        MenuWidgetBase::drawOpenMenu(target, states, m_menus[visibleMenuIdx], calculateMenuOffset(visibleMenuIdx));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::emitMenuItemClick(const std::vector<String>& hierarchy)
    {
        TGUI_ASSERT(hierarchy.size() >= 2, "hierarchy must contain at least 2 elements in MenuBar::emitMenuItemClick");
        onMenuItemClick.emit(this, hierarchy.back(), hierarchy);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::deselectDeepestItem()
    {
        TGUI_ASSERT(m_visibleMenu >= 0, "MenuBar::deselectDeepestItem can only be called when a menu is open");
        MenuWidgetBase::deselectDeepestItem(&m_menus[static_cast<std::size_t>(m_visibleMenu)]);
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

        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);
        return MenuWidgetBase::isMouseOnOpenMenu(pos, m_menus[visibleMenuIdx], calculateMenuOffset(visibleMenuIdx));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::getDefaultMenuItemHeight() const
    {
        return getSize().y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleasedOnMenu()
    {
        // This function may be called while closing the menu (when removing the placeholder widget), in which case m_visibleMenu will be -1
        if (m_visibleMenu < 0)
            return;

        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);

        MenuWidgetBase::leftMouseReleasedOnMenu(&m_menus[visibleMenuIdx]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMovedOnMenu(Vector2f pos)
    {
        TGUI_ASSERT(m_visibleMenu >= 0, "MenuBar::mouseMovedOnMenu can only be called when a menu is open");
        const auto visibleMenuIdx = static_cast<std::size_t>(m_visibleMenu);

        MenuWidgetBase::mouseMovedOnMenu(pos, m_menus[visibleMenuIdx], calculateMenuOffset(visibleMenuIdx));
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

    Widget::Ptr MenuBar::clone() const
    {
        return std::make_shared<MenuBar>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
