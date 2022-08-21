/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_TAB_CONTAINER_HPP
#define TGUI_TAB_CONTAINER_HPP


#include <TGUI/SubwidgetContainer.hpp>
#include <TGUI/TabAlignment.hpp>
#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Widgets/Panel.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief TabContainer widget
    ///
    /// A tab container widget is a control that allow user to switch between panels via tabs.
    /// If you are looking for something without holding panels then check out the Tabs class.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API TabContainer : public SubwidgetContainer
    {
    public:

        typedef std::shared_ptr<TabContainer> Ptr; //!< Shared widget pointer
        typedef std::shared_ptr<const TabContainer> ConstPtr; //!< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /// @see create
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TabContainer(const char* typeName = "TabContainer", bool initRenderer = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new tab container widget
        ///
        /// @return The new tab container
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static TabContainer::Ptr create(const Layout2d& size = { "100%", "100%" });


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another tab container
        ///
        /// @param tabContainer The other tab container
        ///
        /// @return The new tab container
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static TabContainer::Ptr copy(TabContainer::ConstPtr tabContainer);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer of tabs part of widget
        /// @return Temporary pointer to the renderer that may be shared with other widgets using the same renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TabsRenderer* getTabsSharedRenderer();
        const TabsRenderer* getTabsSharedRenderer() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer of tabs part of widget
        /// @return Temporary pointer to the renderer
        /// @warning After calling this function, the widget has its own copy of the renderer and it will no longer be shared.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TabsRenderer* getTabsRenderer();
        const TabsRenderer* getTabsRenderer() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the tab container
        ///
        /// @param size The new size of the tab container
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using SubwidgetContainer::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the height of tabs part of the widget
        ///
        /// @param height Height of tabs part of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTabsHeight(Layout height);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a new tab and corresponding panel
        ///
        /// @param name   Text for the tab
        /// @param select Should the tab be selected? Optional parameter, default value is true
        ///
        /// @return Newly created panel that corresponds to the new tab
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Panel::Ptr addTab(const String& name, bool select = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inserts a new tab and panel somewhere between the existing ones
        ///
        /// @param index  The index where the tab gets inserted. 0 means before the first tab, 1 means behind the first tab
        /// @param name   Text for the tab
        /// @param select Should the tab be selected? Optional parameter, default value is true
        ///
        /// @return Newly created panel that corresponds to the new tab
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Panel::Ptr insertTab(std::size_t index, const String& name, bool select = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a tab and its corresponding panel, given the text of the tab
        ///
        /// @param text  The text on the tab to remove
        ///
        /// @return Whether a tab was removed, false is returned when the text didn't match any tab
        ///
        /// @see removeTab(std::size_t)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeTab(const String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a tab and its corresponding panel with a given index
        ///
        /// @param index  The index of the tab to remove
        ///
        /// @return Whether a tab was removed, false is returned when the index was too high
        ///
        /// @see removeTab(String)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeTab(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets selection to panel
        ///
        /// @param index Index of panel to select
        /// @param genEvents To emit or not signals of selection change.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void select(std::size_t index, bool genEvents = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the number of holded panels
        ///
        /// @return Number of panels in the tab container
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::size_t getPanelCount() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns index of the specified panel
        ///
        /// @param ptr Panel to find
        /// @return Index of the specified panel or -1 if not found
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int getIndex(Panel::Ptr ptr);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the selected panel or nullptr
        ///
        /// @return Selected panel or nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Panel::Ptr getSelected();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns index of the selected panel
        ///
        /// @return Index of the selected panel or -1 for empty tab container
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int getSelectedIndex() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the panel with given index
        ///
        /// @param index Index of panel to get
        /// @return Panel with given index
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Panel::Ptr getPanel(int index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns internal tabs widget
        ///
        /// @return Internal tabs widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Tabs::Ptr getTabs();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the text of one of the tabs
        ///
        /// @param index The index of the tab. The first tab has index 0
        /// @return The text on the tab or an empty string when index was too high
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String getTabText(std::size_t index) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the text of one of the tabs
        ///
        /// @param index  The index of the tab to be changed. The first tab has index 0
        /// @param text   The new text of the tab that will be drawn on top of it
        ///
        /// @return True when text was successfully changed, false when index was too high
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool changeTabText(std::size_t index, const String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the alignment of the tabs
        ///
        /// The possible alignments can be found in the tgui::TabAlign::Alignment enum.
        /// The default alignment is tgui::TabAlign::Top.
        ///
        /// @param align  New tab alignment
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTabAlign(TabAlignment align);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the alignment of the tabs
        ///
        /// @return The current tab alignment
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TabAlignment getTabAlign() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the tab fixed size for fixed size alignment
        ///
        /// The tab fixed size is applied only on fixed size alignments, that can be found in the tgui::TabAlign::Alignment enum.
        ///
        /// @param align  New tab fixed size
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTabFixedSize(float fixedSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the tab fixed size for fixed size alignment, default is 100.0f
        ///
        /// @return The current tab fixed size
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getTabFixedSize() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Widget::Ptr clone() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the widget from a tree of nodes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Helper function that initializes the widget when constructing a new widget or loading one from a file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void init();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Helper function that sets position and size of the tabs according to the tab alignment
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void layoutTabs();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Helper function that sets position of a panel according to the tab alignment
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void layoutPanel(Panel::Ptr panel);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /// Selection of the tab container changed. Optional parameter: index of panel with selection
        SignalInt onSelectionChanged = { "SelectionChanged" };

        /// Selection of the tab container changing.
        /// Selection changing can be vetoed if to set second parameter to false in handler.
        SignalTyped2<int, bool*> onSelectionChanging = { "SelectionChanging" };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Stores the tab panels
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<Panel::Ptr> m_panels;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Stores the selected tab index
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int m_index = -1;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Stores the tabs
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Tabs::Ptr m_tabs;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Stores the tab alignment
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TabAlignment m_tabAlign = TabAlignment(TabAlign::Top);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Stores the tab fixed size for fixed size alignment, default is 100.0f
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float m_tabFixedSize = 100.0f;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TAB_CONTAINER_HPP
