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


#ifndef TGUI_GUI_BUILDER_TABS_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_TABS_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/Tabs.hpp>

struct TabsProperties : WidgetProperties
{
    // TODO: TabsVisible
    // TODO: TabsEnabled
    // TODO: AutoSize (and TabHeight)?

    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const sf::String& value) const override
    {
        auto tabs = std::dynamic_pointer_cast<tgui::Tabs>(widget);
        if (property == "Tabs")
        {
            tabs->removeAll();
            auto tabTexts = deserializeList(value);
            for (const auto& text : tabTexts)
                tabs->add(text, false);
        }
        else if (property == "Selected")
        {
            if (tgui::strToInt(value) < 0)
                tabs->deselect();
            else
                tabs->select(static_cast<std::size_t>(tgui::strToInt(value.toAnsiString())));
        }
        else if (property == "MaximumTabWidth")
            tabs->setMaximumTabWidth(tgui::strToFloat(value));
        else if (property == "TextSize")
            tabs->setTextSize(static_cast<unsigned int>(tgui::strToInt(value.toAnsiString())));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto tabs = std::dynamic_pointer_cast<tgui::Tabs>(widget);

        std::vector<sf::String> tabTexts;
        for (unsigned int i = 0; i < tabs->getTabsCount(); ++i)
            tabTexts.push_back(tabs->getText(i));

        pair.first["Tabs"] = {"List<String>", serializeList(tabTexts)};
        pair.first["Selected"] = {"Int", tgui::to_string(tabs->getSelectedIndex())};
        pair.first["MaximumTabWidth"] = {"Float", tgui::to_string(tabs->getMaximumTabWidth())};
        pair.first["TextSize"] = {"UInt", tgui::to_string(tabs->getTextSize())};

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
        pair.second["TextureTab"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTab())};
        pair.second["TextureTabHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTabHover())};
        pair.second["TextureSelectedTab"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureSelectedTab())};
        pair.second["TextureSelectedTabHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureSelectedTabHover())};
        pair.second["TextureDisabledTab"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDisabledTab())};
        pair.second["DistanceToSide"] = {"Float", tgui::to_string(renderer->getDistanceToSide())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_TABS_PROPERTIES_HPP
