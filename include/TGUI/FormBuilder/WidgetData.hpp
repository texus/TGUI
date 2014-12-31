/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_FORM_BUILDER_WIDGET_DATA_HPP
#define TGUI_FORM_BUILDER_WIDGET_DATA_HPP

#include <TGUI/TGUI.hpp>

struct WidgetData
{
    struct Property
    {
        Property() {}
        Property(const std::string& t) : type(t) {}
        Property(const std::string& t, const std::string& v) : type(t), value(v) {}

        std::string type;
        std::string value;
    };

    WidgetData() : widget(tgui::Picture::Ptr(nullptr)) {}

    WidgetData(const tgui::Widget::Ptr& ptr) :
        widget(ptr)
    {
        widget->hide();
    }

    ~WidgetData()
    {
        if ((widget != nullptr) && (widget->getParent() != nullptr))
            widget->getParent()->remove(widget);
    }

    tgui::Widget::Ptr widget;
    std::list< std::pair<std::string, Property> > properties;
};


#endif // TGUI_FORM_BUILDER_WIDGET_DATA_HPP

