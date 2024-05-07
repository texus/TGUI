/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_PANEL_LIST_BOX_HPP
#define TGUI_PANEL_LIST_BOX_HPP

#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Renderers/PanelListBoxRenderer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief List of panels, which can be flexible changed
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API PanelListBox : public ScrollablePanel
    {
    public:
        using Ptr = std::shared_ptr<PanelListBox>; //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const PanelListBox>; //!< Shared constant widget pointer

        static constexpr const char StaticWidgetType[] = "PanelListBox"; //!< Type name of the widget


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /// @see create
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        PanelListBox(const char* typeName = StaticWidgetType, bool initRenderer = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new panel list box widget
        /// @return The new panel list box
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Ptr create();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another panel list box
        /// @param panelListBox  The other list box
        /// @return The new list box
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static Ptr copy(const ConstPtr& panelListBox);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer that may be shared with other widgets using the same renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD PanelListBoxRenderer* getSharedRenderer() override;
        TGUI_NODISCARD const PanelListBoxRenderer* getSharedRenderer() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer
        /// @warning After calling this function, the widget has its own copy of the renderer and it will no longer be shared.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD PanelListBoxRenderer* getRenderer() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the panel list box
        /// @param size  The new size of the list box
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using ScrollablePanel::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds an item to the list
        /// @param id Optional unique id given to this item for the purpose to later identifying this item
        /// @param index Optional index at which new item will be added. If not provided, item will be added at last position
        /// @return Pointer to newly created panel
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Panel::Ptr addItem(const String& id = {}, int index = -1);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Get panel template from which new elements are created
        ///
        /// @return Pointer to panel template
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Panel::Ptr getPanelTemplate() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the width of the items in the list box
        ///
        /// @return The item width
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Layout getItemsWidth() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the height of the items in the list box
        ///
        /// @param height  The height of a single item in the list
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setItemsHeight(const Layout& height);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the height of the items in the list box
        ///
        /// @return The item height
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Layout getItemsHeight() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Selects an item in the panel list box
        ///
        /// @param panelPtr  Pointer to the item you want select
        ///
        /// In case of nullptr, nothing will be selected.
        ///
        /// @return
        ///         - true on success
        ///         - false when none of the items matches the pointer
        ///
        /// @see setSelectedItemById
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setSelectedItem(const Panel::Ptr& panelPtr);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Selects an item in the panel list box
        ///
        /// @param id  Unique id passed to addItem
        ///
        /// In case the id would not be unique, the first item with that id will be selected.
        ///
        /// @return
        ///         - true on success
        ///         - false when none of the items has the given id
        ///
        /// @see setSelectedItem
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setSelectedItemById(const String& id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Selects an item in the panel list box
        ///
        /// @param index  Index of the item in the list box
        ///
        /// @return
        ///         - true on success
        ///         - false when the index was too high
        ///
        /// @see setSelectedItem
        /// @see setSelectedItemById
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setSelectedItemByIndex(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Deselects the selected item
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void deselectItem();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the item from the panel list with the given pointer
        ///
        /// @param panelPtr  The item to remove
        ///
        /// In case of nullptr, nothing will be removed.
        ///
        /// @return
        ///        - true when the item was removed
        ///        - false when the name did not match any item
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeItem(const Panel::Ptr& panelPtr);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the item that were added with the given id
        ///
        /// @param id  Id that was given to the addItem function
        ///
        /// In case the id is not unique, only the first item with that id will be removed.
        ///
        /// @return
        ///        - true when the item was removed
        ///        - false when there was no item with the given id
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeItemById(const String& id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the item from the panel list box
        ///
        /// @param index  Index of the item in the panel list box
        ///
        /// @return
        ///        - true when the item was removed
        ///        - false when the index was too high
        ///
        /// @see removeItem
        /// @see removeItemById
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeItemByIndex(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes all items from the list
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllItems();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the item name of the item with the given id
        ///
        /// @param id  The id of the item that was given to it when it was added
        ///
        /// In case the id is not unique, the first item with that id will be returned.
        ///
        /// @return The requested item, or an empty string when no item matches the id
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Panel::Ptr getItemById(const String& id) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the item name of the item at the given index
        ///
        /// @param index  The index of the item to return
        ///
        /// @return The requested item, or an empty string when the index was too high
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Panel::Ptr getItemByIndex(std::size_t index) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the index of the item with the given id
        ///
        /// @param id  The id of the item that was given to it when it was added
        ///
        /// In case the id is not unique, the index of the first item with that id will be returned.
        ///
        /// @return The index of the item that matched the id, or -1 when no item matches the id
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int getIndexById(const String& id) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the index of the given item
        ///
        /// @param panelPtr  Pointer to the item to find
        ///
        /// @return The id of the requested item, or an empty string when the corresponding item was not found
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int getIndexByItem(const Panel::Ptr& panelPtr) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the id of the item at the given index
        ///
        /// @param index  The index of the item to find
        ///
        /// @return The id of the requested item, or an empty string when the index was too high
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String getIdByIndex(std::size_t index) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the currently selected item
        ///
        /// @return The selected item
        ///         When no item was selected then this function will return an empty string.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Panel::Ptr getSelectedItem() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the id of the selected item
        ///
        /// @return The id of the selected item, which was the optional id passed to the addItem function.
        ///         When no item was selected then this function returns an empty string
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD String getSelectedItemId() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the index of the selected item
        ///
        /// @return The index of the selected item, or -1 when no item was selected
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD int getSelectedItemIndex() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the amount of items in the panel list box
        ///
        /// @return Number of items inside the list box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::size_t getItemCount() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a copy of the items in the list box
        ///
        /// @return items
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::vector<Panel::Ptr> getItems() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a vector of stored items ids
        ///
        /// @return items ids
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::vector<String> getItemIds() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the maximum items that the list box can contain
        ///
        /// @param maximumItems  The maximum items inside the list box.
        ///                      When the maximum is set to 0 then the limit will be disabled
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMaximumItems(std::size_t maximumItems = 0);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the maximum items that the list box can contain
        ///
        /// @return The maximum items inside the list box.
        ///         If the function returns 0 then there is no limit
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::size_t getMaximumItems() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the panel list box contains the given item
        ///
        /// @param panelPtr  The item to search for
        ///
        /// @return Does the list box contain the item?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool contains(const Panel::Ptr& panelPtr) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the panel list box contains an item with the given id
        ///
        /// @param id  The id of the item to search for
        ///
        /// @return Does the list box contain the id?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool containsId(const String& id) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseMoved(Vector2f pos) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseNoLongerOnWidget() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool leftMousePressed(Vector2f pos) override;


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
        TGUI_NODISCARD Signal& getSignal(String signalName) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function called when one of the properties of the renderer is changed
        ///
        /// @param property  Name of the property that was changed
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rendererChanged(const String& property) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Saves the widget as a tree node in order to save it to a file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::unique_ptr<DataIO::Node> save(SavingRenderersMap& renderers) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the widget from a tree of nodes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the position of the items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateItemsPositions() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the size of the items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateItemsSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Returns all items height
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD float getAllItemsHeight() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Returns height of the items
        //
        /// @param index  Index of item up to which height should be calculated
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD float getItemsHeightUpToIndex(std::size_t index) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update which item is selected
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateSelectedItem(int item);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update on which item the mouse is standing
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateHoveringItem(int item);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Update the colors and style of the selected and hovered items
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateSelectedAndHoveringItemColorsAndStyle() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Clear item style
        //
        /// @param item  Index of item of which style should be cleared
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void clearItemStyle(int item) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Clear all items style
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void clearAllItemsStyle() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Widget::Ptr clone() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        SignalPanelListBoxItem onItemSelect = {"ItemSelected"};  //!< An item was selected in the list box. Optional parameter: selected item or its index


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        struct Item
        {
            Panel::Ptr panel;
            String id;
        };

        std::vector<Item> m_items;
        std::size_t m_maxItems;
        Panel::Ptr m_panelTemplate;

        int m_selectedItem;
        int m_hoveringItem;

        // Cached renderer properties
        Color m_itemsBackgroundColorCached;
        Color m_itemsBackgroundColorHoverCached;
        Color m_selectedItemsBackgroundColorCached;
        Color m_selectedItemsBackgroundColorHoverCached;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_PANEL_LIST_BOX_HPP
