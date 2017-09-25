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


#ifndef TGUI_GUI_BUILDER_RADIO_BUTTON_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_RADIO_BUTTON_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/RadioButton.hpp>

struct RadioButtonProperties : WidgetProperties
{
    void updateProperty(tgui::Widget::Ptr widget, const std::string& property, const std::string& value) const override
    {
        auto radioButton = std::dynamic_pointer_cast<tgui::RadioButton>(widget);
        if (property == "Checked")
        {
            if (parseBoolean(value, false))
                radioButton->check();
            else
                radioButton->uncheck();
        }
        else if (property == "Text")
            radioButton->setText(value);
        else if (property == "TextSize")
            radioButton->setTextSize(static_cast<unsigned int>(tgui::stoi(value)));
        else if (property == "TextClickable")
            radioButton->setTextClickable(parseBoolean(value, true));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    std::map<std::string, std::pair<std::string, std::string>> initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pairs = WidgetProperties::initProperties(widget);
        auto radioButton = std::dynamic_pointer_cast<tgui::RadioButton>(widget);
        pairs["Checked"] = {"Bool", tgui::Serializer::serialize(radioButton->isChecked())};
        pairs["Text"] = {"String", radioButton->getText()};
        pairs["TextSize"] = {"UInt", tgui::to_string(radioButton->getTextSize())};
        pairs["TextClickable"] = {"Bool", tgui::Serializer::serialize(radioButton->isTextClickable())};

        const auto renderer = radioButton->getRenderer();
        pairs["Borders"] = {"Outline", tgui::Serializer::serialize(renderer->getBorders())};
        pairs["TextDistanceRatio"] = {"Float", tgui::Serializer::serialize(renderer->getTextDistanceRatio())};
        pairs["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pairs["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pairs["TextColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorDisabled())};
        pairs["TextColorChecked"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorChecked())};
        pairs["TextColorCheckedHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorCheckedHover())};
        pairs["TextColorCheckedDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorCheckedDisabled())};
        pairs["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pairs["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pairs["BackgroundColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorDisabled())};
        pairs["BackgroundColorChecked"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorChecked())};
        pairs["BackgroundColorCheckedHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorCheckedHover())};
        pairs["BackgroundColorCheckedDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorCheckedDisabled())};
        pairs["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pairs["BorderColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorHover())};
        pairs["BorderColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorDisabled())};
        pairs["BorderColorChecked"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorChecked())};
        pairs["BorderColorCheckedHover"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorCheckedHover())};
        pairs["BorderColorCheckedDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColorCheckedDisabled())};
        pairs["CheckColor"] = {"Color", tgui::Serializer::serialize(renderer->getCheckColor())};
        pairs["CheckColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getCheckColorHover())};
        pairs["CheckColorDisabled"] = {"Color", tgui::Serializer::serialize(renderer->getCheckColorDisabled())};
        pairs["TextureUnchecked"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureUnchecked())};
        pairs["TextureUncheckedHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureUncheckedHover())};
        pairs["TextureUncheckedDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureUncheckedDisabled())};
        pairs["TextureChecked"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureChecked())};
        pairs["TextureCheckedHover"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureCheckedHover())};
        pairs["TextureCheckedDisabled"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureCheckedDisabled())};
        pairs["TextureFocused"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureFocused())};
        return pairs;
    }
};

#endif // TGUI_GUI_BUILDER_RADIO_BUTTON_PROPERTIES_HPP
