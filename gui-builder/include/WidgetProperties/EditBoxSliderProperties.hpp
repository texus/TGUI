/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_GUI_BUILDER_EDIT_BOX_SLIDER_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_EDIT_BOX_SLIDER_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct EditBoxSliderProperties : WidgetProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto editBoxSlider = widget->cast<tgui::EditBoxSlider>();
        if (property == "Minimum")
            editBoxSlider->setMinimum(value.toFloat());
        else if (property == "Maximum")
            editBoxSlider->setMaximum(value.toFloat());
        else if (property == "Value")
            editBoxSlider->setValue(value.toFloat());
        else if (property == "Step")
            editBoxSlider->setStep(value.toFloat());
        else if (property == "DecimalPlaces")
            editBoxSlider->setDecimalPlaces(value.toUInt());
        else if (property == "TextAlignment")
            editBoxSlider->setTextAlignment(deserializeHorizontalAlignment(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto editBoxSlider = widget->cast<tgui::EditBoxSlider>();
        pair.first["Minimum"] = {"Float", tgui::String::fromNumber(editBoxSlider->getMinimum())};
        pair.first["Maximum"] = {"Float", tgui::String::fromNumber(editBoxSlider->getMaximum())};
        pair.first["Value"] = {"Float", tgui::String::fromNumber(editBoxSlider->getValue())};
        pair.first["Step"] = {"Float", tgui::String::fromNumber(editBoxSlider->getStep())};
        pair.first["Step"] = {"Float", tgui::String::fromNumber(editBoxSlider->getStep())};
        pair.first["DecimalPlaces"] = {"UInt", tgui::String::fromNumber(editBoxSlider->getDecimalPlaces())};
        pair.first["TextAlignment"] = {"Enum{Left,Center,Right}", serializeHorizontalAlignment(editBoxSlider->getTextAlignment())};
        
        const auto editBoxRenderer = editBoxSlider->getEditBoxSharedRenderer();
        pair.second["EditBox.Padding"] = {"Outline", editBoxRenderer->getPadding().toString()};
        pair.second["EditBox.CaretWidth"] = {"Float", tgui::String::fromNumber(editBoxRenderer->getCaretWidth())};
        pair.second["EditBox.TextColor"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getTextColor())};
        pair.second["EditBox.TextColorDisabled"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getTextColorDisabled())};
        pair.second["EditBox.TextColorFocused"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getTextColorFocused())};
        pair.second["EditBox.SelectedTextColor"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getSelectedTextColor())};
        pair.second["EditBox.SelectedTextBackgroundColor"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getSelectedTextBackgroundColor())};
        pair.second["EditBox.DefaultTextColor"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getDefaultTextColor())};
        pair.second["EditBox.BackgroundColor"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getBackgroundColor())};
        pair.second["EditBox.BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getBackgroundColorHover())};
        pair.second["EditBox.BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getBackgroundColorDisabled())};
        pair.second["EditBox.BackgroundColorFocused"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getBackgroundColorFocused())};
        pair.second["EditBox.CaretColor"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getCaretColor())};
        pair.second["EditBox.CaretColorHover"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getCaretColorHover())};
TGUI_IGNORE_DEPRECATED_WARNINGS_START
        pair.second["EditBox.CaretColorFocused"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getCaretColorFocused())};
TGUI_IGNORE_DEPRECATED_WARNINGS_END
        pair.second["EditBox.BorderColor"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getBorderColor())};
        pair.second["EditBox.BorderColorHover"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getBorderColorHover())};
        pair.second["EditBox.BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getBorderColorDisabled())};
        pair.second["EditBox.BorderColorFocused"] = {"Color", tgui::Serializer::serialize(editBoxRenderer->getBorderColorFocused())};
        pair.second["EditBox.Texture"] = {"Texture", tgui::Serializer::serialize(editBoxRenderer->getTexture())};
        pair.second["EditBox.TextureHover"] = {"Texture", tgui::Serializer::serialize(editBoxRenderer->getTextureHover())};
        pair.second["EditBox.TextureDisabled"] = {"Texture", tgui::Serializer::serialize(editBoxRenderer->getTextureDisabled())};
        pair.second["EditBox.TextureFocused"] = {"Texture", tgui::Serializer::serialize(editBoxRenderer->getTextureFocused())};
        pair.second["EditBox.TextStyle"] = {"TextStyle", tgui::Serializer::serialize(editBoxRenderer->getTextStyle())};
        pair.second["EditBox.DefaultTextStyle"] = {"TextStyle", tgui::Serializer::serialize(editBoxRenderer->getDefaultTextStyle())};

        const auto sliderRenderer = editBoxSlider->getSliderSharedRenderer();
        pair.second["Slider.Borders"] = {"Outline", tgui::Serializer::serialize(sliderRenderer->getBorders())};
        pair.second["Slider.TrackColor"] = {"Color", tgui::Serializer::serialize(sliderRenderer->getTrackColor())};
        pair.second["Slider.TrackColorHover"] = {"Color", tgui::Serializer::serialize(sliderRenderer->getTrackColorHover())};
        pair.second["Slider.ThumbColor"] = {"Color", tgui::Serializer::serialize(sliderRenderer->getThumbColor())};
        pair.second["Slider.ThumbColorHover"] = {"Color", tgui::Serializer::serialize(sliderRenderer->getThumbColorHover())};
        pair.second["Slider.BorderColor"] = {"Color", tgui::Serializer::serialize(sliderRenderer->getBorderColor())};
        pair.second["Slider.BorderColorHover"] = {"Color", tgui::Serializer::serialize(sliderRenderer->getBorderColorHover())};
        pair.second["Slider.TextureTrack"] = {"Texture", tgui::Serializer::serialize(sliderRenderer->getTextureTrack())};
        pair.second["Slider.TextureTrackHover"] = {"Texture", tgui::Serializer::serialize(sliderRenderer->getTextureTrackHover())};
        pair.second["Slider.TextureThumb"] = {"Texture", tgui::Serializer::serialize(sliderRenderer->getTextureThumb())};
        pair.second["Slider.TextureThumbHover"] = {"Texture", tgui::Serializer::serialize(sliderRenderer->getTextureThumbHover())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_EDIT_BOX_SLIDER_PROPERTIES_HPP
