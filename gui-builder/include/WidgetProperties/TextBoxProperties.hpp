/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_GUI_BUILDER_TEXT_BOX_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_TEXT_BOX_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/TextBox.hpp>

struct TextBoxProperties : WidgetProperties
{
    // TODO: Scrollbar renderer

    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto textBox = std::dynamic_pointer_cast<tgui::TextBox>(widget);
        if (property == "Text")
            textBox->setText(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, value).getString());
        else if (property == "TextSize")
            textBox->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "MaximumCharacters")
            textBox->setMaximumCharacters(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "ReadOnly")
            textBox->setReadOnly(parseBoolean(value, false));
        else if (property == "VerticalScrollbarPolicy")
            textBox->setVerticalScrollbarPolicy(deserializeScrollbarPolicy(value));
        else if (property == "HorizontalScrollbarPolicy")
            textBox->setHorizontalScrollbarPolicy(deserializeScrollbarPolicy(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto textBox = std::dynamic_pointer_cast<tgui::TextBox>(widget);
        pair.first["Text"] = {"MultilineString", tgui::Serializer::serialize(textBox->getText())};
        pair.first["TextSize"] = {"UInt", tgui::to_string(textBox->getTextSize())};
        pair.first["MaximumCharacters"] = {"UInt", tgui::to_string(textBox->getMaximumCharacters())};
        pair.first["ReadOnly"] = {"Bool", tgui::Serializer::serialize(textBox->isReadOnly())};
        pair.first["VerticalScrollbarPolicy"] = {"Enum{Automatic,Always,Never}", serializeScrollbarPolicy(textBox->getVerticalScrollbarPolicy())};
        pair.first["HorizontalScrollbarPolicy"] = {"Enum{Automatic,Always,Never}", serializeScrollbarPolicy(textBox->getHorizontalScrollbarPolicy())};

        const auto renderer = textBox->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["Padding"] = {"Outline", renderer->getPadding().toString()};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pair.second["SelectedTextBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextBackgroundColor())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["CaretColor"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColor())};
        pair.second["CaretWidth"] = {"Float", tgui::to_string(renderer->getCaretWidth())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pair.second["ScrollbarWidth"] = {"Float", tgui::to_string(renderer->getScrollbarWidth())};
        return pair;
    }

private:

    static tgui::EditBox::Alignment deserializeAlignment(std::string value)
    {
        value = tgui::toLower(tgui::trim(value));
        if (value == "right")
            return tgui::EditBox::Alignment::Right;
        else if (value == "center")
            return tgui::EditBox::Alignment::Center;
        else
            return tgui::EditBox::Alignment::Left;
    }

    static std::string serializeAlignment(tgui::EditBox::Alignment alignment)
    {
        if (alignment == tgui::EditBox::Alignment::Center)
            return "Center";
        else if (alignment == tgui::EditBox::Alignment::Right)
            return "Right";
        else
            return "Left";
    }
};

#endif // TGUI_GUI_BUILDER_TEXT_BOX_PROPERTIES_HPP
