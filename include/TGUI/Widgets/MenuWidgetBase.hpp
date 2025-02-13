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

#ifndef TGUI_MENU_WIDGET_BASE_HPP
#define TGUI_MENU_WIDGET_BASE_HPP

#include <TGUI/Widget.hpp>
#include <TGUI/Renderers/MenuWidgetBaseRenderer.hpp>
#include <TGUI/Text.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    class OpenMenuPlaceholder;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for MenuBar and ContextMenu
    ///
    /// @since TGUI 1.8
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API MenuWidgetBase : public Widget
    {
    public:

        /// @brief Used for return value of getMenus
        struct GetMenusElement
        {
            String text;
            bool enabled;
            std::vector<GetMenusElement> menuItems;
        };

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MenuWidgetBase(const MenuWidgetBase& other);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Move constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MenuWidgetBase(MenuWidgetBase&& other) noexcept;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MenuWidgetBase& operator=(const MenuWidgetBase& other);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Move assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MenuWidgetBase& operator=(MenuWidgetBase&& other) noexcept;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer that may be shared with other widgets using the same renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD MenuWidgetBaseRenderer* getSharedRenderer() override;
        TGUI_NODISCARD const MenuWidgetBaseRenderer* getSharedRenderer() const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer
        /// @warning After calling this function, the widget has its own copy of the renderer and it will no longer be shared.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD MenuWidgetBaseRenderer* getRenderer() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Closes the menu if it was open
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void closeMenu() = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        struct Menu
        {
            Text text;
            bool enabled = true;
            int selectedMenuItem = -1;
            std::vector<Menu> menuItems;
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MenuWidgetBase(const char* typeName, bool initRenderer);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves a signal based on its name
        ///
        /// @param signalName  Name of the signal
        ///
        /// @return Signal that corresponds to the name
        ///
        /// @throw Exception when the name does not match any signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Signal& getSignal(String signalName) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function called when one of the properties of the renderer is changed
        ///
        /// @param property  Name of the property that was changed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rendererChanged(const String& property) override;

        /// @internal
        /// Helper function to create a new menu or menu item
        void createMenu(std::vector<Menu>& menus, const String& text);

        /// @internal
        /// Recursively search for the menu containing the menu item specified in the hierarchy, creating the hierarchy if requested.
        /// The initial call to this function must pass "parentIndex = 0".
        TGUI_NODISCARD Menu* findMenuItemParent(const std::vector<String>& hierarchy, unsigned int parentIndex, std::vector<Menu>& menus, bool createParents);

        /// @internal
        /// Recursively search for the menu containing the menu item specified in the hierarchy.
        /// The initial call to this function must pass "parentIndex = 0".
        TGUI_NODISCARD const Menu* findMenuItemParent(const std::vector<String>& hierarchy, unsigned int parentIndex, const std::vector<Menu>& menus) const;

        /// @internal
        /// Search for the menu item specified in the hierarchy and return a pointer to it.
        TGUI_NODISCARD Menu* findMenuItem(const std::vector<String>& hierarchy, std::vector<Menu>& menus);

        /// @internal
        /// Search for the menu item specified in the hierarchy and return a read-only pointer to it.
        TGUI_NODISCARD const Menu* findMenuItem(const std::vector<String>& hierarchy, const std::vector<Menu>& menus) const;

        /// @internal
        /// Helper function to load the menus when the menu bar is being loaded from a text file
        void loadMenus(const std::unique_ptr<DataIO::Node>& node, std::vector<Menu>& menus);

        /// @internal
        /// Closes the open menu and its submenus
        void closeSubMenus(std::vector<Menu>& menus, int& selectedMenu);

        /// @internal
        void updateMenuTextColor(Menu& menu, bool selected);

        /// @internal
        void updateTextColors(std::vector<Menu>& menus, int selectedMenu);

        /// @internal
        void updateTextOpacity(std::vector<Menu>& menus);

        /// @internal
        void updateTextFont(std::vector<Menu>& menus);

        /// @internal
        /// Calculate the width that is needed for the menu to fit all menu items
        TGUI_NODISCARD float calculateMenuWidth(const Menu& menu) const;

        /// @internal
        /// Returns the height of the menu item or the separator
        TGUI_NODISCARD float getMenuItemHeight(const Menu& menuItem) const;

        /// @internal
        /// Calculates the height of all menu items and separators in a menu
        TGUI_NODISCARD float calculateOpenMenuHeight(const std::vector<Menu>& menuItems) const;

        /// @internal
        TGUI_NODISCARD Vector2f calculateSubmenuOffset(const Menu& menu, float globalLeftPos, float menuWidth, float subMenuWidth, bool& openSubMenuToRight) const;

        /// @internal
        TGUI_NODISCARD bool isMouseOnTopOfMenu(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, const Menu& menu, float menuWidth) const;

        /// @internal
        TGUI_NODISCARD bool findMenuItemBelowMouse(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, Menu& menu, float menuWidth, Menu** resultMenu, std::size_t* resultSelectedMenuItem);

        /// @internal
        static bool isSeparator(const Menu& menuItem);

        /// @internal
        static bool removeMenuImpl(const std::vector<String>& hierarchy, bool removeParentsWhenEmpty, unsigned int parentIndex, std::vector<Menu>& menus);

        /// @internal
        static bool removeSubMenusImpl(const std::vector<String>& hierarchy, unsigned int parentIndex, std::vector<Menu>& menus);

        /// @internal
        static void setTextSizeImpl(std::vector<Menu>& menus, unsigned int textSize);

        /// @internal
        static std::vector<GetMenusElement> getMenusImpl(const std::vector<Menu>& menus);

        /// @internal
        static void saveMenus(const std::unique_ptr<DataIO::Node>& parentNode, const std::vector<Menu>& menus);

        /// @internal
        /// Draw an open menu and recusively draw submenus when open
        void drawMenu(BackendRenderTarget& target, RenderStates states, const Menu& menu, float menuWidth, float globalLeftPos, bool openSubMenuToRight) const;

        /// @internal
        void leftMouseReleasedOnMenu(const Menu* menu);

        /// @internal
        bool isMouseOnOpenMenu(Vector2f pos, const Menu& menu, Vector2f menuOffset) const;

        /// @internal
        void mouseMovedOnMenu(Vector2f pos, Menu& menu, Vector2f menuOffset);

        /// @internal
        void deselectDeepestItem(Menu* menu);

        /// @internal
        void drawOpenMenu(BackendRenderTarget& target, RenderStates states, const Menu& menu, Vector2f menuOffset) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Triggers the onMenuItemClick callback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void emitMenuItemClick(const std::vector<String>& hierarchy) = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Returns the height of a normal menu item (i.e. not a separator)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD virtual float getDefaultMenuItemHeight() const = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Left mouse button was released while a menu might be open
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void leftMouseReleasedOnMenu() = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Is the mouse located on one of an open menu?
        /// @param pos  Mouse position
        /// @return True if mouse on menu, false otherwise.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD virtual bool isMouseOnOpenMenu(Vector2f pos) const = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Mouse moved on top of a menu
        /// @param pos  Mouse position
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void mouseMovedOnMenu(Vector2f pos) = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Deselects the selected item of the deepest open submenu
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void deselectDeepestItem() = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Called from OpenMenuPlaceholder to draw the menu that is currently open
        /// @param target Render target to draw to
        /// @param states Current render states
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawOpenMenu(BackendRenderTarget& target, RenderStates states) const = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /// One of the menu items was clicked.
        /// Optional parameters:
        ///     - The text of the clicked menu item
        ///     - List containing both the name of the menu and the menu item that was clicked
        SignalItemHierarchy onMenuItemClick = {"MenuItemClicked"};

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::shared_ptr<OpenMenuPlaceholder> m_openMenuPlaceholder;

        float m_minimumMenuWidth = 125;

        Sprite m_spriteItemBackground;
        Sprite m_spriteSelectedItemBackground;

        // Cached renderer properties
        Color m_backgroundColorCached;
        Color m_selectedBackgroundColorCached;
        Color m_textColorCached;
        Color m_selectedTextColorCached;
        Color m_textColorDisabledCached;
        Color m_separatorColorCached = Color::Black;
        float m_separatorThicknessCached = 1;
        float m_separatorVerticalPaddingCached = 0;
        float m_separatorSidePaddingCached = 0;
        float m_distanceToSideCached = 0;

        bool m_invertedMenuDirection = false; // Only used by MenuBar

        friend class OpenMenuPlaceholder;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Widget that is added to a container when the user clicks on the menu bar. This widget will be added in front of
    ///        all other widgets to ensure that the menus from the menu bar are always drawn in front of other widgets.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class OpenMenuPlaceholder : public Widget
    {
    public:

        // Instances of this class can't be copied
        OpenMenuPlaceholder(const OpenMenuPlaceholder&) = delete;
        OpenMenuPlaceholder& operator=(const OpenMenuPlaceholder&) = delete;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /// @param menuWidget  The widget that owns this menu
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        OpenMenuPlaceholder(MenuWidgetBase* menuWidget);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the entire size that the widget is using
        ///
        /// @return Full size of the widget
        ///
        /// This OpenMenuPlaceholder widget will try to fit the entire screen to absorb all mouse events.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Vector2f getFullSize() const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the distance between the position where the widget is drawn and where the widget is placed
        ///
        /// @return Offset of the widget
        ///
        /// The offset equals -getPosition() for OpenMenuPlaceholder because it tries to fill the entire screen.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Vector2f getWidgetOffset() const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the mouse position (which is relative to the parent widget) lies on top of an open menu
        /// @return Is the mouse on top of an open menu from the menu bar?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD bool isMouseOnWidget(Vector2f pos) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draw the widget to a render target
        ///
        /// @param target Render target to draw to
        /// @param states Current render states
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(BackendRenderTarget& target, RenderStates states) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMouseButtonNoLongerDown() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rightMousePressed(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseMoved(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void keyPressed(const Event::KeyEvent& event) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Widget::Ptr clone() const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        MenuWidgetBase* m_menuWidget;
        bool m_mouseWasOnMenuWidget = true; // When a menu opens then the mouse will be on top of the menu bar
    };

    using MenuBarMenuPlaceholder TGUI_DEPRECATED("Use OpenMenuPlaceholder instead") = OpenMenuPlaceholder;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_MENU_WIDGET_BASE_HPP
