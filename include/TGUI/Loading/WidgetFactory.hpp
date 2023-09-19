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


#ifndef TGUI_WIDGET_FACTORY_HPP
#define TGUI_WIDGET_FACTORY_HPP


#include <TGUI/Widget.hpp>
#include <TGUI/Loading/DataIO.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <functional>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API WidgetFactory
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the construct function that will be used to create a widget when loading it
        ///
        /// @param type         Type of the widget
        /// @param constructor  Function used to construct the widget (all TGUI widgets use std::make_shared<WidgetType>)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setConstructFunction(const String& type, std::function<Widget::Ptr()> constructor);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves the construct function that is used to create a widget when loading it
        ///
        /// @param type  Type of the widget
        ///
        /// @return Function used to construct the widget (all TGUI widgets use std::make_shared<WidgetType>)
        ///
        /// @warning A nullptr is returned if called with a type that isn't in the list returned by getWidgetTypes()
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static std::function<Widget::Ptr()> getConstructFunction(const String& type);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a list of all widget types for which a construct function exists
        ///
        /// @return Widget type list
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static std::vector<String> getWidgetTypes();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        static std::map<String, std::function<Widget::Ptr()>> m_constructFunctions;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_WIDGET_FACTORY_HPP
