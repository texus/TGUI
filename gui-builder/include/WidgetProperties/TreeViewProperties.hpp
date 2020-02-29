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


#ifndef TGUI_GUI_BUILDER_TREE_VIEW_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_TREE_VIEW_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/TreeView.hpp>

struct TreeViewProperties : WidgetProperties
{
    // TODO: Scrollbar renderer

    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const sf::String& value) const override
    {
        auto treeView = std::dynamic_pointer_cast<tgui::TreeView>(widget);
        if (property == "TextSize")
            treeView->setTextSize(static_cast<unsigned int>(tgui::strToInt(value.toAnsiString())));
        else if (property == "ItemHeight")
            treeView->setItemHeight(static_cast<unsigned int>(tgui::strToInt(value.toAnsiString())));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto treeView = std::dynamic_pointer_cast<tgui::TreeView>(widget);
        pair.first["TextSize"] = {"UInt", tgui::to_string(treeView->getTextSize())};
        pair.first["ItemHeight"] = {"UInt", tgui::to_string(treeView->getItemHeight())};

        const auto renderer = treeView->getSharedRenderer();
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
        pair.second["TextureBranchExpanded"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBranchExpanded())};
        pair.second["TextureBranchCollapsed"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBranchCollapsed())};
        pair.second["TextureLeaf"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureLeaf())};
        pair.second["ScrollbarWidth"] = {"Float", tgui::to_string(renderer->getScrollbarWidth())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_TREE_VIEW_PROPERTIES_HPP
