////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_ACCESSIBLE_ROLE_HPP
#define TGUI_ACCESSIBLE_ROLE_HPP

#include <TGUI/Config.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The role of a widget, which tells screen readers and other assistive technology what kind of control it is
    ///
    /// @since TGUI 1.14
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class AccessibleRole
    {
        Default,       //!< The role is determined by the type of the widget
        Window,        //!< The root of the gui, or a child window
        Pane,          //!< Container without a meaning of its own (e.g. a panel or a layout)
        Group,         //!< Container that groups related controls
        Button,        //!< Button that can be pressed
        ToggleButton,  //!< Button that stays down when pressed
        CheckBox,      //!< Check box
        RadioButton,   //!< Radio button
        Edit,          //!< Single-line text input
        MultiLineEdit, //!< Text input with multiple lines
        Text,          //!< Static text
        Image,         //!< Picture
        ComboBox,      //!< Combo box
        List,          //!< List of items
        ListItem,      //!< Item inside a list
        Tree,          //!< Tree view
        TreeItem,      //!< Item inside a tree
        Table,         //!< List with multiple columns
        TableRow,      //!< Row inside a table
        TableCell,     //!< Cell inside a table row
        Header,        //!< Header row of a table
        HeaderItem,    //!< Column header inside a header row
        TabList,       //!< Tabs
        TabItem,       //!< Tab inside a tab list
        Slider,        //!< Slider or knob
        ScrollBar,     //!< Scrollbar
        Spinner,       //!< Spin button or spin control
        ProgressBar,   //!< Progress bar
        MenuBar,       //!< Menu bar
        Menu,          //!< Open menu that contains menu items
        MenuItem,      //!< Item inside a menu bar or menu
        Separator,     //!< Separator line
        Custom         //!< Widget for which no better role exists
    };
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_ACCESSIBLE_ROLE_HPP
