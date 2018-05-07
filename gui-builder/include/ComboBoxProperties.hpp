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


#ifndef TGUI_GUI_BUILDER_COMBO_BOX_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_COMBO_BOX_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/ComboBox.hpp>

struct ComboBoxProperties : WidgetProperties
{
    // TODO: Item Ids
    // TODO: ListBox renderer

    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto comboBox = std::dynamic_pointer_cast<tgui::ComboBox>(widget);
        if (property == "Items")
        {
            comboBox->removeAllItems();
            auto items = deserializeList(value);
            for (const auto& item : items)
                comboBox->addItem(item);
        }
        else if (property == "SelectedItemIndex")
            comboBox->setSelectedItemByIndex(static_cast<std::size_t>(tgui::stoi(value)));
        else if (property == "TextSize")
            comboBox->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "MaximumItems")
            comboBox->setMaximumItems(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "ExpandDirection")
            comboBox->setExpandDirection(deserializeExpandDirection(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto comboBox = std::dynamic_pointer_cast<tgui::ComboBox>(widget);
        pair.first["Items"] = {"List<String>", serializeList(comboBox->getItems())};
        pair.first["SelectedItemIndex"] = {"Int", tgui::to_string(comboBox->getSelectedItemIndex())};
        pair.first["TextSize"] = {"UInt", tgui::to_string(comboBox->getTextSize())};
        pair.first["MaximumItems"] = {"UInt", tgui::to_string(comboBox->getMaximumItems())};
        pair.first["ExpandDirection"] = {"Enum{Down, Up}", serializeExpandDirection(comboBox->getExpandDirection())};

        const auto renderer = comboBox->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["Padding"] = {"Outline", renderer->getPadding().toString()};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["ArrowColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColor())};
        pair.second["ArrowColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColorHover())};
        pair.second["ArrowBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColor())};
        pair.second["ArrowBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColorHover())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pair.second["TextureArrow"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrow())};
        pair.second["TextureArrowHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowHover())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        return pair;
    }


private:

    static tgui::ComboBox::ExpandDirection deserializeExpandDirection(std::string value)
    {
        value = tgui::toLower(tgui::trim(value));
        if (value == "up")
            return tgui::ComboBox::ExpandDirection::Up;
        else
            return tgui::ComboBox::ExpandDirection::Down;
    }

    static std::string serializeExpandDirection(tgui::ComboBox::ExpandDirection alignment)
    {
        if (alignment == tgui::ComboBox::ExpandDirection::Up)
            return "Up";
        else
            return "Down";
    }
};

#endif // TGUI_GUI_BUILDER_COMBO_BOX_PROPERTIES_HPP
