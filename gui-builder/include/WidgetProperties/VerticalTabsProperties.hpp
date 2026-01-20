/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_GUI_BUILDER_VERTICAL_TABS_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_VERTICAL_TABS_PROPERTIES_HPP

#include "TabsBaseProperties.hpp"

struct VerticalTabsProperties : public TabsBaseProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto tabs = widget->cast<tgui::VerticalTabs>();
        if (property == "TabHeight")
            tabs->setTabHeight(value.toFloat());
        else
            TabsBaseProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = TabsBaseProperties::initProperties(widget);
        auto tabs = widget->cast<tgui::VerticalTabs>();

        pair.first["TabHeight"] = {"Float", tgui::String::fromNumber(tabs->getTabHeight())};

        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_VERTICAL_TABS_PROPERTIES_HPP
