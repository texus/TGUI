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


#ifndef TGUI_GUI_BUILDER_TOGGLE_BUTTON_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_TOGGLE_BUTTON_PROPERTIES_HPP

#include "ButtonProperties.hpp"

struct ToggleButtonProperties : ButtonProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto toggleButton = widget->cast<tgui::ToggleButton>();
        if (property == "Down")
            toggleButton->setDown(parseBoolean(value, false));
        else
            ButtonProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = ButtonProperties::initProperties(widget);
        auto toggleButton = widget->cast<tgui::ToggleButton>();
        pair.first["Down"] = {"Bool", tgui::Serializer::serialize(toggleButton->isDown())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_TOGGLE_BUTTON_PROPERTIES_HPP
