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


#ifndef TGUI_GUI_BUILDER_BUTTON_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_BUTTON_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/Button.hpp>

struct ButtonProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto button = std::dynamic_pointer_cast<tgui::Button>(widget);
        if (property == "Text")
            button->setText(value);
        else if (property == "TextSize")
            button->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto button = std::dynamic_pointer_cast<tgui::Button>(widget);
        pair.first["Text"] = {"String", button->getText()};
        pair.first["TextSize"] = {"UInt", tgui::to_string(button->getTextSize())};

        const auto renderer = button->getSharedRenderer();
        pair.second["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pair.second["TextColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDown())};
        pair.second["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pair.second["TextColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorFocused())};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pair.second["BackgroundColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDown())};
        pair.second["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pair.second["BackgroundColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorFocused())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pair.second["BorderColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDown())};
        pair.second["BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDisabled())};
        pair.second["BorderColorFocused"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorFocused())};
        pair.second["Texture"] = {"Texture", tgui::Serializer::serialize(renderer->getTexture())};
        pair.second["TextureHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureHover())};
        pair.second["TextureDown"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDown())};
        pair.second["TextureDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDisabled())};
        pair.second["TextureFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureFocused())};
        pair.second["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pair.second["TextStyleHover"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleHover())};
        pair.second["TextStyleDown"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleDown())};
        pair.second["TextStyleDisabled"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleDisabled())};
        pair.second["TextStyleFocused"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleFocused())};
        return pair;
    }
};

#endif // TGUI_GUI_BUILDER_BUTTON_PROPERTIES_HPP
