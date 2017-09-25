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
            slider->setMinimum(tgui::stoi(value));
        else if (property == "Maximum")
            slider->setMaximum(tgui::stoi(value));
        else if (property == "Value")
            slider->setValue(tgui::stoi(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto slider = std::dynamic_pointer_cast<tgui::Slider>(widget);
        pairs["Minimum"] = {"Int", tgui::to_string(slider->getMinimum())};
        pairs["Maximum"] = {"Int", tgui::to_string(slider->getMaximum())};
        pairs["Value"] = {"Int", tgui::to_string(slider->getValue())};

        const auto renderer = slider->getRenderer();
        pairs["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pairs["TrackColor"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColor())};
        pairs["TrackColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColorHover())};
        pairs["ThumbColor"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColor())};
        pairs["ThumbColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColorHover())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pairs["TextureTrack"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrack())};
        pairs["TextureTrackHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrackHover())};
        pairs["TextureThumb"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumb())};
        pairs["TextureThumbHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumbHover())};
        return pairs;
    }
};

#endif // TGUI_GUI_BUILDER_SLIDER_PROPERTIES_HPP
