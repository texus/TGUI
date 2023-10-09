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


#ifndef TGUI_GUI_BUILDER_PROGRESS_BAR_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_PROGRESS_BAR_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct ProgressBarProperties : WidgetProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto progressBar = widget->cast<tgui::ProgressBar>();
        if (property == "Minimum")
            progressBar->setMinimum(value.toUInt());
        else if (property == "Maximum")
            progressBar->setMaximum(value.toUInt());
        else if (property == "Value")
            progressBar->setValue(value.toUInt());
        else if (property == "Text")
            progressBar->setText(value);
        else if (property == "TextSize")
            progressBar->setTextSize(value.toUInt());
        else if (property == "FillDirection")
            progressBar->setFillDirection(deserializeFillDirection(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto progressBar = widget->cast<tgui::ProgressBar>();
        pair.first["Minimum"] = {"UInt", tgui::String::fromNumber(progressBar->getMinimum())};
        pair.first["Maximum"] = {"UInt", tgui::String::fromNumber(progressBar->getMaximum())};
        pair.first["Value"] = {"UInt", tgui::String::fromNumber(progressBar->getValue())};
        pair.first["Text"] = {"String", progressBar->getText()};
        pair.first["TextSize"] = {"UInt", tgui::String::fromNumber(progressBar->getTextSize())};
        pair.first["FillDirection"] = {"Enum{LeftToRight,RightToLeft,TopToBottom,BottomToTop}", serializeFillDirection(progressBar->getFillDirection())};

        const auto renderer = progressBar->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorFilled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorFilled())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["FillColor"] = {"Color", tgui::Serializer::serialize(renderer->getFillColor())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pair.second["TextureFill"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureFill())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pair.second["TextOutlineColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextOutlineColor())};
        pair.second["TextOutlineThickness"] = {"Float", tgui::String::fromNumber(renderer->getTextOutlineThickness())};
        return pair;
    }

private:

    TGUI_NODISCARD static tgui::ProgressBar::FillDirection deserializeFillDirection(tgui::String value)
    {
        value = value.trim().toLower();
        if (value == "bottomtotop")
            return tgui::ProgressBar::FillDirection::BottomToTop;
        else if (value == "toptobottom")
            return tgui::ProgressBar::FillDirection::TopToBottom;
        else if (value == "righttoleft")
            return tgui::ProgressBar::FillDirection::RightToLeft;
        else
            return tgui::ProgressBar::FillDirection::LeftToRight;
    }

    TGUI_NODISCARD static tgui::String serializeFillDirection(tgui::ProgressBar::FillDirection direction)
    {
        if (direction == tgui::ProgressBar::FillDirection::BottomToTop)
            return "BottomToTop";
        else if (direction == tgui::ProgressBar::FillDirection::TopToBottom)
            return "TopToBottom";
        else if (direction == tgui::ProgressBar::FillDirection::RightToLeft)
            return "RightToLeft";
        else
            return "LeftToRight";
    }
};

#endif // TGUI_GUI_BUILDER_PROGRESS_BAR_PROPERTIES_HPP
