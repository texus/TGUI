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


#ifndef TGUI_GUI_BUILDER_COMBO_BOX_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_COMBO_BOX_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/ComboBox.hpp>

struct ComboBoxProperties : WidgetProperties
{
    // TODO: Item Ids
    // TODO: ListBox renderer

    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const sf::String& value) const override
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
            comboBox->setSelectedItemByIndex(static_cast<std::size_t>(tgui::strToInt(value.toAnsiString())));
        else if (property == "ItemsToDisplay")
            comboBox->setItemsToDisplay(static_cast<std::size_t>(tgui::strToInt(value.toAnsiString())));
        else if (property == "TextSize")
            comboBox->setTextSize(static_cast<unsigned int>(tgui::strToInt(value.toAnsiString())));
        else if (property == "DefaultText")
            comboBox->setDefaultText(value);
        else if (property == "MaximumItems")
            comboBox->setMaximumItems(static_cast<unsigned int>(tgui::strToInt(value.toAnsiString())));
        else if (property == "ExpandDirection")
            comboBox->setExpandDirection(deserializeExpandDirection(value));
        else if (property == "ChangeItemOnScroll")
            comboBox->setChangeItemOnScroll(parseBoolean(value, false));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto comboBox = std::dynamic_pointer_cast<tgui::ComboBox>(widget);
        pair.first["Items"] = {"List<String>", serializeList(comboBox->getItems())};
        pair.first["SelectedItemIndex"] = {"Int", tgui::to_string(comboBox->getSelectedItemIndex())};
        pair.first["ItemsToDisplay"] = {"Int", tgui::to_string(comboBox->getItemsToDisplay())};
        pair.first["TextSize"] = {"UInt", tgui::to_string(comboBox->getTextSize())};
        pair.first["MaximumItems"] = {"UInt", tgui::to_string(comboBox->getMaximumItems())};
        pair.first["ExpandDirection"] = {"Enum{Down, Up, Automatic}", serializeExpandDirection(comboBox->getExpandDirection())};
        pair.first["ChangeItemOnScroll"] = {"Bool", tgui::Serializer::serialize(comboBox->getChangeItemOnScroll())};

        const auto renderer = comboBox->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["Padding"] = {"Outline", renderer->getPadding().toString()};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pair.second["DefaultTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getDefaultTextColor())};
        pair.second["ArrowColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColor())};
        pair.second["ArrowColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColorHover())};
        pair.second["ArrowColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColorDisabled())};
        pair.second["ArrowBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColor())};
        pair.second["ArrowBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColorHover())};
        pair.second["ArrowBackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColorDisabled())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pair.second["TextureBackgroundDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackgroundDisabled())};
        pair.second["TextureArrow"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrow())};
        pair.second["TextureArrowHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowHover())};
        pair.second["TextureArrowDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDisabled())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pair.second["DefaultTextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getDefaultTextStyle())};
        return pair;
    }


private:

    static tgui::ComboBox::ExpandDirection deserializeExpandDirection(std::string value)
    {
        value = tgui::toLower(tgui::trim(value));
        if (value == "up")
            return tgui::ComboBox::ExpandDirection::Up;
        else if (value == "down")
            return tgui::ComboBox::ExpandDirection::Down;
        else
            return tgui::ComboBox::ExpandDirection::Automatic;
    }

    static std::string serializeExpandDirection(tgui::ComboBox::ExpandDirection alignment)
    {
        if (alignment == tgui::ComboBox::ExpandDirection::Up)
            return "Up";
        else if (alignment == tgui::ComboBox::ExpandDirection::Down)
            return "Down";
        else
            return "Automatic";
    }
};

#endif // TGUI_GUI_BUILDER_COMBO_BOX_PROPERTIES_HPP
