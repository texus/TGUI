/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_MENU_BAR_HPP
#define TGUI_MENU_BAR_HPP


#include <TGUI/Widget.hpp>
#include <TGUI/Renderers/MenuBarRenderer.hpp>
#include <TGUI/Text.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Menu bar widget
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API MenuBar : public Widget
    {
    public:

        typedef std::shared_ptr<MenuBar> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const MenuBar> ConstPtr; ///< Shared constant widget pointer

    #ifndef TGUI_REMOVE_DEPRECATED_CODE
        /// @brief Used for return value of getAllMenus
        /// @deprecated The getMenuList should be used instead of getAllMenus
        struct GetAllMenusElement
        {
            sf::String text;
            bool enabled;
            std::vector<std::unique_ptr<GetAllMenusElement>> menuItems;
        };
    #endif

        /// @brief Used for return value of getMenuList
        struct GetMenuListElement
        {
            sf::String text;
            bool enabled;
            std::vector<GetMenuListElement> menuItems;
        };

        /// @internal
        struct Menu
        {
            Text text;
            bool enabled = true;
            int selectedMenuItem = -1;
            std::vector<Menu> menuItems;
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MenuBar();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new menu bar widget
        ///
        /// @return The new menu bar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static MenuBar::Ptr create();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another menu bar
        ///
        /// @param menuBar  The other menu bar
        ///
        /// @return The new menu bar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static MenuBar::Ptr copy(MenuBar::ConstPtr menuBar);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer that may be shared with other widgets using the same renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MenuBarRenderer* getSharedRenderer();
        const MenuBarRenderer* getSharedRenderer() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer
        /// @warning After calling this function, the widget has its own copy of the renderer and it will no longer be shared.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MenuBarRenderer* getRenderer();
        const MenuBarRenderer* getRenderer() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler to the "MenuItemClicked" callback that will only be called when a specific
        ///        menu item was clicked.
        ///
        /// @param menu     Menu containing the menu item
        /// @param menuItem Menu item which should trigger the signal
        /// @param handler  Callback function to call
        /// @param args     Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        ///
        /// The hierarchy does not need to exist yet and the signal will still exist when removing and readding the menu items.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args>
        unsigned int connectMenuItem(const sf::String& menu, const sf::String& menuItem, Func&& handler, const Args&... args)
        {
            return connectMenuItem({menu, menuItem}, std::forward<Func>(handler), args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler to the "MenuItemClicked" callback that will only be called when a specific
        ///        menu item was clicked.
        ///
        /// @param hierarchy Hierarchy of the menu items, starting with the menu and ending with menu item that should trigger
        ///                  the signal when pressed
        /// @param handler   Callback function to call
        /// @param args      Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        ///
        /// The hierarchy does not need to exist yet and the signal will still exist when removing and readding the menu items.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args>
        unsigned int connectMenuItem(const std::vector<sf::String>& hierarchy, Func&& handler, const Args&... args)
        {
#if defined(__cpp_lib_invoke) && (__cpp_lib_invoke >= 201411L)
            return connect("MenuItemClicked",
                [=](const std::vector<sf::String>& clickedMenuItem)
                {
                    if (clickedMenuItem == hierarchy)
                        std::invoke(handler, args...);
                }
            );
#else
            return connect("MenuItemClicked",
                [f=std::function<void(const Args&...)>(handler),args...,hierarchy](const std::vector<sf::String>& clickedMenuItem)
                {
                    if (clickedMenuItem == hierarchy)
                        f(args...);
                }
            );
#endif
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the menu bar
        ///
        /// @param size  The new size of the menu bar
        ///
        /// By default, the menu bar has the same width as the window and the height is 20 pixels.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using Widget::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Enables or disables the widget
        /// @param enabled  Is the widget enabled?
        ///
        /// The disabled widget will no longer receive events and thus no longer send callbacks.
        /// All widgets are enabled by default.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setEnabled(bool enabled) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a new menu
        ///
        /// @param text  The text written on the menu
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void addMenu(const sf::String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a new menu item to the last added menu
        ///
        /// @param text  The text written on this menu item
        ///
        /// @return True when the item was added, false when the menu bar doesn't contain any menus yet
        ///
        /// @code
        /// menuBar->addMenu("File");
        /// menuBar->addMenuItem("Load");
        /// menuBar->addMenuItem("Save");
        /// menuBar->addMenu("Edit");
        /// menuBar->addMenuItem("Undo");
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool addMenuItem(const sf::String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a new menu item to an existing menu
        ///
        /// @param menu  The name of the menu to which the menu item will be added
        /// @param text  The text written on this menu item
        ///
        /// @return True when the item was added, false when menu was not found
        ///
        /// @code
        /// menuBar->addMenu("File");
        /// menuBar->addMenu("Edit");
        /// menuBar->addMenuItem("File", "Load");
        /// menuBar->addMenuItem("File", "Save");
        /// menuBar->addMenuItem("Edit", "Undo");
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool addMenuItem(const sf::String& menu, const sf::String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a new menu item (or sub menu item)
        ///
        /// @param hierarchy     Hierarchy of the menu items, starting with the menu and ending with menu item to be added
        /// @param createParents Should the hierarchy be created if it did not exist yet?
        ///
        /// @return True when the item was added, false when createParents was false and the parents hierarchy does not exist
        ///         or if hierarchy does not contain at least 2 elements.
        ///
        /// @code
        /// menuBar->addMenuItem({"File", "Save"});
        /// menuBar->addMenuItem({"View", "Messages", "Tags", "Important"});
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool addMenuItem(const std::vector<sf::String>& hierarchy, bool createParents = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes all menus
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllMenus();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a menu
        ///
        /// Any menu items that belong to this menu will be removed as well.
        ///
        /// @param menu  The name of the menu to remove
        ///
        /// @return True when the menu was removed, false when menu was not found
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeMenu(const sf::String& menu);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a menu item
        ///
        /// @param menu      The name of the menu in which the menu item is located
        /// @param menuItem  The name of the menu item to remove
        ///
        /// @return True when the item was removed, false when menu or menuItem was not found
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeMenuItem(const sf::String& menu, const sf::String& menuItem);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a menu item (or sub menu item)
        ///
        /// @param hierarchy  Hierarchy of the menu item, starting with the menu and ending with menu item to be deleted
        /// @param removeParentsWhenEmpty  Also delete the parent of the deleted menu item if it has no other children
        ///
        /// @return True when the menu item existed and was removed, false when hierarchy was incorrect
        ///
        /// @code
        /// menuBar->removeMenuItem({"File", "Save"});
        /// menuBar->removeMenuItem({"View", "Messages", "Tags", "Important"});
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeMenuItem(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes all menu items from a menu
        ///
        /// @param menu      The name of the menu for which all menu items should be removed
        ///
        /// @return True when the menu existed and its children were removed, false when menu was not found
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeMenuItems(const sf::String& menu);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a all menu items below a (sub) menu
        ///
        /// @param hierarchy Hierarchy of the menu item, starting with the menu and ending with the sub menu containing the items
        ///
        /// @return True when the menu item existed and its children were removed, false when hierarchy was incorrect
        ///
        /// @code
        /// menuBar->removeSubMenuItems({"File", "Recent files"});
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeSubMenuItems(const std::vector<sf::String>& hierarchy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Enable or disable an entire menu
        /// @param menu     The name of the menu to enable or disable
        /// @param enabled  Should the menu be enabled or disabled?
        /// @return True when the menu exists, false when menu was not found
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setMenuEnabled(const sf::String& menu, bool enabled);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Check if an entire menu is enabled or disabled
        /// @param menu  The name of the menu to check
        /// @return True if the menu is enabled, false if it was disabled or when the menu did not exist
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool getMenuEnabled(const sf::String& menu) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Enable or disable a menu item
        /// @param menu      The name of the menu in which the menu item is located
        /// @param menuItem  The name of the menu item to enable or disable
        /// @param enabled   Should the menu item be enabled or disabled?
        /// @return True when the menu item exists, false when menu or menuItem was not found
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setMenuItemEnabled(const sf::String& menu, const sf::String& menuItem, bool enabled);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Enable or disable a menu item
        /// @param hierarchy  Hierarchy of menu items, starting with the menu and ending with the menu item to enable/disable
        /// @param enabled    Should the menu item be enabled or disabled?
        /// @return True when the menu item exists, false when hierarchy was incorrect
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setMenuItemEnabled(const std::vector<sf::String>& hierarchy, bool enabled);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Check if a menu item is enabled or disabled
        /// @param menu      The name of the menu in which the menu item is located
        /// @param menuItem  The name of the menu item to check
        /// @return True if the menu item is enabled, false if it was disabled or when the menu or menuItem did not exist
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool getMenuItemEnabled(const sf::String& menu, const sf::String& menuItem) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Check if a menu item is enabled or disabled
        /// @param hierarchy  Hierarchy of menu items, starting with the menu and ending with the menu item to check
        /// @return True if the menu item is enabled, false if it was disabled or when the hierarchy was incorrect
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool getMenuItemEnabled(const std::vector<sf::String>& hierarchy) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the character size of the text
        /// @param size  The new size of the text.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextSize(unsigned int size) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the minimum width of the submenus
        ///
        /// When a submenu is displayed, the width will be either this or the width of the longest text in the submenu.
        /// The default minimum width is 125 pixels.
        ///
        /// @param minimumWidth  minimum width of the submenus
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMinimumSubMenuWidth(float minimumWidth);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the distance between the text and the side of the menu item
        ///
        /// @return minimum width of the submenus
        ///
        /// @see setMinimumSubMenuWidth
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getMinimumSubMenuWidth() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes whether the menus open above or below the menu bar
        ///
        /// @param invertDirection  Should the menus open above the bar instead of below like it does by default?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setInvertedMenuDirection(bool invertDirection);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the menus open above or below the menu bar
        ///
        /// @return Do menus open above the bar instead of below like it does by default?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool getInvertedMenuDirection() const;


    #ifndef TGUI_REMOVE_DEPRECATED_CODE
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Returns a copy of all the menus and their menu items
        /// @return Map of menus and their items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("This function doesn't work with submenus, use getMenuList instead")
        std::vector<std::pair<sf::String, std::vector<sf::String>>> getMenus() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Returns the menus and their menu items, including submenus
        /// @return List of menus
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("This function is deprecated, use getMenuList instead")
        std::vector<std::unique_ptr<GetAllMenusElement>> getAllMenus() const;
    #endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Returns the menus and their menu items, including submenus
        /// @return List of menus
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<GetMenuListElement> getMenuList() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Closes the open menu when one of the menus is open
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void closeMenu();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the mouse position (which is relative to the parent widget) lies on top of the widget
        ///
        /// @return Is the mouse on top of the widget?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool mouseOnWidget(Vector2f pos) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMousePressed(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMouseReleased(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseMoved(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMouseButtonNoLongerDown() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // This function is called when the mouse leaves the widget. If requested, a callback will be send.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseLeftWidget() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draw the widget to a render target
        ///
        /// @param target Render target to draw to
        /// @param states Current render states
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves a signal based on its name
        ///
        /// @param signalName  Name of the signal
        ///
        /// @return Signal that corresponds to the name
        ///
        /// @throw Exception when the name does not match any signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal& getSignal(std::string signalName) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function called when one of the properties of the renderer is changed
        ///
        /// @param property  Lowercase name of the property that was changed
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rendererChanged(const std::string& property) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Saves the widget as a tree node in order to save it to a file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::unique_ptr<DataIO::Node> save(SavingRenderersMap& renderers) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the widget from a tree of nodes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Widget::Ptr clone() const override
        {
            return std::make_shared<MenuBar>(*this);
        }

        /// @internal
        /// Helper function to create a new menu or menu item
        void createMenu(std::vector<Menu>& menus, const sf::String& text);

        /// @internal
        /// Recursively search for the menu containing the menu item specified in the hierarchy, creating the hierarchy if requested.
        /// The initial call to this function must pass "parentIndex = 0" and "menus = m_menus".
        Menu* findMenu(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, std::vector<Menu>& menus, bool createParents);

        /// @internal
        /// Recursively search for the menu containing the menu item specified in the hierarchy.
        /// The initial call to this function must pass "parentIndex = 0" and "menus = m_menus".
        const Menu* findMenu(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, const std::vector<Menu>& menus) const;

        /// @internal
        /// Search for the menu item specified in the hierarchy.
        const Menu* findMenuItem(const std::vector<sf::String>& hierarchy) const;

        /// @internal
        /// Helper function to load the menus when the menu bar is being loaded from a text file
        void loadMenus(const std::unique_ptr<DataIO::Node>& node, std::vector<Menu>& menus);

        /// @internal
        /// Closes the open menu and its submenus
        void closeSubMenus(std::vector<Menu>& menus, int& selectedMenu);

        /// @internal
        void deselectBottomItem();

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
        float calculateMenuWidth(const Menu& menu) const;

        /// @internal
        Vector2f calculateSubmenuOffset(const Menu& menu, float globalLeftPos, float menuWidth, float subMenuWidth, bool& openSubMenuToRight) const;

        /// @internal
        bool isMouseOnTopOfMenu(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, const Menu& menu, float menuWidth) const;

        /// @internal
        bool findMenuItemBelowMouse(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, Menu& menu, float menuWidth, Menu** resultMenu, int* resultSelectedMenuItem);

        /// @internal
        /// Draw the backgrounds and text of the menu names on top of the bar itself
        void drawMenusOnBar(sf::RenderTarget& target, sf::RenderStates states, Sprite& backgroundSprite) const;

        /// @internal
        /// Draw an open menu and recusively draw submenus when open
        void drawMenu(sf::RenderTarget& target, sf::RenderStates states, const Menu& menu, float menuWidth, Sprite& backgroundSprite, float globalLeftPos, bool openSubMenuToRight) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /// One of the menu items was clicked.
        /// Optional parameters:
        ///     - The text of the clicked menu item
        ///     - List containing both the name of the menu and the menu item that was clicked
        SignalItemHierarchy onMenuItemClick = {"MenuItemClicked"};


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::vector<Menu> m_menus;

        int m_visibleMenu = -1;

        float m_minimumSubMenuWidth = 125;

        bool m_invertedMenuDirection = false;

        Sprite m_spriteBackground;
        Sprite m_spriteItemBackground;
        Sprite m_spriteSelectedItemBackground;

        // Cached renderer properties
        Color m_backgroundColorCached;
        Color m_selectedBackgroundColorCached;
        Color m_textColorCached;
        Color m_selectedTextColorCached;
        Color m_textColorDisabledCached;
        float m_distanceToSideCached = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_MENU_BAR_HPP
