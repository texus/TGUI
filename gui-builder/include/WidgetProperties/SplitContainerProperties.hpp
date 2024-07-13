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


#ifndef TGUI_GUI_BUILDER_SPLIT_CONTAINER_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_SPLIT_CONTAINER_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct SplitContainerProperties : WidgetProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto splitContainer = widget->cast<tgui::SplitContainer>();
        if (property == "Orientation")
            splitContainer->setOrientation(deserializeOrientation(value));
        else if (property == "MinValidSplitterOffset")
            splitContainer->setMinValidSplitterOffset(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber());
        else if (property == "MaxValidSplitterOffset")
            splitContainer->setMaxValidSplitterOffset(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber());
        else if (property == "SplitterOffset")
            splitContainer->setSplitterOffset(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber());
        else if (property == "SplitterWidth")
            splitContainer->setSplitterWidth(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber());
        else if (property == "MinimumGrabWidth")
            splitContainer->setMinimumGrabWidth(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, value).getNumber());
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto splitContainer = widget->cast<tgui::SplitContainer>();
        pair.first["Orientation"] = {"Enum{Vertical,Horizontal}", serializeOrientation(splitContainer->getOrientation())};
        pair.first["MinValidSplitterOffset"] = {"Float", tgui::String::fromNumber(splitContainer->getMinValidSplitterOffset())};
        pair.first["MaxValidSplitterOffset"] = {"Float", tgui::String::fromNumber(splitContainer->getMaxValidSplitterOffset())};
        pair.first["SplitterOffset"] = {"Float", tgui::String::fromNumber(splitContainer->getSplitterOffset())};
        pair.first["SplitterWidth"] = {"Float", tgui::String::fromNumber(splitContainer->getSplitterWidth())};
        pair.first["MinimumGrabWidth"] = {"Float", tgui::String::fromNumber(splitContainer->getSplitterWidth())};

        const auto renderer = splitContainer->getSharedRenderer();
        pair.second["SplitterColor"] = {"Color", tgui::Serializer::serialize(renderer->getSplitterColor())};
        pair.second["SplitterColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSplitterColorHover())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_SPLIT_CONTAINER_PROPERTIES_HPP
