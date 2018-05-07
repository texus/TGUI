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


#ifndef TGUI_GUI_BUILDER_LIST_BOX_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_LIST_BOX_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/ListBox.hpp>

struct ListBoxProperties : WidgetProperties
{
    // TODO: Item Ids
    // TODO: Scrollbar renderer

    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto listBox = std::dynamic_pointer_cast<tgui::ListBox>(widget);
        if (property == "Items")
        {
            listBox->removeAllItems();
            auto items = deserializeList(value);
            for (const auto& item : items)
                listBox->addItem(item);
        }
        else if (property == "SelectedItemIndex")
            listBox->setSelectedItemByIndex(static_cast<std::size_t>(tgui::stoi(value)));
        else if (property == "ItemHeight")
            listBox->setItemHeight(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "TextSize")
            listBox->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "MaximumItems")
            listBox->setMaximumItems(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "AutoScroll")
            listBox->setAutoScroll(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto listBox = std::dynamic_pointer_cast<tgui::ListBox>(widget);
        pair.first["Items"] = {"List<String>", serializeList(listBox->getItems())};
        pair.first["SelectedItemIndex"] = {"Int", tgui::to_string(listBox->getSelectedItemIndex())};
        pair.first["ItemHeight"] = {"UInt", tgui::to_string(listBox->getItemHeight())};
        pair.first["TextSize"] = {"UInt", tgui::to_string(listBox->getTextSize())};
        pair.first["MaximumItems"] = {"UInt", tgui::to_string(listBox->getMaximumItems())};
        pair.first["AutoScroll"] = {"Bool", tgui::Serializer::serialize(listBox->getAutoScroll())};

        const auto renderer = listBox->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["Padding"] = {"Outline", renderer->getPadding().toString()};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pair.second["SelectedBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBackgroundColor())};
        pair.second["SelectedBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBackgroundColorHover())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pair.second["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pair.second["SelectedTextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColorHover())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pair.second["SelectedTextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getSelectedTextStyle())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_LIST_BOX_PROPERTIES_HPP
