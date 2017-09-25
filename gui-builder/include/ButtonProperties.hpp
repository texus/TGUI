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

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto button = std::dynamic_pointer_cast<tgui::Button>(widget);
        pairs["Text"] = {"String", button->getText()};
        pairs["TextSize"] = {"UInt", tgui::to_string(button->getTextSize())};

        const auto renderer = button->getRenderer();
        pairs["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pairs["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pairs["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pairs["TextColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDown())};
        pairs["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pairs["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pairs["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pairs["BackgroundColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDown())};
        pairs["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pairs["BorderColorDown"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDown())};
        pairs["BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDisabled())};
        pairs["Texture"] = {"Texture", tgui::Serializer::serialize(renderer->getTexture())};
        pairs["TextureHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureHover())};
        pairs["TextureDown"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDown())};
        pairs["TextureDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureDisabled())};
        pairs["TextureFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureFocused())};
        pairs["TextStyle"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyle())};
        pairs["TextStyleHover"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleHover())};
        pairs["TextStyleDown"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleDown())};
        pairs["TextStyleDisabled"] = {"TextStyle", tgui::Serializer::serialize(renderer->getTextStyleDisabled())};
        return pairs;
    }
};

#endif // TGUI_GUI_BUILDER_BUTTON_PROPERTIES_HPP
