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


#ifndef TGUI_GUI_BUILDER_CHAT_BOX_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_CHAT_BOX_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct ChatBoxProperties : WidgetProperties
{
    // TODO: Scrollbar renderer

    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto chatBox = widget->cast<tgui::ChatBox>();
        if (property == "TextSize")
            chatBox->setTextSize(value.toUInt());
        else if (property == "TextColor")
            chatBox->setTextColor(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Color, value).getColor());
        else if (property == "TextStyle")
            chatBox->setTextStyle(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::TextStyle, value).getTextStyle());
        else if (property == "LineLimit")
            chatBox->setLineLimit(value.toUInt());
        else if (property == "LinesStartFromTop")
            chatBox->setLinesStartFromTop(parseBoolean(value, true));
        else if (property == "NewLinesBelowOthers")
            chatBox->setNewLinesBelowOthers(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto chatBox = widget->cast<tgui::ChatBox>();
        pair.first["TextSize"] = {"UInt", tgui::String::fromNumber(chatBox->getTextSize())};
        pair.first["TextColor"] = {"Color", tgui::Serializer::serialize(chatBox->getTextColor())};
        pair.first["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(chatBox->getTextStyle())};
        pair.first["LineLimit"] = {"UInt", tgui::String::fromNumber(chatBox->getLineLimit())};
        pair.first["LinesStartFromTop"] = {"Bool", tgui::Serializer::serialize(chatBox->getLinesStartFromTop())};
        pair.first["NewLinesBelowOthers"] = {"Bool", tgui::Serializer::serialize(chatBox->getNewLinesBelowOthers())};

        const auto renderer = chatBox->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["Padding"] = {"Outline", tgui::Serializer::serialize(renderer->getPadding())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pair.second["ScrollbarWidth"] = {"Float", tgui::String::fromNumber(renderer->getScrollbarWidth())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_CHAT_BOX_PROPERTIES_HPP
