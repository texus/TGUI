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

#include <TGUI/Widgets/ContextMenu.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char ContextMenu::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContextMenu::ContextMenu(const char* typeName, bool initRenderer) :
        MenuWidgetBase{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ContextMenuRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setItemHeight(std::round(Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f));
            setMinimumMenuWidth((Text::getLineHeight(m_fontCached, m_textSizeCached) * 4) + (2 * m_distanceToSideCached));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContextMenu::Ptr ContextMenu::create()
    {
        return std::make_shared<ContextMenu>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContextMenu::Ptr ContextMenu::copy(const ContextMenu::ConstPtr& menuBar)
    {
        if (menuBar)
            return std::static_pointer_cast<ContextMenu>(menuBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContextMenuRenderer* ContextMenu::getSharedRenderer()
    {
        return aurora::downcast<ContextMenuRenderer*>(MenuWidgetBase::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ContextMenuRenderer* ContextMenu::getSharedRenderer() const
    {
        return aurora::downcast<const ContextMenuRenderer*>(MenuWidgetBase::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ContextMenuRenderer* ContextMenu::getRenderer()
    {
        return aurora::downcast<ContextMenuRenderer*>(MenuWidgetBase::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::isMenuOpen() const
    {
        return m_menuOpen;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::openMenu()
    {
        closeMenu();

        m_menuOpen = true;

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

            // Steal the focus, to intercept key events and to e.g. prevent typing in an edit box while the menu is open
            m_openMenuPlaceholder->setFocused(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::openMenu(Vector2f position)
    {
        setPosition(position);
        openMenu();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::openMenuAtMouseCursor()
    {
        auto position = m_position;
        if (m_parentGui)
        {
            const Vector2f lastMousePos = m_parentGui->mapPixelToCoords(m_parentGui->getLastMousePosition());
            position = lastMousePos - getAbsolutePosition(-getPosition());
        }

        setPosition(position);
        openMenu();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::closeMenu()
    {
        if (!m_menuOpen)
            return;

        closeSubMenus(m_menu.menuItems, m_menu.selectedMenuItem);
        m_menuOpen = false;

        if (m_openMenuPlaceholder->getParent())
            m_openMenuPlaceholder->getParent()->remove(m_openMenuPlaceholder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::setSize(const Layout2d&)
    {
        // Manually changing the size is not possible, the context menu is always auto-sized
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::setEnabled(bool enabled)
    {
        if (m_enabled == enabled)
            return;

        Widget::setEnabled(enabled);

        if (!enabled)
            closeMenu();

        updateTextColors(m_menu.menuItems, m_menu.selectedMenuItem);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::setItemHeight(float itemHeight)
    {
        m_itemHeight = itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ContextMenu::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::addMenuItem(const String& text)
    {
        createMenu(m_menu.menuItems, text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::addMenuItem(const std::vector<String>& hierarchy, bool createParents)
    {
        if (hierarchy.empty())
            return false;

        if (hierarchy.size() == 1)
        {
            createMenu(m_menu.menuItems, hierarchy.back());
            return true;
        }

        auto* menu = findMenuItemParent(hierarchy, 0, m_menu.menuItems, createParents);
        if (!menu)
            return false;

        createMenu(menu->menuItems, hierarchy.back());
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::changeMenuItem(const std::vector<String>& hierarchy, const String& text)
    {
        if (hierarchy.empty())
            return false;

        auto* menu = findMenuItem(hierarchy, m_menu.menuItems);
        if (!menu)
            return false;

        menu->text.setString(text);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::removeAllMenuItems()
    {
        m_menu.menuItems.clear();
        m_menu.selectedMenuItem = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::removeMenuItem(const String& menuItem)
    {
        return removeMenuItem(std::vector<String>{menuItem}, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::removeMenuItem(const std::vector<String>& hierarchy, bool removeParentsWhenEmpty)
    {
        if (hierarchy.empty())
            return false;

        return removeMenuImpl(hierarchy, removeParentsWhenEmpty, 0, m_menu.menuItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::removeSubMenuItems(const std::vector<String>& hierarchy)
    {
        if (hierarchy.empty())
            return false;

        return removeSubMenusImpl(hierarchy, 0, m_menu.menuItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::setMenuItemEnabled(const String& menuItem, bool enabled)
    {
        return setMenuItemEnabled(std::vector<String>{menuItem}, enabled);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::setMenuItemEnabled(const std::vector<String>& hierarchy, bool enabled)
    {
        if (hierarchy.empty())
            return false;

        Menu* menu = &m_menu;
        if (hierarchy.size() >= 2)
        {
            menu = findMenuItemParent(hierarchy, 0, m_menu.menuItems, false);
            if (!menu)
                return false;
        }

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

    bool ContextMenu::getMenuItemEnabled(const String& menuItem) const
    {
        return getMenuItemEnabled(std::vector<String>{menuItem});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::getMenuItemEnabled(const std::vector<String>& hierarchy) const
    {
        if (hierarchy.empty())
            return false;

        auto* menuItem = findMenuItem(hierarchy, m_menu.menuItems);
        if (!menuItem)
            return false;

        return menuItem->enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::setMinimumMenuWidth(float minimumWidth)
    {
        m_minimumMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ContextMenu::getMinimumMenuWidth() const
    {
        return m_minimumMenuWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<MenuWidgetBase::GetMenusElement> ContextMenu::getMenuItems() const
    {
        return getMenusImpl(m_menu.menuItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::isMouseOnWidget(Vector2f) const
    {
        // The widget can't be interacted with directly.
        // When the menu is open then the OpenMenuPlaceholder will intercept the mouse event and forward it to this class.
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::draw(BackendRenderTarget&, RenderStates) const
    {
        // Nothing is drawn here.
        // When the menu is open then the OpenMenuPlaceholder will call the drawOpenMenu function to do the rendering.
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::rendererChanged(const String& property)
    {
        if (property == U"TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors(m_menu.menuItems, m_menu.selectedMenuItem);
        }
        else if (property == U"SelectedTextColor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors(m_menu.menuItems, m_menu.selectedMenuItem);
        }
        else if (property == U"TextColorDisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            updateTextColors(m_menu.menuItems, m_menu.selectedMenuItem);
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);
            updateTextOpacity(m_menu.menuItems);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);
            updateTextFont(m_menu.menuItems);
        }
        else
            MenuWidgetBase::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ContextMenu::save(SavingRenderersMap& renderers) const
    {
        auto node = MenuWidgetBase::save(renderers);
        saveMenus(node, m_menu.menuItems);
        node->propertyValuePairs[U"MinimumMenuWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimumMenuWidth));
        node->propertyValuePairs[U"ItemHeight"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_itemHeight));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        MenuWidgetBase::load(node, renderers);

        loadMenus(node, m_menu.menuItems);
        m_menu.selectedMenuItem = -1;

        // Remove the 'menu' nodes as they have been processed
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
            [](const std::unique_ptr<DataIO::Node>& child){ return child->name == U"Menu"; }), node->children.end());

        if (node->propertyValuePairs[U"MinimumMenuWidth"])
            setMinimumMenuWidth(node->propertyValuePairs[U"MinimumMenuWidth"]->value.toFloat());
        if (node->propertyValuePairs[U"ItemHeight"])
            setItemHeight(node->propertyValuePairs[U"ItemHeight"]->value.toFloat());

        // Update the text colors to properly display disabled menus
        updateTextColors(m_menu.menuItems, m_menu.selectedMenuItem);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::updateTextSize()
    {
        setTextSizeImpl(m_menu.menuItems, m_textSizeCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ContextMenu::clone() const
    {
        return std::make_shared<ContextMenu>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::drawOpenMenu(BackendRenderTarget& target, RenderStates states) const
    {
        TGUI_ASSERT(m_menuOpen, "ContextMenu::drawOpenMenu can only be called when the menu is open");
        MenuWidgetBase::drawOpenMenu(target, states, m_menu, {0, 0});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::emitMenuItemClick(const std::vector<String>& hierarchy)
    {
        TGUI_ASSERT(hierarchy.size() >= 2, "hierarchy must contain at least 2 elements in ContextMenu::emitMenuItemClick");
        TGUI_ASSERT(hierarchy.front().empty(), "hierarchy must start with an empty element in ContextMenu::emitMenuItemClick");

        // The fist element of the hierarchy is always empty due to the way we stored it (which is done because the code
        // originated from MenuBar where there are multiple named menus, while ContextMenu uses a single unnamed menu).
        std::vector<String> correctedHierarchy = hierarchy;
        correctedHierarchy.erase(correctedHierarchy.begin());

        onMenuItemClick.emit(this, correctedHierarchy.back(), correctedHierarchy);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::deselectDeepestItem()
    {
        TGUI_ASSERT(m_menuOpen, "ContextMenu::deselectDeepestItem can only be called when the menu is open");
        MenuWidgetBase::deselectDeepestItem(&m_menu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ContextMenu::isMouseOnOpenMenu(Vector2f pos) const
    {
        // If the menu isn't open then the mouse can't be on top of it
        if (!m_menuOpen)
            return false;

        return MenuWidgetBase::isMouseOnOpenMenu(pos, m_menu, {0, 0});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ContextMenu::getDefaultMenuItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::leftMouseReleasedOnMenu()
    {
        // This function may be called while closing the menu (when removing the placeholder widget)
        if (!m_menuOpen)
            return;

        MenuWidgetBase::leftMouseReleasedOnMenu(&m_menu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ContextMenu::mouseMovedOnMenu(Vector2f pos)
    {
        TGUI_ASSERT(m_menuOpen, "ContextMenu::mouseMovedOnMenu can only be called when the menu is open");

        MenuWidgetBase::mouseMovedOnMenu(pos, m_menu, {0, 0});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
