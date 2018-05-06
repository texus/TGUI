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
        else if (property == "ViewportSize")
            scrollbar->setViewportSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "ScrollAmount")
            scrollbar->setScrollAmount(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "AutoHide")
            scrollbar->setAutoHide(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto scrollbar = std::dynamic_pointer_cast<tgui::Scrollbar>(widget);
        pair.first["Maximum"] = {"UInt", tgui::to_string(scrollbar->getMaximum())};
        pair.first["Value"] = {"UInt", tgui::to_string(scrollbar->getValue())};
        pair.first["ViewportSize"] = {"UInt", tgui::to_string(scrollbar->getViewportSize())};
        pair.first["ScrollAmount"] = {"UInt", tgui::to_string(scrollbar->getScrollAmount())};
        pair.first["AutoHide"] = {"Bool", tgui::Serializer::serialize(scrollbar->getAutoHide())};

        const auto renderer = scrollbar->getSharedRenderer();
        pair.second["TrackColor"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColor())};
        pair.second["TrackColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTrackColorHover())};
        pair.second["ThumbColor"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColor())};
        pair.second["ThumbColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getThumbColorHover())};
        pair.second["ArrowBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColor())};
        pair.second["ArrowBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowBackgroundColorHover())};
        pair.second["ArrowColor"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColor())};
        pair.second["ArrowColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getArrowColorHover())};
        pair.second["TextureTrack"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrack())};
        pair.second["TextureTrackHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureTrackHover())};
        pair.second["TextureThumb"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumb())};
        pair.second["TextureThumbHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureThumbHover())};
        pair.second["TextureArrowUp"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowUp())};
        pair.second["TextureArrowUpHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowUpHover())};
        pair.second["TextureArrowDown"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDown())};
        pair.second["TextureArrowDownHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureArrowDownHover())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_SCROLLBAR_PROPERTIES_HPP
