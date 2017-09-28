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
            spinButton->setMinimum(tgui::stoi(value));
        else if (property == "Maximum")
            spinButton->setMaximum(tgui::stoi(value));
        else if (property == "Value")
            spinButton->setValue(tgui::stoi(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto spinButton = std::dynamic_pointer_cast<tgui::SpinButton>(widget);
        pairs["Minimum"] = {"Int", tgui::to_string(spinButton->getMinimum())};
        pairs["Maximum"] = {"Int", tgui::to_string(spinButton->getMaximum())};
        pairs["Value"] = {"Int", tgui::to_string(spinButton->getValue())};

        const auto renderer = spinButton->getRenderer();
        pairs["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pairs["SpaceBetweenArrows"] = {"Float", tgui::Serializer::serialize(renderer->getSpaceBetweenArrows())};
        pairs["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pairs["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pairs["ArrowColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColor())};
        pairs["ArrowColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColorHover())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["TextureArrowUp"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowUp())};
        pairs["TextureArrowUpHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowUpHover())};
        pairs["TextureArrowDown"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDown())};
        pairs["TextureArrowDownHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDownHover())};
        return pairs;
    }
};

#endif // TGUI_GUI_BUILDER_SPIN_BUTTON_PROPERTIES_HPP
