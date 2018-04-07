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


#ifndef TGUI_GUI_BUILDER_SLIDER_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_SLIDER_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/Slider.hpp>

struct SliderProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto slider = std::dynamic_pointer_cast<tgui::Slider>(widget);
        if (property == "Minimum")
            slider->setMinimum(tgui::stof(value));
        else if (property == "Maximum")
            slider->setMaximum(tgui::stof(value));
        else if (property == "Value")
            slider->setValue(tgui::stof(value));
        else if (property == "Step")
            slider->setStep(tgui::stof(value));
        else if (property == "InvertedDirection")
            slider->setInvertedDirection(parseBoolean(value, false));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto slider = std::dynamic_pointer_cast<tgui::Slider>(widget);
        pair.first["Minimum"] = {"Float", tgui::to_string(slider->getMinimum())};
        pair.first["Maximum"] = {"Float", tgui::to_string(slider->getMaximum())};
        pair.first["Value"] = {"Float", tgui::to_string(slider->getValue())};
        pair.first["Step"] = {"Float", tgui::to_string(slider->getStep())};
        pair.first["InvertedDirection"] = {"Bool", tgui::to_string(slider->getInvertedDirection())};

        const auto renderer = slider->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["TrackColor"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColor())};
        pair.second["TrackColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColorHover())};
        pair.second["ThumbColor"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColor())};
        pair.second["ThumbColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColorHover())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pair.second["TextureTrack"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrack())};
        pair.second["TextureTrackHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrackHover())};
        pair.second["TextureThumb"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumb())};
        pair.second["TextureThumbHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumbHover())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_SLIDER_PROPERTIES_HPP
