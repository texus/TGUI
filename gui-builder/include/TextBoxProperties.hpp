/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
            textBox->setText(value);
        else if (property == "TextSize")
            textBox->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "MaximumCharacters")
            textBox->setMaximumCharacters(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "ReadOnly")
            textBox->setReadOnly(parseBoolean(value, false));
        else if (property == "VerticalScrollbarPresent")
            textBox->setVerticalScrollbarPresent(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto textBox = std::dynamic_pointer_cast<tgui::TextBox>(widget);
        pairs["Text"] = {"String", textBox->getText()};
        pairs["TextSize"] = {"UInt", tgui::to_string(textBox->getTextSize())};
        pairs["MaximumCharacters"] = {"UInt", tgui::to_string(textBox->getMaximumCharacters())};
        pairs["ReadOnly"] = {"Bool", tgui::Serializer::serialize(textBox->isReadOnly())};
        pairs["VerticalScrollbarPresent"] = {"Bool", tgui::Serializer::serialize(textBox->isVerticalScrollbarPresent())};

        const auto renderer = textBox->getRenderer();
        pairs["Borders"] = {"Outline", renderer->getBorders().toString()};
        pairs["Padding"] = {"Outline", renderer->getPadding().toString()};
        pairs["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pairs["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pairs["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pairs["SelectedTextBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextBackgroundColor())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["CaretColor"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColor())};
        pairs["CaretWidth"] = {"Float", tgui::to_string(renderer->getCaretWidth())};
        pairs["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        return pairs;
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
