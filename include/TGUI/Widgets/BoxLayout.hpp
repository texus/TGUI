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


#ifndef TGUI_BOX_LAYOUT_HPP
#define TGUI_BOX_LAYOUT_HPP

#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Renderers/BoxLayoutRenderer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Abstract class for box layout containers
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BoxLayout : public Group
    {
    public:

        using Ptr = std::shared_ptr<BoxLayout>; //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const BoxLayout>; //!< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /// @see create
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BoxLayout(const char* typeName, bool initRenderer);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer that may be shared with other widgets using the same renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD BoxLayoutRenderer* getSharedRenderer() override;
        TGUI_NODISCARD const BoxLayoutRenderer* getSharedRenderer() const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer
        /// @warning After calling this function, the widget has its own copy of the renderer and it will no longer be shared.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD BoxLayoutRenderer* getRenderer() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the group
        ///
        /// @param size  The new size of the group
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using Widget::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a widget at the end of the layout
        ///
        /// @param widget      Pointer to the widget you would like to add
        /// @param widgetName  An identifier to access to the widget later
        ///
        /// @warning Widgets should be named as if they are C++ variables, i.e. names must not include any whitespace, or most
        ///          symbols (e.g.: +, -, *, /, ., &), and should not start with a number. If you do not follow these rules,
        ///          layout expressions may give unexpected results. Alphanumeric characters and underscores are safe to use,
        ///          and widgets are permitted to have no name.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void add(const Widget::Ptr& widget, const String& widgetName = "") override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inserts a widget to the layout
        ///
        /// @param widget      Pointer to the widget you would like to add
        /// @param index       Index of the widget in the container
        /// @param widgetName  An identifier to access to the widget later
        ///
        /// If the index is too high, the widget will simply be added at the end of the list.
        ///
        /// @warning Widgets should be named as if they are C++ variables, i.e. names must not include any whitespace, or most
        ///          symbols (e.g.: +, -, *, /, ., &), and should not start with a number. If you do not follow these rules,
        ///          layout expressions may give unexpected results. Alphanumeric characters and underscores are safe to use,
        ///          and widgets are permitted to have no name.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void insert(std::size_t index, const Widget::Ptr& widget, const String& widgetName = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a single widget that was added to the container
        ///
        /// @param widget  Pointer to the widget to remove
        ///
        /// @return True if widget is removed, false if widget was not found
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool remove(const Widget::Ptr& widget) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a single widget that was added to the container
        ///
        /// @param index  Index in the layout of the widget to remove
        ///
        /// @return False if the index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool remove(std::size_t index);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the widget at the given index in the layout
        ///
        /// @param index  Index of the widget in the layout
        ///
        /// @return Widget of given index, or nullptr if index was too high
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Widget::Ptr get(std::size_t index) const;
        using Container::get;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the index of a widget in this container
        ///
        /// Widgets are drawn in the order of the list, so widgets with a higher index will appear after others.
        ///
        /// @param widget  Widget that is to be moved to a different index
        /// @param index   New index of the widget, corresponding to the widget position after the widget has been moved
        ///
        /// @return True when the index was changed, false if widget wasn't found in the container or index was too high
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool setWidgetIndex(const Widget::Ptr& widget, std::size_t index) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function called when one of the properties of the renderer is changed
        ///
        /// @param property  Name of the property that was changed
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rendererChanged(const String& property) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // @brief Repositions and resize the widgets
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void updateWidgets() = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        float m_spaceBetweenWidgetsCached = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BOX_LAYOUT_HPP
