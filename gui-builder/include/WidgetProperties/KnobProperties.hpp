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


#ifndef TGUI_GUI_BUILDER_KNOB_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_KNOB_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/Knob.hpp>

struct KnobProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const sf::String& value) const override
    {
        auto knob = std::dynamic_pointer_cast<tgui::Knob>(widget);
        if (property == "Minimum")
            knob->setMinimum(tgui::strToInt(value));
        else if (property == "Maximum")
            knob->setMaximum(tgui::strToInt(value));
        else if (property == "Value")
            knob->setValue(tgui::strToInt(value));
        else if (property == "StartRotation")
            knob->setStartRotation(tgui::strToFloat(value));
        else if (property == "EndRotation")
            knob->setEndRotation(tgui::strToFloat(value));
        else if (property == "ClockwiseTurning")
            knob->setClockwiseTurning(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto knob = std::dynamic_pointer_cast<tgui::Knob>(widget);
        pair.first["Minimum"] = {"Int", tgui::to_string(knob->getMinimum())};
        pair.first["Maximum"] = {"Int", tgui::to_string(knob->getMaximum())};
        pair.first["Value"] = {"Int", tgui::to_string(knob->getValue())};
        pair.first["StartRotation"] = {"Float", tgui::to_string(knob->getStartRotation())};
        pair.first["EndRotation"] = {"Float", tgui::to_string(knob->getEndRotation())};
        pair.first["ClockwiseTurning"] = {"Bool", tgui::Serializer::serialize(knob->getClockwiseTurning())};

        const auto renderer = knob->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["ThumbColor"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColor())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pair.second["TextureForeground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureForeground())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_KNOB_PROPERTIES_HPP
