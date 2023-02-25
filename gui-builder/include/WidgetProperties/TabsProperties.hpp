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


#ifndef TGUI_GUI_BUILDER_TABS_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_TABS_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct TabsProperties : WidgetProperties
{
    // TODO: TabsVisible
    // TODO: TabsEnabled

    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto tabs = widget->cast<tgui::Tabs>();
        if (property == "Tabs")
        {
            tabs->removeAll();
            auto tabTexts = deserializeList(value);
            for (const auto& text : tabTexts)
                tabs->add(text, false);
        }
        else if (property == "Selected")
        {
            if (value.toInt() < 0)
                tabs->deselect();
            else
                tabs->select(value.toUInt());
        }
        else if (property == "MaximumTabWidth")
            tabs->setMaximumTabWidth(value.toFloat());
        else if (property == "TabHeight")
            tabs->setTabHeight(value.toFloat());
        else if (property == "AutoSize")
            tabs->setAutoSize(parseBoolean(value, false));
        else if (property == "TextSize")
            tabs->setTextSize(value.toUInt());
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto tabs = widget->cast<tgui::Tabs>();

        std::vector<tgui::String> tabTexts;
        for (unsigned int i = 0; i < tabs->getTabsCount(); ++i)
            tabTexts.push_back(tabs->getText(i));

        pair.first["Tabs"] = {"List<String>", serializeList(tabTexts)};
        pair.first["Selected"] = {"Int", tgui::String::fromNumber(tabs->getSelectedIndex())};
        pair.first["MaximumTabWidth"] = {"Float", tgui::String::fromNumber(tabs->getMaximumTabWidth())};
        pair.first["TabHeight"] = {"Float", tgui::String::fromNumber(tabs->getSize().y)};
        pair.first["AutoSize"] = {"Bool", tgui::Serializer::serialize(tabs->getAutoSize())};
        pair.first["TextSize"] = {"UInt", tgui::String::fromNumber(tabs->getTextSize())};

        const auto renderer = tabs->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pair.second["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pair.second["SelectedBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBackgroundColor())};
        pair.second["SelectedBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBackgroundColorHover())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pair.second["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pair.second["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pair.second["SelectedTextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColorHover())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pair.second["SelectedBorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBorderColor())};
        pair.second["SelectedBorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBorderColorHover())};
        pair.second["TextureTab"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTab())};
        pair.second["TextureTabHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTabHover())};
        pair.second["TextureSelectedTab"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureSelectedTab())};
        pair.second["TextureSelectedTabHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureSelectedTabHover())};
        pair.second["TextureDisabledTab"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDisabledTab())};
        pair.second["DistanceToSide"] = {"Float", tgui::String::fromNumber(renderer->getDistanceToSide())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_TABS_PROPERTIES_HPP
