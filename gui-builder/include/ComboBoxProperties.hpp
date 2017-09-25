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
            comboBox->setSelectedItemByIndex(tgui::stoi(value));
        else if (property == "TextSize")
            comboBox->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "MaximumItems")
            comboBox->setMaximumItems(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "ExpandDirection")
            comboBox->setExpandDirection(deserializeExpandDirection(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto comboBox = std::dynamic_pointer_cast<tgui::ComboBox>(widget);
        pairs["Items"] = {"List<String>", serializeList(comboBox->getItems())};
        pairs["SelectedItemIndex"] = {"Int", tgui::to_string(comboBox->getSelectedItemIndex())};
        pairs["TextSize"] = {"UInt", tgui::to_string(comboBox->getTextSize())};
        pairs["MaximumItems"] = {"UInt", tgui::to_string(comboBox->getMaximumItems())};
        pairs["ExpandDirection"] = {"Enum{Down, Up}", serializeExpandDirection(comboBox->getExpandDirection())};

        const auto renderer = comboBox->getRenderer();
        pairs["Borders"] = {"Outline", renderer->getBorders().toString()};
        pairs["Padding"] = {"Outline", renderer->getPadding().toString()};
        pairs["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pairs["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pairs["ArrowColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColor())};
        pairs["ArrowColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColorHover())};
        pairs["ArrowBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColor())};
        pairs["ArrowBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColorHover())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pairs["TextureArrow"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrow())};
        pairs["TextureArrowHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowHover())};
        pairs["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        return pairs;
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
