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


#ifndef TGUI_GUI_BUILDER_SPIN_CONTROL_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_SPIN_CONTROL_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/SpinControl.hpp>
#include <set>

struct SpinControlProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const tgui::String& property, const tgui::String& value) const override
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
        else
        {
            bool isButtonProperty = m_buttonProperties.count(property);
            bool isTextProperty = m_textProperties.count(property);

            if (isButtonProperty)
            {
                if (property.find("Buttons") == 0)
                {
                    spinControl->getSpinButtonRenderer()->setProperty(property.substr(7), value);
                    return;
                }
                else
                    spinControl->getSpinButtonRenderer()->setProperty(property, value);
            }

            if (isTextProperty)
                spinControl->getSpinTextRenderer()->setProperty(property, value);

            if(!isButtonProperty && !isTextProperty)
                WidgetProperties::updateProperty(widget, property, value);
        }
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto spinControl = widget->cast<tgui::SpinControl>();
        pair.first["Minimum"] = {"Float", tgui::String::fromNumber(spinControl->getMinimum())};
        pair.first["Maximum"] = {"Float", tgui::String::fromNumber(spinControl->getMaximum())};
        pair.first["Value"] = {"Float", tgui::String::fromNumber(spinControl->getValue())};
        pair.first["Step"] = {"Float", tgui::String::fromNumber(spinControl->getStep())};
        pair.first["Step"] = {"Float", tgui::String::fromNumber(spinControl->getStep())};
        pair.first["DecimalPlaces"] = {"UInt", tgui::String::fromNumber(spinControl->getDecimalPlaces())};

        const auto buttonRenderer = spinControl->getSpinButtonSharedRenderer();
        pair.second["ButtonsBorders"] = {"Outline", tgui::Serializer::serialize(buttonRenderer->getBorders())};
        pair.second["BorderBetweenArrows"] = {"Float", tgui::Serializer::serialize(buttonRenderer->getBorderBetweenArrows())};
        pair.second["ButtonsBackgroundColor"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getBackgroundColor())};
        pair.second["ButtonsBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getBackgroundColorHover())};
        pair.second["ArrowColor"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getArrowColor())};
        pair.second["ArrowColorHover"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getArrowColorHover())};
        pair.second["ButtonsBorderColor"] = {"Color", tgui::Serializer::serialize(buttonRenderer->getBorderColor())};
        pair.second["TextureArrowUp"] = {"Texture", tgui::Serializer::serialize(buttonRenderer->getTextureArrowUp())};
        pair.second["TextureArrowUpHover"] = {"Texture", tgui::Serializer::serialize(buttonRenderer->getTextureArrowUpHover())};
        pair.second["TextureArrowDown"] = {"Texture", tgui::Serializer::serialize(buttonRenderer->getTextureArrowDown())};
        pair.second["TextureArrowDownHover"] = {"Texture", tgui::Serializer::serialize(buttonRenderer->getTextureArrowDownHover())};

        const auto textRenderer = spinControl->getSpinTextSharedRenderer();
        pair.second["Padding"] = {"Outline", textRenderer->getPadding().toString()};
        pair.second["CaretWidth"] = {"Float", tgui::String::fromNumber(textRenderer->getCaretWidth())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getTextColor())};
        pair.second["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(textRenderer->getTextColorDisabled())};
        pair.second["TextColorFocused"] = {"Color", tgui::Serializer::serialize(textRenderer->getTextColorFocused())};
        pair.second["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getSelectedTextColor())};
        pair.second["SelectedTextBackgroundColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getSelectedTextBackgroundColor())};
        pair.second["DefaultTextColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getDefaultTextColor())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getBackgroundColor())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(textRenderer->getBackgroundColorHover())};
        pair.second["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(textRenderer->getBackgroundColorDisabled())};
        pair.second["BackgroundColorFocused"] = {"Color", tgui::Serializer::serialize(textRenderer->getBackgroundColorFocused())};
        pair.second["CaretColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getCaretColor())};
        pair.second["CaretColorHover"] = {"Color", tgui::Serializer::serialize(textRenderer->getCaretColorHover())};
        pair.second["CaretColorFocused"] = {"Color", tgui::Serializer::serialize(textRenderer->getCaretColorFocused())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(textRenderer->getBorderColor())};
        pair.second["BorderColorHover"] = {"Color", tgui::Serializer::serialize(textRenderer->getBorderColorHover())};
        pair.second["BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(textRenderer->getBorderColorDisabled())};
        pair.second["BorderColorFocused"] = {"Color", tgui::Serializer::serialize(textRenderer->getBorderColorFocused())};
        pair.second["Texture"] = {"Texture", tgui::Serializer::serialize(textRenderer->getTexture())};
        pair.second["TextureHover"] = {"Texture", tgui::Serializer::serialize(textRenderer->getTextureHover())};
        pair.second["TextureDisabled"] = {"Texture", tgui::Serializer::serialize(textRenderer->getTextureDisabled())};
        pair.second["TextureFocused"] = {"Texture", tgui::Serializer::serialize(textRenderer->getTextureFocused())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(textRenderer->getTextStyle())};
        pair.second["DefaultTextStyle"] = {"TextStyle", tgui::Serializer::serialize(textRenderer->getDefaultTextStyle())};
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
