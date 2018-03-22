/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_GUI_BUILDER_LABEL_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_LABEL_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/Label.hpp>

struct LabelProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto label = std::dynamic_pointer_cast<tgui::Label>(widget);
        if (property == "Text")
            label->setText(value);
        else if (property == "TextSize")
            label->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "HorizontalAlignment")
            label->setHorizontalAlignment(deserializeHorizontalAlignment(value));
        else if (property == "VerticalAlignment")
            label->setVerticalAlignment(deserializeVerticalAlignment(value));
        else if (property == "AutoSize")
            label->setAutoSize(parseBoolean(value, true));
        else if (property == "MaximumTextWidth")
            label->setMaximumTextWidth(tgui::stof(value));
        else if (property == "IgnoreMouseEvents")
            label->ignoreMouseEvents(parseBoolean(value, false));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto label = std::dynamic_pointer_cast<tgui::Label>(widget);
        pair.first["Text"] = {"String", label->getText()};
        pair.first["TextSize"] = {"UInt", tgui::to_string(label->getTextSize())};
        pair.first["HorizontalAlignment"] = {"Enum{Left,Center,Right}", serializeHorizontalAlignment(label->getHorizontalAlignment())};
        pair.first["VerticalAlignment"] = {"Enum{Top,Center,Bottom}", serializeVerticalAlignment(label->getVerticalAlignment())};
        pair.first["AutoSize"] = {"Bool", tgui::Serializer::serialize(label->getAutoSize())};
        pair.first["MaximumTextWidth"] = {"Float", tgui::to_string(label->getMaximumTextWidth())};
        pair.first["IgnoreMouseEvents"] = {"Bool", tgui::Serializer::serialize(label->isIgnoringMouseEvents())};

        const auto renderer = label->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["Padding"] = {"Outline", renderer->getPadding().toString()};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        return pair;
    }

private:

    static tgui::Label::HorizontalAlignment deserializeHorizontalAlignment(std::string value)
    {
        value = tgui::toLower(tgui::trim(value));
        if (value == "right")
            return tgui::Label::HorizontalAlignment::Right;
        else if (value == "center")
            return tgui::Label::HorizontalAlignment::Center;
        else
            return tgui::Label::HorizontalAlignment::Left;
    }

    static tgui::Label::VerticalAlignment deserializeVerticalAlignment(std::string value)
    {
        value = tgui::toLower(tgui::trim(value));
        if (value == "bottom")
            return tgui::Label::VerticalAlignment::Bottom;
        else if (value == "center")
            return tgui::Label::VerticalAlignment::Center;
        else
            return tgui::Label::VerticalAlignment::Top;
    }

    static std::string serializeHorizontalAlignment(tgui::Label::HorizontalAlignment alignment)
    {
        if (alignment == tgui::Label::HorizontalAlignment::Center)
            return "Center";
        else if (alignment == tgui::Label::HorizontalAlignment::Right)
            return "Right";
        else
            return "Left";
    }

    static std::string serializeVerticalAlignment(tgui::Label::VerticalAlignment alignment)
    {
        if (alignment == tgui::Label::VerticalAlignment::Center)
            return "Center";
        else if (alignment == tgui::Label::VerticalAlignment::Bottom)
            return "Bottom";
        else
            return "Top";
    }
};

#endif // TGUI_GUI_BUILDER_LABEL_PROPERTIES_HPP
