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


#ifndef TGUI_GUI_BUILDER_SCROLLBAR_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_SCROLLBAR_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/Scrollbar.hpp>

struct ScrollbarProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto scrollbar = std::dynamic_pointer_cast<tgui::Scrollbar>(widget);
        if (property == "Maximum")
            scrollbar->setMaximum(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "Value")
            scrollbar->setValue(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "LowValue")
            scrollbar->setLowValue(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "ScrollAmount")
            scrollbar->setScrollAmount(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "AutoHide")
            scrollbar->setAutoHide(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto scrollbar = std::dynamic_pointer_cast<tgui::Scrollbar>(widget);
        pairs["Maximum"] = {"UInt", tgui::to_string(scrollbar->getMaximum())};
        pairs["Value"] = {"UInt", tgui::to_string(scrollbar->getValue())};
        pairs["LowValue"] = {"UInt", tgui::to_string(scrollbar->getLowValue())};
        pairs["ScrollAmount"] = {"UInt", tgui::to_string(scrollbar->getScrollAmount())};
        pairs["AutoHide"] = {"Bool", tgui::Serializer::serialize(scrollbar->getAutoHide())};

        const auto renderer = scrollbar->getRenderer();
        pairs["TrackColor"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColor())};
        pairs["TrackColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColorHover())};
        pairs["ThumbColor"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColor())};
        pairs["ThumbColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColorHover())};
        pairs["ArrowBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColor())};
        pairs["ArrowBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColorHover())};
        pairs["ArrowColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColor())};
        pairs["ArrowColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColorHover())};
        pairs["TextureTrack"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrack())};
        pairs["TextureTrackHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrackHover())};
        pairs["TextureThumb"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumb())};
        pairs["TextureThumbHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumbHover())};
        pairs["TextureArrowUp"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowUp())};
        pairs["TextureArrowUpHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowUpHover())};
        pairs["TextureArrowDown"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDown())};
        pairs["TextureArrowDownHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDownHover())};
        return pairs;
    }
};

#endif // TGUI_GUI_BUILDER_SCROLLBAR_PROPERTIES_HPP
