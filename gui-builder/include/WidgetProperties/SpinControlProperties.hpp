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


#ifndef TGUI_GUI_BUILDER_SPIN_CONTROL_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_SPIN_CONTROL_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct SpinControlProperties : WidgetProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto spinControl = widget->cast<tgui::SpinControl>();
        if (property == "Minimum")
            spinControl->setMinimum(value.toFloat());
        else if (property == "Maximum")
            spinControl->setMaximum(value.toFloat());
        else if (property == "Value")
            spinControl->setValue(value.toFloat());
        else if (property == "Step")
            spinControl->setStep(value.toFloat());
        else if (property == "DecimalPlaces")
            spinControl->setDecimalPlaces(value.toUInt());
        else if (property == "UseWideArrows")
            spinControl->setUseWideArrows(parseBoolean(value, false));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto spinControl = widget->cast<tgui::SpinControl>();
        pair.first["Minimum"] = {"Float", tgui::String::fromNumber(spinControl->getMinimum())};
        pair.first["Maximum"] = {"Float", tgui::String::fromNumber(spinControl->getMaximum())};
        pair.first["Value"] = {"Float", tgui::String::fromNumber(spinControl->getValue())};
        pair.first["Step"] = {"Float", tgui::String::fromNumber(spinControl->getStep())};
        pair.first["Step"] = {"Float", tgui::String::fromNumber(spinControl->getStep())};
        pair.first["DecimalPlaces"] = {"UInt", tgui::String::fromNumber(spinControl->getDecimalPlaces())};
        pair.first["UseWideArrows"] = {"Bool", tgui::Serializer::serialize(spinControl->getUseWideArrows())};

        const auto buttonRenderer = spinControl->getSpinButtonSharedRenderer();
        pair.second["SpinButton.ButtonsBorders"] = {"Outline", tgui::Serializer::serialize(buttonRenderer->getBorders())};
        pair.second["SpinButton.BorderBetweenArrows"] = {"Float", tgui::Serializer::serialize(buttonRenderer->getBorderBetweenArrows())};
        pair.second["SpinButton.ButtonsBackgroundColor"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getBackgroundColor())};
        pair.second["SpinButton.ButtonsBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getBackgroundColorHover())};
        pair.second["SpinButton.ArrowColor"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getArrowColor())};
        pair.second["SpinButton.ArrowColorHover"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getArrowColorHover())};
        pair.second["SpinButton.ButtonsBorderColor"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getBorderColor())};
        pair.second["SpinButton.TextureArrowUp"] = {"Texture", tgui::Serializer::serialize(buttonRenderer->getTextureArrowUp())};
        pair.second["SpinButton.TextureArrowUpHover"] = {"Texture", tgui::Serializer::serialize(buttonRenderer->getTextureArrowUpHover())};
        pair.second["SpinButton.TextureArrowDown"] = {"Texture", tgui::Serializer::serialize(buttonRenderer->getTextureArrowDown())};
        pair.second["SpinButton.TextureArrowDownHover"] = {"Texture", tgui::Serializer::serialize(buttonRenderer->getTextureArrowDownHover())};

        const auto textRenderer = spinControl->getSpinTextSharedRenderer();
        pair.second["SpinText.Padding"] = {"Outline", textRenderer->getPadding().toString()};
        pair.second["SpinText.CaretWidth"] = {"Float", tgui::String::fromNumber(textRenderer->getCaretWidth())};
        pair.second["SpinText.TextColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getTextColor())};
        pair.second["SpinText.TextColorDisabled"] = {"Color", tgui::Serializer::serialize(textRenderer->getTextColorDisabled())};
        pair.second["SpinText.TextColorFocused"] = {"Color", tgui::Serializer::serialize(textRenderer->getTextColorFocused())};
        pair.second["SpinText.SelectedTextColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getSelectedTextColor())};
        pair.second["SpinText.SelectedTextBackgroundColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getSelectedTextBackgroundColor())};
        pair.second["SpinText.DefaultTextColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getDefaultTextColor())};
        pair.second["SpinText.BackgroundColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getBackgroundColor())};
        pair.second["SpinText.BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(textRenderer->getBackgroundColorHover())};
        pair.second["SpinText.BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(textRenderer->getBackgroundColorDisabled())};
        pair.second["SpinText.BackgroundColorFocused"] = {"Color", tgui::Serializer::serialize(textRenderer->getBackgroundColorFocused())};
        pair.second["SpinText.CaretColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getCaretColor())};
        pair.second["SpinText.CaretColorHover"] = {"Color", tgui::Serializer::serialize(textRenderer->getCaretColorHover())};
        pair.second["SpinText.CaretColorFocused"] = {"Color", tgui::Serializer::serialize(textRenderer->getCaretColorFocused())};
        pair.second["SpinText.BorderColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getBorderColor())};
        pair.second["SpinText.BorderColorHover"] = {"Color", tgui::Serializer::serialize(textRenderer->getBorderColorHover())};
        pair.second["SpinText.BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(textRenderer->getBorderColorDisabled())};
        pair.second["SpinText.BorderColorFocused"] = {"Color", tgui::Serializer::serialize(textRenderer->getBorderColorFocused())};
        pair.second["SpinText.Texture"] = {"Texture", tgui::Serializer::serialize(textRenderer->getTexture())};
        pair.second["SpinText.TextureHover"] = {"Texture", tgui::Serializer::serialize(textRenderer->getTextureHover())};
        pair.second["SpinText.TextureDisabled"] = {"Texture", tgui::Serializer::serialize(textRenderer->getTextureDisabled())};
        pair.second["SpinText.TextureFocused"] = {"Texture", tgui::Serializer::serialize(textRenderer->getTextureFocused())};
        pair.second["SpinText.TextStyle"] = {"TextStyle", tgui::Serializer::serialize(textRenderer->getTextStyle())};
        pair.second["SpinText.DefaultTextStyle"] = {"TextStyle", tgui::Serializer::serialize(textRenderer->getDefaultTextStyle())};
        return pair;
    }

private:

    const std::set<tgui::String> m_buttonProperties = {
        "ButtonsBorders", "BorderBetweenArrows", "ButtonsBackgroundColor", "ButtonsBackgroundColorHover", "ArrowColor",
        "ArrowColorHover", "ButtonsBorderColor", "TextureArrowUp", "TextureArrowUpHover", "TextureArrowDown",
        "TextureArrowDownHover"
    };

    const std::set<tgui::String> m_textProperties = {
        "Padding", "CaretWidth", "TextColor", "TextColorDisabled", "TextColorFocused", "SelectedTextColor",
        "SelectedTextBackgroundColor", "DefaultTextColor", "BackgroundColor", "BackgroundColorHover",
        "BackgroundColorDisabled", "BackgroundColorFocused", "CaretColor", "CaretColorHover", "CaretColorFocused",
        "BorderColor", "BorderColorHover", "BorderColorDisabled", "BorderColorFocused", "Texture", "TextureHover",
        "TextureDisabled", "TextureFocused", "TextStyle", "DefaultTextStyle", "Borders"
    };
};

#endif // TGUI_GUI_BUILDER_SPIN_CONTROL_PROPERTIES_HPP
