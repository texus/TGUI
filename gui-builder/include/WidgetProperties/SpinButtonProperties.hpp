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


#ifndef TGUI_GUI_BUILDER_SPIN_BUTTON_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_SPIN_BUTTON_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct SpinButtonProperties : WidgetProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto spinButton = widget->cast<tgui::SpinButton>();
        if (property == "Minimum")
            spinButton->setMinimum(value.toFloat());
        else if (property == "Maximum")
            spinButton->setMaximum(value.toFloat());
        else if (property == "Value")
            spinButton->setValue(value.toFloat());
        else if (property == "Step")
            spinButton->setStep(value.toFloat());
        else if (property == "VerticalScroll")
            spinButton->setVerticalScroll(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto spinButton = widget->cast<tgui::SpinButton>();
        pair.first["Minimum"] = {"Float", tgui::String::fromNumber(spinButton->getMinimum())};
        pair.first["Maximum"] = {"Float", tgui::String::fromNumber(spinButton->getMaximum())};
        pair.first["Value"] = {"Float", tgui::String::fromNumber(spinButton->getValue())};
        pair.first["Step"] = {"Float", tgui::String::fromNumber(spinButton->getStep())};
        pair.first["VerticalScroll"] = {"Bool", tgui::Serializer::serialize(spinButton->getVerticalScroll())};

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
