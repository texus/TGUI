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
            listBox->setSelectedItemByIndex(tgui::stoi(value));
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

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto listBox = std::dynamic_pointer_cast<tgui::ListBox>(widget);
        pairs["Items"] = {"List<String>", serializeList(listBox->getItems())};
        pairs["SelectedItemIndex"] = {"Int", tgui::to_string(listBox->getSelectedItemIndex())};
        pairs["ItemHeight"] = {"UInt", tgui::to_string(listBox->getItemHeight())};
        pairs["TextSize"] = {"UInt", tgui::to_string(listBox->getTextSize())};
        pairs["MaximumItems"] = {"UInt", tgui::to_string(listBox->getMaximumItems())};
        pairs["AutoScroll"] = {"Bool", tgui::Serializer::serialize(listBox->getAutoScroll())};

        const auto renderer = listBox->getRenderer();
        pairs["Borders"] = {"Outline", renderer->getBorders().toString()};
        pairs["Padding"] = {"Outline", renderer->getPadding().toString()};
        pairs["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pairs["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pairs["SelectedBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBackgroundColor())};
        pairs["SelectedBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBackgroundColorHover())};
        pairs["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pairs["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pairs["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pairs["SelectedTextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColorHover())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pairs["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pairs["SelectedTextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getSelectedTextStyle())};
        return pairs;
    }
};

#endif // TGUI_GUI_BUILDER_LIST_BOX_PROPERTIES_HPP
