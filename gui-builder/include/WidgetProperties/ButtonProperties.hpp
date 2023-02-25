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


#ifndef TGUI_GUI_BUILDER_BUTTON_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_BUTTON_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct ButtonProperties : WidgetProperties
{
    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto button = widget->cast<tgui::ButtonBase>(); // Cast to ButtonBase instead of button to reuse code for ToggleButton
        if (property == "Text")
            button->setText(value);
        else if (property == "TextSize")
            button->setTextSize(value.toUInt());
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto button = widget->cast<tgui::ButtonBase>(); // Cast to ButtonBase instead of button to reuse code for ToggleButton
        pair.first["Text"] = {"String", button->getText()};
        pair.first["TextSize"] = {"UInt", tgui::String::fromNumber(button->getTextSize())};

        const auto renderer = button->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDown())};
        pair.second["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pair.second["TextColorDownHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDownHover())};
        pair.second["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pair.second["TextColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorFocused())};
        pair.second["TextColorDownFocused"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDownFocused())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDown())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pair.second["BackgroundColorDownHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDownHover())};
        pair.second["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pair.second["BackgroundColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorFocused())};
        pair.second["BackgroundColorDownFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDownFocused())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BorderColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDown())};
        pair.second["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pair.second["BorderColorDownHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDownHover())};
        pair.second["BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDisabled())};
        pair.second["BorderColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorFocused())};
        pair.second["BorderColorDownFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDownFocused())};
        pair.second["TextOutlineColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextOutlineColor())};
        pair.second["TextOutlineThickness"] = {"Float", tgui::String::fromNumber(renderer->getTextOutlineThickness())};
        pair.second["Texture"] = {"Texture", tgui::Serializer::serialize(renderer->getTexture())};
        pair.second["TextureDown"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDown())};
        pair.second["TextureHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureHover())};
        pair.second["TextureDownHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDownHover())};
        pair.second["TextureDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDisabled())};
        pair.second["TextureFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureFocused())};
        pair.second["TextureDownFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDownFocused())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pair.second["TextStyleDown"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleDown())};
        pair.second["TextStyleHover"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleHover())};
        pair.second["TextStyleDownHover"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleDownHover())};
        pair.second["TextStyleDisabled"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleDisabled())};
        pair.second["TextStyleFocused"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleFocused())};
        pair.second["TextStyleDownFocused"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleDownFocused())};
        pair.second["RoundedBorderRadius"] = {"Float", tgui::String::fromNumber(renderer->getRoundedBorderRadius())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_BUTTON_PROPERTIES_HPP
