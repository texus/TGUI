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


#ifndef TGUI_GUI_BUILDER_SPIN_BUTTON_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_SPIN_BUTTON_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/SpinButton.hpp>

struct SpinButtonProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto spinButton = std::dynamic_pointer_cast<tgui::SpinButton>(widget);
        if (property == "Minimum")
            spinButton->setMinimum(tgui::stof(value));
        else if (property == "Maximum")
            spinButton->setMaximum(tgui::stof(value));
        else if (property == "Value")
            spinButton->setValue(tgui::stof(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto spinButton = std::dynamic_pointer_cast<tgui::SpinButton>(widget);
        pair.first["Minimum"] = {"Float", tgui::to_string(spinButton->getMinimum())};
        pair.first["Maximum"] = {"Float", tgui::to_string(spinButton->getMaximum())};
        pair.first["Value"] = {"Float", tgui::to_string(spinButton->getValue())};
        pair.first["Step"] = {"Float", tgui::to_string(spinButton->getStep())};

        const auto renderer = spinButton->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["BorderBetweenArrows"] = {"Float", tgui::Serializer::serialize(renderer->getBorderBetweenArrows())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pair.second["ArrowColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColor())};
        pair.second["ArrowColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColorHover())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["TextureArrowUp"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowUp())};
        pair.second["TextureArrowUpHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowUpHover())};
        pair.second["TextureArrowDown"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDown())};
        pair.second["TextureArrowDownHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDownHover())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_SPIN_BUTTON_PROPERTIES_HPP
