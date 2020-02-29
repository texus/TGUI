/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_GUI_BUILDER_SCROLLABLE_PANEL_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_SCROLLABLE_PANEL_PROPERTIES_HPP

#include "PanelProperties.hpp"
#include <TGUI/Widgets/ScrollablePanel.hpp>

struct ScrollablePanelProperties : PanelProperties
{
    // TODO: Scrollbar renderer

    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const sf::String& value) const override
    {
        auto panel = std::dynamic_pointer_cast<tgui::ScrollablePanel>(widget);
        if (property == "VerticalScrollbarPolicy")
            panel->setVerticalScrollbarPolicy(deserializeScrollbarPolicy(value));
        else if (property == "HorizontalScrollbarPolicy")
            panel->setHorizontalScrollbarPolicy(deserializeScrollbarPolicy(value));
        else if (property == "VerticalScrollAmount")
            panel->setVerticalScrollAmount(static_cast<unsigned int>(tgui::strToInt(value.toAnsiString())));
        else if (property == "HorizontalScrollAmount")
            panel->setHorizontalScrollAmount(static_cast<unsigned int>(tgui::strToInt(value.toAnsiString())));
        else if (property == "ContentWidth")
            panel->setContentSize({tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber(), panel->getContentSize().y});
        else if (property == "ContentHeight")
            panel->setContentSize({panel->getContentSize().x, tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber()});
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = PanelProperties::initProperties(widget);
        auto panel = std::dynamic_pointer_cast<tgui::ScrollablePanel>(widget);
        pair.first["VerticalScrollbarPolicy"] = {"Enum{Automatic,Always,Never}", serializeScrollbarPolicy(panel->getVerticalScrollbarPolicy())};
        pair.first["HorizontalScrollbarPolicy"] = {"Enum{Automatic,Always,Never}", serializeScrollbarPolicy(panel->getHorizontalScrollbarPolicy())};
        pair.first["VerticalScrollAmount"] = {"UInt", tgui::to_string(panel->getVerticalScrollAmount())};
        pair.first["HorizontalScrollAmount"] = {"UInt", tgui::to_string(panel->getHorizontalScrollAmount())};
        pair.first["ContentWidth"] = {"Float", tgui::to_string(panel->getContentSize().x)};
        pair.first["ContentHeight"] = {"Float", tgui::to_string(panel->getContentSize().y)};

        const auto renderer = panel->getSharedRenderer();
        pair.second["ScrollbarWidth"] = {"Float", tgui::to_string(renderer->getScrollbarWidth())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_PANEL_PROPERTIES_HPP
