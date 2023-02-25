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


#ifndef TGUI_GUI_BUILDER_RADIO_BUTTON_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_RADIO_BUTTON_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct RadioButtonProperties : WidgetProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto radioButton = widget->cast<tgui::RadioButton>();
        if (property == "Checked")
            radioButton->setChecked(parseBoolean(value, false));
        else if (property == "Text")
            radioButton->setText(value);
        else if (property == "TextSize")
            radioButton->setTextSize(value.toUInt());
        else if (property == "TextClickable")
            radioButton->setTextClickable(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto radioButton = widget->cast<tgui::RadioButton>();
        pair.first["Checked"] = {"Bool", tgui::Serializer::serialize(radioButton->isChecked())};
        pair.first["Text"] = {"String", radioButton->getText()};
        pair.first["TextSize"] = {"UInt", tgui::String::fromNumber(radioButton->getTextSize())};
        pair.first["TextClickable"] = {"Bool", tgui::Serializer::serialize(radioButton->isTextClickable())};

        const auto renderer = radioButton->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["TextDistanceRatio"] = {"Float", tgui::Serializer::serialize(renderer->getTextDistanceRatio())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pair.second["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pair.second["TextColorChecked"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorChecked())};
        pair.second["TextColorCheckedHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorCheckedHover())};
        pair.second["TextColorCheckedDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorCheckedDisabled())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pair.second["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pair.second["BackgroundColorChecked"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorChecked())};
        pair.second["BackgroundColorCheckedHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorCheckedHover())};
        pair.second["BackgroundColorCheckedDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorCheckedDisabled())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pair.second["BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDisabled())};
        pair.second["BorderColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorFocused())};
        pair.second["BorderColorChecked"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorChecked())};
        pair.second["BorderColorCheckedHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorCheckedHover())};
        pair.second["BorderColorCheckedDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorCheckedDisabled())};
        pair.second["BorderColorCheckedFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorCheckedFocused())};
        pair.second["CheckColor"] = {"Color", tgui::Serializer::serialize(renderer->getCheckColor())};
        pair.second["CheckColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getCheckColorHover())};
        pair.second["CheckColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getCheckColorDisabled())};
        pair.second["TextureUnchecked"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureUnchecked())};
        pair.second["TextureUncheckedHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureUncheckedHover())};
        pair.second["TextureUncheckedDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureUncheckedDisabled())};
        pair.second["TextureUncheckedFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureUncheckedFocused())};
        pair.second["TextureChecked"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureChecked())};
        pair.second["TextureCheckedHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureCheckedHover())};
        pair.second["TextureCheckedDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureCheckedDisabled())};
        pair.second["TextureCheckedFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureCheckedFocused())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_RADIO_BUTTON_PROPERTIES_HPP
